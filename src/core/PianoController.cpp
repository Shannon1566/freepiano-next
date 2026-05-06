#include "core/PianoController.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QtGlobal>

#ifndef FREEPIANO_DEFAULT_VST3_BUNDLE
#define FREEPIANO_DEFAULT_VST3_BUNDLE ""
#endif

PianoController::PianoController(QObject *parent)
    : QObject(parent)
{
    connect(&m_audioEngine, &WasapiAudioEngine::runningChanged, this, &PianoController::audioRunningChanged);
    connect(&m_audioEngine, &WasapiAudioEngine::statusTextChanged, this, &PianoController::updateStatusText);
    connect(&m_vst2Host, &Vst2Host::instrumentLoadedChanged, this, &PianoController::instrumentLoadedChanged);
    connect(&m_vst2Host, &Vst2Host::statusTextChanged, this, &PianoController::updateStatusText);
    connect(&m_vst3Host, &Vst3Host::instrumentLoadedChanged, this, &PianoController::instrumentLoadedChanged);
    connect(&m_vst3Host, &Vst3Host::statusTextChanged, this, &PianoController::updateStatusText);

    m_audioEngine.setVst3Host(&m_vst3Host);
    m_audioEngine.setVst2Host(&m_vst2Host);
    m_audioEngine.start();
    refreshInstruments();
    loadDefaultInstrument();
    updateStatusText();
}

bool PianoController::audioRunning() const
{
    return m_audioEngine.isRunning();
}

bool PianoController::instrumentLoaded() const
{
    if (m_currentInstrumentIndex >= 0 && m_currentInstrumentIndex < m_instruments.size()
        && m_instruments.at(m_currentInstrumentIndex).kind == InstrumentDescriptor::Kind::Vst2) {
        return m_vst2Host.isInstrumentLoaded();
    }
    return m_vst3Host.isInstrumentLoaded();
}

QString PianoController::statusText() const
{
    return m_statusText;
}

QObject *PianoController::keyboardMapper()
{
    return &m_keyboardMapper;
}

QStringList PianoController::availableInstruments() const
{
    QStringList names;
    names.reserve(m_instruments.size());
    for (const auto &instrument : m_instruments) {
        names.append(instrument.name);
    }
    return names;
}

int PianoController::currentInstrumentIndex() const
{
    return m_currentInstrumentIndex;
}

QString PianoController::currentInstrumentName() const
{
    if (m_currentInstrumentIndex < 0 || m_currentInstrumentIndex >= m_instruments.size()) {
        return QString();
    }
    return m_instruments.at(m_currentInstrumentIndex).name;
}

void PianoController::noteOn(int note, int velocity)
{
    m_router.noteOn(note + m_octaveShift, velocity, InputSource::Keyboard);
    flushMidiEvents();
}

void PianoController::noteOff(int note)
{
    m_router.noteOff(note + m_octaveShift, InputSource::Keyboard);
    flushMidiEvents();
}

void PianoController::setSustain(bool enabled)
{
    m_router.sustain(enabled, InputSource::Keyboard);
    flushMidiEvents();
}

void PianoController::setOctaveShift(int semitones)
{
    const int clamped = qBound(-24, semitones, 24);
    if (m_octaveShift == clamped) {
        return;
    }

    panic();
    m_octaveShift = clamped;
    updateStatusText();
}

void PianoController::panic()
{
    m_router.allNotesOff(InputSource::System);
    flushMidiEvents();
}

void PianoController::loadDefaultInstrument()
{
    if (m_instruments.isEmpty()) {
        m_vst3Host.loadDefaultInstrument();
        updateStatusText();
        return;
    }

    loadInstrument(0);
}

void PianoController::loadInstrument(int index)
{
    if (index < 0 || index >= m_instruments.size()) {
        return;
    }

    panic();
    const auto &instrument = m_instruments.at(index);
    bool loaded = false;
    if (instrument.kind == InstrumentDescriptor::Kind::Vst2) {
        loaded = m_vst2Host.loadInstrument(instrument.bundlePath);
        m_audioEngine.setUseVst2Host(loaded);
    } else {
        loaded = m_vst3Host.loadInstrument(instrument.bundlePath, instrument.className, instrument.name);
        m_audioEngine.setUseVst2Host(false);
    }

    if (loaded) {
        if (m_currentInstrumentIndex != index) {
            m_currentInstrumentIndex = index;
            emit currentInstrumentChanged();
        }
        emit instrumentLoadedChanged();
    }
    updateStatusText();
}

void PianoController::refreshInstruments()
{
    m_instruments.clear();
    addInstrumentBundle(QStringLiteral(FREEPIANO_DEFAULT_VST3_BUNDLE));

    const QDir pluginsDir(QCoreApplication::applicationDirPath() + QStringLiteral("/plugins"));
    const QFileInfoList bundles = pluginsDir.entryInfoList(QStringList {QStringLiteral("*.vst3")},
                                                           QDir::Dirs | QDir::NoDotAndDotDot,
                                                           QDir::Name);
    for (const QFileInfo &bundle : bundles) {
        addInstrumentBundle(bundle.absoluteFilePath());
    }

    const QFileInfoList dlls = pluginsDir.entryInfoList(QStringList {QStringLiteral("*.dll")},
                                                        QDir::Files,
                                                        QDir::Name);
    for (const QFileInfo &dll : dlls) {
        addVst2Instrument(dll.absoluteFilePath());
    }
    const QFileInfoList pluginChildren = pluginsDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const QFileInfo &child : pluginChildren) {
        const QDir childDir(child.absoluteFilePath());
        const QFileInfoList childDlls = childDir.entryInfoList(QStringList {QStringLiteral("*.dll")},
                                                               QDir::Files,
                                                               QDir::Name);
        for (const QFileInfo &dll : childDlls) {
            addVst2Instrument(dll.absoluteFilePath());
        }
    }

    if (m_currentInstrumentIndex >= m_instruments.size()) {
        m_currentInstrumentIndex = -1;
        emit currentInstrumentChanged();
    }

    emit availableInstrumentsChanged();
    updateStatusText();
}

void PianoController::flushMidiEvents()
{
    const QVector<MidiEvent> events = m_router.takePendingEvents();
    if (events.isEmpty()) {
        return;
    }

    if (m_currentInstrumentIndex >= 0 && m_currentInstrumentIndex < m_instruments.size()
        && m_instruments.at(m_currentInstrumentIndex).kind == InstrumentDescriptor::Kind::Vst2) {
        m_vst2Host.processEvents(events);
    } else {
        m_vst3Host.processEvents(events);
    }
    m_audioEngine.submitEvents(events);
}

void PianoController::updateStatusText()
{
    const QString sourceText = currentInstrumentName().isEmpty()
        ? QStringLiteral("Source: none")
        : QStringLiteral("Source: %1").arg(currentInstrumentName());
    const QString instrumentStatus = m_currentInstrumentIndex >= 0
        && m_currentInstrumentIndex < m_instruments.size()
        && m_instruments.at(m_currentInstrumentIndex).kind == InstrumentDescriptor::Kind::Vst2
        ? m_vst2Host.statusText()
        : m_vst3Host.statusText();
    const QString text = QStringLiteral("%1 | %2 | %3 | Octave shift: %4")
                             .arg(m_audioEngine.statusText(), instrumentStatus, sourceText)
                             .arg(m_octaveShift);
    if (m_statusText == text) {
        return;
    }

    m_statusText = text;
    emit statusTextChanged();
}

void PianoController::addInstrumentBundle(const QString &bundlePath)
{
    const QFileInfo bundleInfo(bundlePath);
    if (!bundleInfo.exists() || !bundleInfo.isDir()) {
        return;
    }

    const QString canonicalPath = bundleInfo.canonicalFilePath();
    for (const auto &instrument : m_instruments) {
        if (QFileInfo(instrument.bundlePath).canonicalFilePath() == canonicalPath) {
            return;
        }
    }

    const auto instrumentClasses = Vst3Host::scanInstrumentClasses(canonicalPath);
    for (const auto &instrumentClass : instrumentClasses) {
        QString name = instrumentClass.name;
        if (!instrumentClass.vendor.isEmpty()) {
            name = QStringLiteral("%1 - %2").arg(instrumentClass.vendor, name);
        }

        m_instruments.append({
            InstrumentDescriptor::Kind::Vst3,
            name,
            canonicalPath,
            instrumentClass.name,
        });
    }
}

void PianoController::addVst2Instrument(const QString &dllPath)
{
    const QFileInfo dllInfo(dllPath);
    if (!dllInfo.exists() || !dllInfo.isFile()) {
        return;
    }

    const QString canonicalPath = dllInfo.canonicalFilePath();
    for (const auto &instrument : m_instruments) {
        if (QFileInfo(instrument.bundlePath).canonicalFilePath() == canonicalPath) {
            return;
        }
    }

    m_instruments.append({
        InstrumentDescriptor::Kind::Vst2,
        QStringLiteral("VST2 - %1").arg(dllInfo.completeBaseName()),
        canonicalPath,
        QString(),
    });
}
