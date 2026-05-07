#include "vst3/FpnVst3Host.h"

#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>

#include <algorithm>

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/base/smartpointer.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivstevents.h"
#include "public.sdk/source/vst/hosting/eventlist.h"
#include "public.sdk/source/vst/hosting/hostclasses.h"
#include "public.sdk/source/vst/hosting/module.h"
#include "public.sdk/source/vst/hosting/parameterchanges.h"
#include "public.sdk/source/vst/hosting/plugprovider.h"
#include "public.sdk/source/vst/hosting/processdata.h"

#ifndef FREEPIANO_DEFAULT_VST3_BUNDLE
#define FREEPIANO_DEFAULT_VST3_BUNDLE ""
#endif

namespace {
bool hasInstrumentSubCategory(const VST3::Hosting::ClassInfo &classInfo)
{
    for (const auto &subCategory : classInfo.subCategories()) {
        if (subCategory == Steinberg::Vst::PlugType::kInstrument
            || subCategory.find("Instrument") == 0) {
            return true;
        }
    }
    return false;
}

bool isInstrumentClass(const VST3::Hosting::ClassInfo &classInfo)
{
    return classInfo.category() == kVstAudioEffectClass
        && hasInstrumentSubCategory(classInfo);
}

bool isDefaultPianoClass(const VST3::Hosting::ClassInfo &classInfo)
{
    return isInstrumentClass(classInfo)
        && classInfo.name().find("Piano") != std::string::npos;
}

Steinberg::Vst::Event makeNoteEvent(const FpnMidiEvent &fpnMidiEvent)
{
    Steinberg::Vst::Event event {};
    event.busIndex = 0;
    event.sampleOffset = static_cast<Steinberg::int32>(fpnMidiEvent.fpnSampleOffset);
    event.ppqPosition = 0;
    event.flags = Steinberg::Vst::Event::kIsLive;

    const int fpnStatus = fpnMidiEvent.fpnStatus & 0xF0;
    if (fpnStatus == 0x90 && fpnMidiEvent.fpnData2 > 0) {
        event.type = Steinberg::Vst::Event::kNoteOnEvent;
        event.noteOn.channel = 0;
        event.noteOn.pitch = fpnMidiEvent.fpnData1;
        event.noteOn.tuning = 0.0f;
        event.noteOn.velocity = std::clamp(static_cast<float>(fpnMidiEvent.fpnData2) / 127.0f, 0.0f, 1.0f);
        event.noteOn.length = 0;
        event.noteOn.noteId = -1;
    } else {
        event.type = Steinberg::Vst::Event::kNoteOffEvent;
        event.noteOff.channel = 0;
        event.noteOff.pitch = fpnMidiEvent.fpnData1;
        event.noteOff.velocity = 0.0f;
        event.noteOff.noteId = -1;
        event.noteOff.tuning = 0.0f;
    }

    return event;
}

}

struct FpnVst3Host::FpnImpl {
    QMutex mutex;

    Steinberg::IPtr<Steinberg::Vst::HostApplication> hostApplication;
    VST3::Hosting::Module::Ptr module;
    Steinberg::IPtr<Steinberg::Vst::PlugProvider> plugProvider;
    Steinberg::IPtr<Steinberg::Vst::IComponent> component;
    Steinberg::IPtr<Steinberg::Vst::IEditController> controller;
    Steinberg::IPtr<Steinberg::Vst::IAudioProcessor> processor;
    Steinberg::Vst::HostProcessData processData;
    Steinberg::Vst::EventList eventList {256};
    Steinberg::Vst::ParameterChanges inputParameterChanges;
    Steinberg::Vst::ProcessContext processContext {};
    QVector<FpnMidiEvent> pendingEvents;
    double sampleRate = 0.0;
    int blockSize = 0;
    bool processing = false;

    FpnImpl()
        : hostApplication(Steinberg::owned(new Steinberg::Vst::HostApplication()))
    {
        Steinberg::Vst::PluginContextFactory::instance().setPluginContext(hostApplication);
        processData.inputEvents = &eventList;
        processData.inputParameterChanges = &inputParameterChanges;
        processData.processContext = &processContext;
        processContext.tempo = 120.0;
    }

    ~FpnImpl()
    {
        Steinberg::Vst::PluginContextFactory::instance().setPluginContext(nullptr);
    }

    bool configure(double newSampleRate, int newBlockSize)
    {
        if (!component || !processor) {
            return false;
        }
        if (processing && sampleRate == newSampleRate && blockSize == newBlockSize) {
            return true;
        }
        if (processing) {
            processor->setProcessing(false);
            component->setActive(false);
            processing = false;
        }

        sampleRate = newSampleRate;
        blockSize = newBlockSize;
        processContext.sampleRate = sampleRate;
        processData.prepare(*component, blockSize, Steinberg::Vst::kSample32);

        Steinberg::Vst::ProcessSetup setup {
            Steinberg::Vst::kRealtime,
            Steinberg::Vst::kSample32,
            static_cast<Steinberg::int32>(blockSize),
            sampleRate,
        };
        if (processor->setupProcessing(setup) != Steinberg::kResultOk) {
            return false;
        }
        if (component->setActive(true) != Steinberg::kResultOk) {
            return false;
        }
        if (processor->setProcessing(true) != Steinberg::kResultOk) {
            return false;
        }

        processing = true;
        return true;
    }

    void resetPluginState()
    {
        if (processing) {
            processor->setProcessing(false);
            if (component) {
                component->setActive(false);
            }
            processing = false;
        }

        pendingEvents.clear();
        eventList.clear();
        inputParameterChanges.clearQueue();
        processor = nullptr;
        controller = nullptr;
        component = nullptr;
        plugProvider = nullptr;
        module = nullptr;
        sampleRate = 0.0;
        blockSize = 0;
    }

};

FpnVst3Host::FpnVst3Host(QObject *parent)
    : QObject(parent)
    , m_fpnImpl(std::make_unique<FpnImpl>())
    , m_fpnStatusText(QStringLiteral("Instrument not loaded"))
{
}

FpnVst3Host::~FpnVst3Host() = default;

QVector<FpnVst3Host::FpnInstrumentClass> FpnVst3Host::fpnScanInstrumentClasses(const QString &bundlePath)
{
    QVector<FpnInstrumentClass> instruments;
    const QFileInfo bundleInfo(bundlePath);
    if (!bundleInfo.exists() || !bundleInfo.isDir()) {
        return instruments;
    }

    std::string error;
    const auto module = VST3::Hosting::Module::create(bundleInfo.absoluteFilePath().toStdString(), error);
    if (!module) {
        return instruments;
    }

    const auto factory = module->getFactory();
    for (const auto &classInfo : factory.classInfos()) {
        if (!isInstrumentClass(classInfo)) {
            continue;
        }

        instruments.append({
            QString::fromStdString(classInfo.name()),
            QString::fromStdString(classInfo.vendor()),
            QString::fromStdString(classInfo.subCategoriesString()),
        });
    }

    return instruments;
}

bool FpnVst3Host::fpnLoadDefaultInstrument()
{
    return fpnLoadInstrument(QStringLiteral(FREEPIANO_DEFAULT_VST3_BUNDLE),
                          QString(),
                          QStringLiteral("mda Piano"));
}

bool FpnVst3Host::fpnLoadInstrument(const QString &bundlePath, const QString &className, const QString &displayName)
{
    const QFileInfo selectedBundleInfo(bundlePath);
    if (!selectedBundleInfo.exists() || !selectedBundleInfo.isDir()) {
        fpnSetInstrumentLoaded(false);
        fpnSetStatusText(QStringLiteral("VST3 bundle not found: %1").arg(bundlePath));
        return false;
    }

    QMutexLocker locker(&m_fpnImpl->mutex);
    m_fpnImpl->resetPluginState();
    std::string error;
    m_fpnImpl->module = VST3::Hosting::Module::create(selectedBundleInfo.absoluteFilePath().toStdString(), error);
    if (!m_fpnImpl->module) {
        fpnSetInstrumentLoaded(false);
        fpnSetStatusText(QStringLiteral("Failed to load VST3 module: %1").arg(QString::fromStdString(error)));
        return false;
    }

    const auto factory = m_fpnImpl->module->getFactory();
    VST3::Hosting::ClassInfo selectedClass;
    bool found = false;
    if (!className.isEmpty()) {
        for (const auto &classInfo : factory.classInfos()) {
            if (QString::fromStdString(classInfo.name()) == className && isInstrumentClass(classInfo)) {
                selectedClass = classInfo;
                found = true;
                break;
            }
        }
    } else {
        for (const auto &classInfo : factory.classInfos()) {
            if (isDefaultPianoClass(classInfo)) {
                selectedClass = classInfo;
                found = true;
                break;
            }
        }
    }
    if (!found) {
        for (const auto &classInfo : factory.classInfos()) {
            if (className.isEmpty() && isInstrumentClass(classInfo)) {
                selectedClass = classInfo;
                found = true;
                break;
            }
        }
    }
    if (!found) {
        fpnSetInstrumentLoaded(false);
        fpnSetStatusText(QStringLiteral("No playable instrument class found in VST3 bundle"));
        return false;
    }

    m_fpnImpl->plugProvider = Steinberg::owned(new Steinberg::Vst::PlugProvider(factory, selectedClass, true));
    if (!m_fpnImpl->plugProvider->initialize()) {
        fpnSetInstrumentLoaded(false);
        fpnSetStatusText(QStringLiteral("Failed to initialize VST3 instrument"));
        return false;
    }

    m_fpnImpl->component = Steinberg::owned(m_fpnImpl->plugProvider->getComponent());
    m_fpnImpl->controller = Steinberg::owned(m_fpnImpl->plugProvider->getController());
    m_fpnImpl->processor = Steinberg::FUnknownPtr<Steinberg::Vst::IAudioProcessor>(m_fpnImpl->component);
    if (!m_fpnImpl->component || !m_fpnImpl->processor) {
        fpnSetInstrumentLoaded(false);
        fpnSetStatusText(QStringLiteral("VST3 instrument has no audio processor"));
        return false;
    }

    const Steinberg::int32 audioOutputBuses = m_fpnImpl->component->getBusCount(Steinberg::Vst::kAudio, Steinberg::Vst::kOutput);
    for (Steinberg::int32 bus = 0; bus < audioOutputBuses; ++bus) {
        m_fpnImpl->component->activateBus(Steinberg::Vst::kAudio, Steinberg::Vst::kOutput, bus, true);
    }
    const Steinberg::int32 eventInputBuses = m_fpnImpl->component->getBusCount(Steinberg::Vst::kEvent, Steinberg::Vst::kInput);
    for (Steinberg::int32 bus = 0; bus < eventInputBuses; ++bus) {
        m_fpnImpl->component->activateBus(Steinberg::Vst::kEvent, Steinberg::Vst::kInput, bus, true);
    }

    fpnSetInstrumentLoaded(true);
    const QString loadedName = displayName.isEmpty()
        ? QString::fromStdString(selectedClass.name())
        : displayName;
    fpnSetStatusText(QStringLiteral("%1 loaded").arg(loadedName));
    return true;
}

bool FpnVst3Host::fpnIsInstrumentLoaded() const
{
    return m_fpnInstrumentLoaded;
}

QString FpnVst3Host::fpnStatusText() const
{
    return m_fpnStatusText;
}

void FpnVst3Host::fpnProcessEvents(const QVector<FpnMidiEvent> &events)
{
    if (events.isEmpty()) {
        return;
    }

    QMutexLocker locker(&m_fpnImpl->mutex);
    m_fpnImpl->pendingEvents += events;
}

bool FpnVst3Host::fpnRender(float *interleavedOutput, int frames, int channels, double sampleRate)
{
    if (!interleavedOutput || frames <= 0 || channels <= 0) {
        return false;
    }
    std::fill(interleavedOutput, interleavedOutput + frames * channels, 0.0f);

    QMutexLocker locker(&m_fpnImpl->mutex);
    if (!m_fpnImpl->component || !m_fpnImpl->processor || !m_fpnImpl->configure(sampleRate, frames)) {
        return false;
    }

    m_fpnImpl->eventList.clear();
    for (const FpnMidiEvent &fpnMidiEvent : std::as_const(m_fpnImpl->pendingEvents)) {
        const int fpnStatus = fpnMidiEvent.fpnStatus & 0xF0;
        if (fpnStatus == 0x90 || fpnStatus == 0x80) {
            auto event = makeNoteEvent(fpnMidiEvent);
            m_fpnImpl->eventList.addEvent(event);
        }
    }
    m_fpnImpl->pendingEvents.clear();

    m_fpnImpl->processData.numSamples = frames;
    if (m_fpnImpl->processor->process(m_fpnImpl->processData) != Steinberg::kResultOk) {
        m_fpnImpl->inputParameterChanges.clearQueue();
        m_fpnImpl->eventList.clear();
        return false;
    }

    if (m_fpnImpl->processData.numOutputs <= 0 || !m_fpnImpl->processData.outputs) {
        return true;
    }

    auto &outputBus = m_fpnImpl->processData.outputs[0];
    for (int frame = 0; frame < frames; ++frame) {
        for (int channel = 0; channel < channels; ++channel) {
            const int sourceChannel = std::min<Steinberg::int32>(channel, outputBus.numChannels - 1);
            if (sourceChannel >= 0 && outputBus.channelBuffers32 && outputBus.channelBuffers32[sourceChannel]) {
                interleavedOutput[frame * channels + channel] = outputBus.channelBuffers32[sourceChannel][frame];
            }
        }
    }

    m_fpnImpl->inputParameterChanges.clearQueue();
    m_fpnImpl->eventList.clear();
    return true;
}

void FpnVst3Host::fpnSetInstrumentLoaded(bool loaded)
{
    if (m_fpnInstrumentLoaded == loaded) {
        return;
    }

    m_fpnInstrumentLoaded = loaded;
    emit fpnInstrumentLoadedChanged();
}

void FpnVst3Host::fpnSetStatusText(const QString &text)
{
    if (m_fpnStatusText == text) {
        return;
    }

    m_fpnStatusText = text;
    emit fpnStatusTextChanged();
}
