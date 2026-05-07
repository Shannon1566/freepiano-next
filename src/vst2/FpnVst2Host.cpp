#include "vst2/FpnVst2Host.h"

#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>

#include <algorithm>
#include <atomic>
#include <vector>

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include "vst2/aeffectx.h"

namespace {

std::atomic<int> g_blockSize = 64;
std::atomic<int> g_sampleRate = 44100;

VstIntPtr VSTCALLBACK hostCallback(AEffect *, VstInt32 opcode, VstInt32, VstIntPtr, void *ptr, float)
{
    switch (opcode) {
    case audioMasterVersion:
        return kVstVersion;
    case audioMasterGetSampleRate:
        return g_sampleRate.load();
    case audioMasterGetBlockSize:
        return g_blockSize.load();
    case audioMasterGetCurrentProcessLevel:
        return kVstProcessLevelRealtime;
    case audioMasterGetVendorString:
        if (ptr) {
            vst_strncpy(static_cast<char *>(ptr), "freepiano-next", kVstMaxVendorStrLen);
            return 1;
        }
        return 0;
    case audioMasterGetProductString:
        if (ptr) {
            vst_strncpy(static_cast<char *>(ptr), "freepiano-next", kVstMaxProductStrLen);
            return 1;
        }
        return 0;
    case audioMasterGetVendorVersion:
        return 1;
    case audioMasterCanDo:
        if (ptr) {
            const char *capability = static_cast<const char *>(ptr);
            if (std::strcmp(capability, "sendVstEvents") == 0
                || std::strcmp(capability, "sendVstMidiEvent") == 0
                || std::strcmp(capability, "receiveVstEvents") == 0
                || std::strcmp(capability, "receiveVstMidiEvent") == 0) {
                return 1;
            }
        }
        return 0;
    default:
        return 0;
    }
}

bool isPlayableMidiStatus(const FpnMidiEvent &fpnEvent)
{
    const int fpnStatus = fpnEvent.fpnStatus & 0xF0;
    return fpnStatus == 0x80 || fpnStatus == 0x90 || fpnStatus == 0xB0;
}

}

struct FpnVst2Host::FpnImpl {
    QMutex mutex;

#ifdef Q_OS_WIN
    HMODULE module = nullptr;
#endif
    AEffect *effect = nullptr;
    QVector<FpnMidiEvent> pendingEvents;
    std::vector<float> scratchBuffer;
    std::vector<float> leftBuffer;
    std::vector<float> rightBuffer;
    std::vector<float *> inputPointers;
    std::vector<float *> outputPointers;
    double sampleRate = 0.0;
    int blockSize = 0;
    bool processing = false;

    ~FpnImpl()
    {
        unload();
    }

    void unload()
    {
        if (effect) {
            if (processing) {
                effect->dispatcher(effect, effMainsChanged, 0, 0, nullptr, 0.0f);
                processing = false;
            }
            effect->dispatcher(effect, effClose, 0, 0, nullptr, 0.0f);
            effect = nullptr;
        }
#ifdef Q_OS_WIN
        if (module) {
            FreeLibrary(module);
            module = nullptr;
        }
#endif
        pendingEvents.clear();
        scratchBuffer.clear();
        leftBuffer.clear();
        rightBuffer.clear();
        inputPointers.clear();
        outputPointers.clear();
        sampleRate = 0.0;
        blockSize = 0;
    }

    bool configure(double newSampleRate, int newBlockSize)
    {
        if (!effect || !effect->processReplacing) {
            return false;
        }
        if (processing && sampleRate == newSampleRate && blockSize == newBlockSize) {
            return true;
        }
        if (processing) {
            effect->dispatcher(effect, effMainsChanged, 0, 0, nullptr, 0.0f);
            processing = false;
        }

        sampleRate = newSampleRate;
        blockSize = newBlockSize;
        g_sampleRate.store(static_cast<int>(sampleRate));
        g_blockSize.store(blockSize);

        effect->dispatcher(effect, effSetSampleRate, 0, 0, nullptr, static_cast<float>(sampleRate));
        effect->dispatcher(effect, effSetBlockSize, 0, blockSize, nullptr, 0.0f);
        effect->dispatcher(effect, effMainsChanged, 0, 1, nullptr, 0.0f);
        processing = true;

        scratchBuffer.assign(blockSize, 0.0f);
        leftBuffer.assign(blockSize, 0.0f);
        rightBuffer.assign(blockSize, 0.0f);
        return true;
    }
};

FpnVst2Host::FpnVst2Host(QObject *parent)
    : QObject(parent)
    , m_fpnImpl(std::make_unique<FpnImpl>())
    , m_fpnStatusText(QStringLiteral("VST2 instrument not loaded"))
{
}

FpnVst2Host::~FpnVst2Host() = default;

bool FpnVst2Host::fpnLoadInstrument(const QString &dllPath)
{
#ifndef Q_OS_WIN
    Q_UNUSED(dllPath);
    fpnSetInstrumentLoaded(false);
    fpnSetStatusText(QStringLiteral("VST2 DLL instruments require Windows"));
    return false;
#else
    const QFileInfo dllInfo(dllPath);
    if (!dllInfo.exists() || !dllInfo.isFile()) {
        fpnSetInstrumentLoaded(false);
        fpnSetStatusText(QStringLiteral("VST2 DLL not found: %1").arg(dllPath));
        return false;
    }

    QMutexLocker locker(&m_fpnImpl->mutex);
    m_fpnImpl->unload();

    using FpnPluginEntryProc = AEffect *(VSTCALLBACK *)(audioMasterCallback);

    m_fpnImpl->module = LoadLibraryW(reinterpret_cast<LPCWSTR>(dllInfo.absoluteFilePath().utf16()));
    if (!m_fpnImpl->module) {
        fpnSetInstrumentLoaded(false);
        fpnSetStatusText(QStringLiteral("Failed to load VST2 DLL: %1").arg(dllInfo.fileName()));
        return false;
    }

    auto *mainProc = reinterpret_cast<FpnPluginEntryProc>(GetProcAddress(m_fpnImpl->module, "VSTPluginMain"));
    if (!mainProc) {
        mainProc = reinterpret_cast<FpnPluginEntryProc>(GetProcAddress(m_fpnImpl->module, "main"));
    }
    if (!mainProc) {
        m_fpnImpl->unload();
        fpnSetInstrumentLoaded(false);
        fpnSetStatusText(QStringLiteral("VST2 entry point not found: %1").arg(dllInfo.fileName()));
        return false;
    }

    m_fpnImpl->effect = mainProc(hostCallback);
    if (!m_fpnImpl->effect || m_fpnImpl->effect->magic != kEffectMagic) {
        m_fpnImpl->unload();
        fpnSetInstrumentLoaded(false);
        fpnSetStatusText(QStringLiteral("Invalid VST2 instrument: %1").arg(dllInfo.fileName()));
        return false;
    }
    if (!m_fpnImpl->effect->processReplacing) {
        m_fpnImpl->unload();
        fpnSetInstrumentLoaded(false);
        fpnSetStatusText(QStringLiteral("VST2 instrument does not support processReplacing: %1").arg(dllInfo.fileName()));
        return false;
    }

    m_fpnImpl->effect->dispatcher(m_fpnImpl->effect, effOpen, 0, 0, nullptr, 0.0f);
    m_fpnImpl->effect->dispatcher(m_fpnImpl->effect, effBeginSetProgram, 0, 0, nullptr, 0.0f);
    m_fpnImpl->effect->dispatcher(m_fpnImpl->effect, effSetProgram, 0, 0, nullptr, 0.0f);
    m_fpnImpl->effect->dispatcher(m_fpnImpl->effect, effEndSetProgram, 0, 0, nullptr, 0.0f);

    fpnSetInstrumentLoaded(true);
    fpnSetStatusText(QStringLiteral("%1 loaded").arg(dllInfo.completeBaseName()));
    return true;
#endif
}

void FpnVst2Host::fpnUnloadInstrument()
{
    QMutexLocker locker(&m_fpnImpl->mutex);
    m_fpnImpl->unload();
    fpnSetInstrumentLoaded(false);
    fpnSetStatusText(QStringLiteral("VST2 instrument not loaded"));
}

bool FpnVst2Host::fpnIsInstrumentLoaded() const
{
    return m_fpnInstrumentLoaded;
}

QString FpnVst2Host::fpnStatusText() const
{
    return m_fpnStatusText;
}

void FpnVst2Host::fpnProcessEvents(const QVector<FpnMidiEvent> &events)
{
    if (events.isEmpty()) {
        return;
    }

    QMutexLocker locker(&m_fpnImpl->mutex);
    m_fpnImpl->pendingEvents += events;
}

bool FpnVst2Host::fpnRender(float *interleavedOutput, int frames, int channels, double sampleRate)
{
    if (!interleavedOutput || frames <= 0 || channels <= 0) {
        return false;
    }
    std::fill(interleavedOutput, interleavedOutput + frames * channels, 0.0f);

    QMutexLocker locker(&m_fpnImpl->mutex);
    if (!m_fpnImpl->effect || !m_fpnImpl->configure(sampleRate, frames)) {
        return false;
    }

    struct FpnEventBlock {
        VstInt32 numEvents = 0;
        VstIntPtr reserved = 0;
        VstEvent *events[256] {};
    } FpnEventBlock;
    std::vector<VstMidiEvent> midiEvents;
    midiEvents.reserve(std::min<std::size_t>(static_cast<std::size_t>(m_fpnImpl->pendingEvents.size()), 256));

    for (const FpnMidiEvent &event : std::as_const(m_fpnImpl->pendingEvents)) {
        if (!isPlayableMidiStatus(event) || midiEvents.size() >= 256) {
            continue;
        }

        VstMidiEvent vstEvent {};
        vstEvent.type = kVstMidiType;
        vstEvent.byteSize = sizeof(VstMidiEvent);
        vstEvent.deltaFrames = std::min<int>(event.fpnSampleOffset, frames - 1);
        vstEvent.flags = kVstMidiEventIsRealtime;
        vstEvent.midiData[0] = static_cast<char>(event.fpnStatus);
        vstEvent.midiData[1] = static_cast<char>(event.fpnData1);
        vstEvent.midiData[2] = static_cast<char>(event.fpnData2);
        vstEvent.midiData[3] = static_cast<char>(event.fpnData3);
        midiEvents.push_back(vstEvent);
    }
    for (int i = 0; i < static_cast<int>(midiEvents.size()); ++i) {
        FpnEventBlock.events[i] = reinterpret_cast<VstEvent *>(&midiEvents[static_cast<std::size_t>(i)]);
    }
    FpnEventBlock.numEvents = static_cast<VstInt32>(midiEvents.size());
    m_fpnImpl->effect->dispatcher(m_fpnImpl->effect, effProcessEvents, 0, 0, &FpnEventBlock, 0.0f);
    m_fpnImpl->pendingEvents.clear();

    std::fill(m_fpnImpl->leftBuffer.begin(), m_fpnImpl->leftBuffer.end(), 0.0f);
    std::fill(m_fpnImpl->rightBuffer.begin(), m_fpnImpl->rightBuffer.end(), 0.0f);
    std::fill(m_fpnImpl->scratchBuffer.begin(), m_fpnImpl->scratchBuffer.end(), 0.0f);

    const int inputCount = std::max(0, m_fpnImpl->effect->numInputs);
    const int outputCount = std::max(2, m_fpnImpl->effect->numOutputs);
    m_fpnImpl->inputPointers.assign(static_cast<std::size_t>(inputCount), m_fpnImpl->scratchBuffer.data());
    m_fpnImpl->outputPointers.assign(static_cast<std::size_t>(outputCount), m_fpnImpl->scratchBuffer.data());
    m_fpnImpl->outputPointers[0] = m_fpnImpl->leftBuffer.data();
    m_fpnImpl->outputPointers[1] = m_fpnImpl->rightBuffer.data();

    m_fpnImpl->effect->dispatcher(m_fpnImpl->effect, effStartProcess, 0, 0, nullptr, 0.0f);
    m_fpnImpl->effect->processReplacing(m_fpnImpl->effect,
                                     m_fpnImpl->inputPointers.data(),
                                     m_fpnImpl->outputPointers.data(),
                                     frames);
    m_fpnImpl->effect->dispatcher(m_fpnImpl->effect, effStopProcess, 0, 0, nullptr, 0.0f);

    for (int frame = 0; frame < frames; ++frame) {
        for (int channel = 0; channel < channels; ++channel) {
            interleavedOutput[frame * channels + channel] = channel == 0
                ? m_fpnImpl->leftBuffer[static_cast<std::size_t>(frame)]
                : m_fpnImpl->rightBuffer[static_cast<std::size_t>(frame)];
        }
    }

    return true;
}

void FpnVst2Host::fpnSetInstrumentLoaded(bool loaded)
{
    if (m_fpnInstrumentLoaded == loaded) {
        return;
    }

    m_fpnInstrumentLoaded = loaded;
    emit fpnInstrumentLoadedChanged();
}

void FpnVst2Host::fpnSetStatusText(const QString &text)
{
    if (m_fpnStatusText == text) {
        return;
    }

    m_fpnStatusText = text;
    emit fpnStatusTextChanged();
}
