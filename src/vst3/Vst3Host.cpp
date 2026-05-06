#include "vst3/Vst3Host.h"

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
bool isPianoClass(const VST3::Hosting::ClassInfo &classInfo)
{
    return classInfo.category() == kVstAudioEffectClass
        && classInfo.name().find("Piano") != std::string::npos;
}

Steinberg::Vst::Event makeNoteEvent(const MidiEvent &midiEvent)
{
    Steinberg::Vst::Event event {};
    event.busIndex = 0;
    event.sampleOffset = static_cast<Steinberg::int32>(midiEvent.sampleOffset);
    event.ppqPosition = 0;
    event.flags = Steinberg::Vst::Event::kIsLive;

    const int status = midiEvent.status & 0xF0;
    if (status == 0x90 && midiEvent.data2 > 0) {
        event.type = Steinberg::Vst::Event::kNoteOnEvent;
        event.noteOn.channel = 0;
        event.noteOn.pitch = midiEvent.data1;
        event.noteOn.tuning = 0.0f;
        event.noteOn.velocity = std::clamp(static_cast<float>(midiEvent.data2) / 127.0f, 0.0f, 1.0f);
        event.noteOn.length = 0;
        event.noteOn.noteId = -1;
    } else {
        event.type = Steinberg::Vst::Event::kNoteOffEvent;
        event.noteOff.channel = 0;
        event.noteOff.pitch = midiEvent.data1;
        event.noteOff.velocity = 0.0f;
        event.noteOff.noteId = -1;
        event.noteOff.tuning = 0.0f;
    }

    return event;
}

}

struct Vst3Host::Impl {
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
    QVector<MidiEvent> pendingEvents;
    double sampleRate = 0.0;
    int blockSize = 0;
    bool processing = false;

    Impl()
        : hostApplication(Steinberg::owned(new Steinberg::Vst::HostApplication()))
    {
        Steinberg::Vst::PluginContextFactory::instance().setPluginContext(hostApplication);
        processData.inputEvents = &eventList;
        processData.inputParameterChanges = &inputParameterChanges;
        processData.processContext = &processContext;
        processContext.tempo = 120.0;
    }

    ~Impl()
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

};

Vst3Host::Vst3Host(QObject *parent)
    : QObject(parent)
    , m_impl(std::make_unique<Impl>())
    , m_statusText(QStringLiteral("Instrument not loaded"))
{
}

Vst3Host::~Vst3Host() = default;

bool Vst3Host::loadDefaultInstrument()
{
    const QFileInfo bundleInfo(QStringLiteral(FREEPIANO_DEFAULT_VST3_BUNDLE));
    if (!bundleInfo.exists() || !bundleInfo.isDir()) {
        setInstrumentLoaded(false);
        setStatusText(QStringLiteral("Default mda Piano VST3 bundle not found"));
        return false;
    }

    QMutexLocker locker(&m_impl->mutex);
    std::string error;
    m_impl->module = VST3::Hosting::Module::create(bundleInfo.absoluteFilePath().toStdString(), error);
    if (!m_impl->module) {
        setInstrumentLoaded(false);
        setStatusText(QStringLiteral("Failed to load VST3 module: %1").arg(QString::fromStdString(error)));
        return false;
    }

    const auto factory = m_impl->module->getFactory();
    VST3::Hosting::ClassInfo selectedClass;
    bool found = false;
    for (const auto &classInfo : factory.classInfos()) {
        if (isPianoClass(classInfo)) {
            selectedClass = classInfo;
            found = true;
            break;
        }
    }
    if (!found) {
        setInstrumentLoaded(false);
        setStatusText(QStringLiteral("mda Piano class not found in VST3 bundle"));
        return false;
    }

    m_impl->plugProvider = Steinberg::owned(new Steinberg::Vst::PlugProvider(factory, selectedClass, true));
    if (!m_impl->plugProvider->initialize()) {
        setInstrumentLoaded(false);
        setStatusText(QStringLiteral("Failed to initialize mda Piano VST3"));
        return false;
    }

    m_impl->component = Steinberg::owned(m_impl->plugProvider->getComponent());
    m_impl->controller = Steinberg::owned(m_impl->plugProvider->getController());
    m_impl->processor = Steinberg::FUnknownPtr<Steinberg::Vst::IAudioProcessor>(m_impl->component);
    if (!m_impl->component || !m_impl->processor) {
        setInstrumentLoaded(false);
        setStatusText(QStringLiteral("mda Piano VST3 has no audio processor"));
        return false;
    }

    const Steinberg::int32 audioOutputBuses = m_impl->component->getBusCount(Steinberg::Vst::kAudio, Steinberg::Vst::kOutput);
    for (Steinberg::int32 bus = 0; bus < audioOutputBuses; ++bus) {
        m_impl->component->activateBus(Steinberg::Vst::kAudio, Steinberg::Vst::kOutput, bus, true);
    }
    const Steinberg::int32 eventInputBuses = m_impl->component->getBusCount(Steinberg::Vst::kEvent, Steinberg::Vst::kInput);
    for (Steinberg::int32 bus = 0; bus < eventInputBuses; ++bus) {
        m_impl->component->activateBus(Steinberg::Vst::kEvent, Steinberg::Vst::kInput, bus, true);
    }

    setInstrumentLoaded(true);
    setStatusText(QStringLiteral("mda Piano VST3 loaded"));
    return true;
}

bool Vst3Host::isInstrumentLoaded() const
{
    return m_instrumentLoaded;
}

QString Vst3Host::statusText() const
{
    return m_statusText;
}

void Vst3Host::processEvents(const QVector<MidiEvent> &events)
{
    if (events.isEmpty()) {
        return;
    }

    QMutexLocker locker(&m_impl->mutex);
    m_impl->pendingEvents += events;
}

bool Vst3Host::render(float *interleavedOutput, int frames, int channels, double sampleRate)
{
    if (!interleavedOutput || frames <= 0 || channels <= 0) {
        return false;
    }
    std::fill(interleavedOutput, interleavedOutput + frames * channels, 0.0f);

    QMutexLocker locker(&m_impl->mutex);
    if (!m_impl->component || !m_impl->processor || !m_impl->configure(sampleRate, frames)) {
        return false;
    }

    m_impl->eventList.clear();
    for (const MidiEvent &midiEvent : std::as_const(m_impl->pendingEvents)) {
        const int status = midiEvent.status & 0xF0;
        if (status == 0x90 || status == 0x80) {
            auto event = makeNoteEvent(midiEvent);
            m_impl->eventList.addEvent(event);
        }
    }
    m_impl->pendingEvents.clear();

    m_impl->processData.numSamples = frames;
    if (m_impl->processor->process(m_impl->processData) != Steinberg::kResultOk) {
        m_impl->inputParameterChanges.clearQueue();
        m_impl->eventList.clear();
        return false;
    }

    if (m_impl->processData.numOutputs <= 0 || !m_impl->processData.outputs) {
        return true;
    }

    auto &outputBus = m_impl->processData.outputs[0];
    for (int frame = 0; frame < frames; ++frame) {
        for (int channel = 0; channel < channels; ++channel) {
            const int sourceChannel = std::min<Steinberg::int32>(channel, outputBus.numChannels - 1);
            if (sourceChannel >= 0 && outputBus.channelBuffers32 && outputBus.channelBuffers32[sourceChannel]) {
                interleavedOutput[frame * channels + channel] = outputBus.channelBuffers32[sourceChannel][frame];
            }
        }
    }

    m_impl->inputParameterChanges.clearQueue();
    m_impl->eventList.clear();
    return true;
}

void Vst3Host::setInstrumentLoaded(bool loaded)
{
    if (m_instrumentLoaded == loaded) {
        return;
    }

    m_instrumentLoaded = loaded;
    emit instrumentLoadedChanged();
}

void Vst3Host::setStatusText(const QString &text)
{
    if (m_statusText == text) {
        return;
    }

    m_statusText = text;
    emit statusTextChanged();
}
