#include "vst2/Vst2Host.h"

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

bool isPlayableMidiStatus(const MidiEvent &event)
{
    const int status = event.status & 0xF0;
    return status == 0x80 || status == 0x90 || status == 0xB0;
}

}

struct Vst2Host::Impl {
    QMutex mutex;

#ifdef Q_OS_WIN
    HMODULE module = nullptr;
#endif
    AEffect *effect = nullptr;
    QVector<MidiEvent> pendingEvents;
    std::vector<float> scratchBuffer;
    std::vector<float> leftBuffer;
    std::vector<float> rightBuffer;
    std::vector<float *> inputPointers;
    std::vector<float *> outputPointers;
    double sampleRate = 0.0;
    int blockSize = 0;
    bool processing = false;

    ~Impl()
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

Vst2Host::Vst2Host(QObject *parent)
    : QObject(parent)
    , m_impl(std::make_unique<Impl>())
    , m_statusText(QStringLiteral("VST2 instrument not loaded"))
{
}

Vst2Host::~Vst2Host() = default;

bool Vst2Host::loadInstrument(const QString &dllPath)
{
#ifndef Q_OS_WIN
    Q_UNUSED(dllPath);
    setInstrumentLoaded(false);
    setStatusText(QStringLiteral("VST2 DLL instruments require Windows"));
    return false;
#else
    const QFileInfo dllInfo(dllPath);
    if (!dllInfo.exists() || !dllInfo.isFile()) {
        setInstrumentLoaded(false);
        setStatusText(QStringLiteral("VST2 DLL not found: %1").arg(dllPath));
        return false;
    }

    QMutexLocker locker(&m_impl->mutex);
    m_impl->unload();

    using PluginEntryProc = AEffect *(VSTCALLBACK *)(audioMasterCallback);

    m_impl->module = LoadLibraryW(reinterpret_cast<LPCWSTR>(dllInfo.absoluteFilePath().utf16()));
    if (!m_impl->module) {
        setInstrumentLoaded(false);
        setStatusText(QStringLiteral("Failed to load VST2 DLL: %1").arg(dllInfo.fileName()));
        return false;
    }

    auto *mainProc = reinterpret_cast<PluginEntryProc>(GetProcAddress(m_impl->module, "VSTPluginMain"));
    if (!mainProc) {
        mainProc = reinterpret_cast<PluginEntryProc>(GetProcAddress(m_impl->module, "main"));
    }
    if (!mainProc) {
        m_impl->unload();
        setInstrumentLoaded(false);
        setStatusText(QStringLiteral("VST2 entry point not found: %1").arg(dllInfo.fileName()));
        return false;
    }

    m_impl->effect = mainProc(hostCallback);
    if (!m_impl->effect || m_impl->effect->magic != kEffectMagic) {
        m_impl->unload();
        setInstrumentLoaded(false);
        setStatusText(QStringLiteral("Invalid VST2 instrument: %1").arg(dllInfo.fileName()));
        return false;
    }
    if (!m_impl->effect->processReplacing) {
        m_impl->unload();
        setInstrumentLoaded(false);
        setStatusText(QStringLiteral("VST2 instrument does not support processReplacing: %1").arg(dllInfo.fileName()));
        return false;
    }

    m_impl->effect->dispatcher(m_impl->effect, effOpen, 0, 0, nullptr, 0.0f);
    m_impl->effect->dispatcher(m_impl->effect, effBeginSetProgram, 0, 0, nullptr, 0.0f);
    m_impl->effect->dispatcher(m_impl->effect, effSetProgram, 0, 0, nullptr, 0.0f);
    m_impl->effect->dispatcher(m_impl->effect, effEndSetProgram, 0, 0, nullptr, 0.0f);

    setInstrumentLoaded(true);
    setStatusText(QStringLiteral("%1 loaded").arg(dllInfo.completeBaseName()));
    return true;
#endif
}

void Vst2Host::unloadInstrument()
{
    QMutexLocker locker(&m_impl->mutex);
    m_impl->unload();
    setInstrumentLoaded(false);
    setStatusText(QStringLiteral("VST2 instrument not loaded"));
}

bool Vst2Host::isInstrumentLoaded() const
{
    return m_instrumentLoaded;
}

QString Vst2Host::statusText() const
{
    return m_statusText;
}

void Vst2Host::processEvents(const QVector<MidiEvent> &events)
{
    if (events.isEmpty()) {
        return;
    }

    QMutexLocker locker(&m_impl->mutex);
    m_impl->pendingEvents += events;
}

bool Vst2Host::render(float *interleavedOutput, int frames, int channels, double sampleRate)
{
    if (!interleavedOutput || frames <= 0 || channels <= 0) {
        return false;
    }
    std::fill(interleavedOutput, interleavedOutput + frames * channels, 0.0f);

    QMutexLocker locker(&m_impl->mutex);
    if (!m_impl->effect || !m_impl->configure(sampleRate, frames)) {
        return false;
    }

    struct EventBlock {
        VstInt32 numEvents = 0;
        VstIntPtr reserved = 0;
        VstEvent *events[256] {};
    } eventBlock;
    std::vector<VstMidiEvent> midiEvents;
    midiEvents.reserve(std::min<std::size_t>(static_cast<std::size_t>(m_impl->pendingEvents.size()), 256));

    for (const MidiEvent &event : std::as_const(m_impl->pendingEvents)) {
        if (!isPlayableMidiStatus(event) || midiEvents.size() >= 256) {
            continue;
        }

        VstMidiEvent vstEvent {};
        vstEvent.type = kVstMidiType;
        vstEvent.byteSize = sizeof(VstMidiEvent);
        vstEvent.deltaFrames = std::min<int>(event.sampleOffset, frames - 1);
        vstEvent.flags = kVstMidiEventIsRealtime;
        vstEvent.midiData[0] = static_cast<char>(event.status);
        vstEvent.midiData[1] = static_cast<char>(event.data1);
        vstEvent.midiData[2] = static_cast<char>(event.data2);
        vstEvent.midiData[3] = static_cast<char>(event.data3);
        midiEvents.push_back(vstEvent);
    }
    for (int i = 0; i < static_cast<int>(midiEvents.size()); ++i) {
        eventBlock.events[i] = reinterpret_cast<VstEvent *>(&midiEvents[static_cast<std::size_t>(i)]);
    }
    eventBlock.numEvents = static_cast<VstInt32>(midiEvents.size());
    m_impl->effect->dispatcher(m_impl->effect, effProcessEvents, 0, 0, &eventBlock, 0.0f);
    m_impl->pendingEvents.clear();

    std::fill(m_impl->leftBuffer.begin(), m_impl->leftBuffer.end(), 0.0f);
    std::fill(m_impl->rightBuffer.begin(), m_impl->rightBuffer.end(), 0.0f);
    std::fill(m_impl->scratchBuffer.begin(), m_impl->scratchBuffer.end(), 0.0f);

    const int inputCount = std::max(0, m_impl->effect->numInputs);
    const int outputCount = std::max(2, m_impl->effect->numOutputs);
    m_impl->inputPointers.assign(static_cast<std::size_t>(inputCount), m_impl->scratchBuffer.data());
    m_impl->outputPointers.assign(static_cast<std::size_t>(outputCount), m_impl->scratchBuffer.data());
    m_impl->outputPointers[0] = m_impl->leftBuffer.data();
    m_impl->outputPointers[1] = m_impl->rightBuffer.data();

    m_impl->effect->dispatcher(m_impl->effect, effStartProcess, 0, 0, nullptr, 0.0f);
    m_impl->effect->processReplacing(m_impl->effect,
                                     m_impl->inputPointers.data(),
                                     m_impl->outputPointers.data(),
                                     frames);
    m_impl->effect->dispatcher(m_impl->effect, effStopProcess, 0, 0, nullptr, 0.0f);

    for (int frame = 0; frame < frames; ++frame) {
        for (int channel = 0; channel < channels; ++channel) {
            interleavedOutput[frame * channels + channel] = channel == 0
                ? m_impl->leftBuffer[static_cast<std::size_t>(frame)]
                : m_impl->rightBuffer[static_cast<std::size_t>(frame)];
        }
    }

    return true;
}

void Vst2Host::setInstrumentLoaded(bool loaded)
{
    if (m_instrumentLoaded == loaded) {
        return;
    }

    m_instrumentLoaded = loaded;
    emit instrumentLoadedChanged();
}

void Vst2Host::setStatusText(const QString &text)
{
    if (m_statusText == text) {
        return;
    }

    m_statusText = text;
    emit statusTextChanged();
}
