#include "core/FpnPianoController.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QtGlobal>

#ifndef FREEPIANO_DEFAULT_VST3_BUNDLE
#define FREEPIANO_DEFAULT_VST3_BUNDLE ""
#endif

FpnPianoController::FpnPianoController(QObject *parent)
    : QObject(parent)
{
    connect(&m_fpnAudioEngine, &FpnWasapiAudioEngine::fpnRunningChanged, this, &FpnPianoController::fpnAudioRunningChanged);
    connect(&m_fpnAudioEngine, &FpnWasapiAudioEngine::fpnStatusTextChanged, this, &FpnPianoController::fpnUpdateStatusText);
    connect(&m_fpnVst2Host, &FpnVst2Host::fpnInstrumentLoadedChanged, this, &FpnPianoController::fpnInstrumentLoadedChanged);
    connect(&m_fpnVst2Host, &FpnVst2Host::fpnStatusTextChanged, this, &FpnPianoController::fpnUpdateStatusText);
    connect(&m_fpnVst3Host, &FpnVst3Host::fpnInstrumentLoadedChanged, this, &FpnPianoController::fpnInstrumentLoadedChanged);
    connect(&m_fpnVst3Host, &FpnVst3Host::fpnStatusTextChanged, this, &FpnPianoController::fpnUpdateStatusText);
    connect(&m_fpnKeyboardMapper, &FpnKeyboardInputMapper::fpnKeyboardLayoutStatusChanged, this, &FpnPianoController::fpnUpdateStatusText);

    m_fpnAudioEngine.fpnSetVst3Host(&m_fpnVst3Host);
    m_fpnAudioEngine.fpnSetVst2Host(&m_fpnVst2Host);
    m_fpnAudioEngine.fpnStart();
    fpnRefreshInstruments();
    fpnLoadDefaultInstrument();
    fpnUpdateStatusText();
}

bool FpnPianoController::fpnAudioRunning() const
{
    return m_fpnAudioEngine.fpnIsRunning();
}

bool FpnPianoController::fpnInstrumentLoaded() const
{
    if (m_fpnCurrentInstrumentIndex >= 0 && m_fpnCurrentInstrumentIndex < m_fpnInstruments.size()
        && m_fpnInstruments.at(m_fpnCurrentInstrumentIndex).fpnKind == FpnInstrumentDescriptor::FpnKind::FpnVst2) {
        return m_fpnVst2Host.fpnIsInstrumentLoaded();
    }
    return m_fpnVst3Host.fpnIsInstrumentLoaded();
}

QString FpnPianoController::fpnStatusText() const
{
    return m_fpnStatusText;
}

QObject *FpnPianoController::fpnKeyboardMapper()
{
    return &m_fpnKeyboardMapper;
}

QStringList FpnPianoController::fpnAvailableInstruments() const
{
    QStringList names;
    names.reserve(m_fpnInstruments.size());
    for (const auto &instrument : m_fpnInstruments) {
        names.append(instrument.fpnName);
    }
    return names;
}

int FpnPianoController::fpnCurrentInstrumentIndex() const
{
    return m_fpnCurrentInstrumentIndex;
}

QString FpnPianoController::fpnCurrentInstrumentName() const
{
    if (m_fpnCurrentInstrumentIndex < 0 || m_fpnCurrentInstrumentIndex >= m_fpnInstruments.size()) {
        return QString();
    }
    return m_fpnInstruments.at(m_fpnCurrentInstrumentIndex).fpnName;
}

void FpnPianoController::fpnNoteOn(int note, int velocity)
{
    m_fpnRouter.fpnNoteOn(note + m_fpnOctaveShift, velocity, FpnInputSource::FpnKeyboard);
    fpnFlushMidiEvents();
}

void FpnPianoController::fpnNoteOff(int note)
{
    m_fpnRouter.fpnNoteOff(note + m_fpnOctaveShift, FpnInputSource::FpnKeyboard);
    fpnFlushMidiEvents();
}

void FpnPianoController::fpnSetSustain(bool enabled)
{
    m_fpnRouter.fpnSustain(enabled, FpnInputSource::FpnKeyboard);
    fpnFlushMidiEvents();
}

void FpnPianoController::fpnSetOctaveShift(int semitones)
{
    const int clamped = qBound(-24, semitones, 24);
    if (m_fpnOctaveShift == clamped) {
        return;
    }

    fpnPanic();
    m_fpnOctaveShift = clamped;
    fpnUpdateStatusText();
}

void FpnPianoController::fpnPanic()
{
    m_fpnRouter.fpnAllNotesOff(FpnInputSource::FpnSystem);
    fpnFlushMidiEvents();
}

void FpnPianoController::fpnLoadDefaultInstrument()
{
    if (m_fpnInstruments.isEmpty()) {
        m_fpnVst3Host.fpnLoadDefaultInstrument();
        fpnUpdateStatusText();
        return;
    }

    fpnLoadInstrument(0);
}

void FpnPianoController::fpnLoadInstrument(int index)
{
    if (index < 0 || index >= m_fpnInstruments.size()) {
        return;
    }

    fpnPanic();
    const auto &instrument = m_fpnInstruments.at(index);
    bool loaded = false;
    if (instrument.fpnKind == FpnInstrumentDescriptor::FpnKind::FpnVst2) {
        loaded = m_fpnVst2Host.fpnLoadInstrument(instrument.fpnBundlePath);
        m_fpnAudioEngine.fpnSetUseVst2Host(loaded);
    } else {
        loaded = m_fpnVst3Host.fpnLoadInstrument(instrument.fpnBundlePath, instrument.fpnClassName, instrument.fpnName);
        m_fpnAudioEngine.fpnSetUseVst2Host(false);
    }

    if (loaded) {
        if (m_fpnCurrentInstrumentIndex != index) {
            m_fpnCurrentInstrumentIndex = index;
            emit fpnCurrentInstrumentChanged();
        }
        emit fpnInstrumentLoadedChanged();
    }
    fpnUpdateStatusText();
}

void FpnPianoController::fpnRefreshInstruments()
{
    m_fpnInstruments.clear();
    fpnAddInstrumentBundle(QStringLiteral(FREEPIANO_DEFAULT_VST3_BUNDLE));

    const QDir pluginsDir(QCoreApplication::applicationDirPath() + QStringLiteral("/plugins"));
    const QFileInfoList bundles = pluginsDir.entryInfoList(QStringList {QStringLiteral("*.vst3")},
                                                           QDir::Dirs | QDir::NoDotAndDotDot,
                                                           QDir::Name);
    for (const QFileInfo &bundle : bundles) {
        fpnAddInstrumentBundle(bundle.absoluteFilePath());
    }

    const QFileInfoList dlls = pluginsDir.entryInfoList(QStringList {QStringLiteral("*.dll")},
                                                        QDir::Files,
                                                        QDir::Name);
    for (const QFileInfo &dll : dlls) {
        fpnAddVst2Instrument(dll.absoluteFilePath());
    }
    const QFileInfoList pluginChildren = pluginsDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const QFileInfo &child : pluginChildren) {
        const QDir childDir(child.absoluteFilePath());
        const QFileInfoList childDlls = childDir.entryInfoList(QStringList {QStringLiteral("*.dll")},
                                                               QDir::Files,
                                                               QDir::Name);
        for (const QFileInfo &dll : childDlls) {
            fpnAddVst2Instrument(dll.absoluteFilePath());
        }
    }

    if (m_fpnCurrentInstrumentIndex >= m_fpnInstruments.size()) {
        m_fpnCurrentInstrumentIndex = -1;
        emit fpnCurrentInstrumentChanged();
    }

    emit fpnAvailableInstrumentsChanged();
    fpnUpdateStatusText();
}

void FpnPianoController::fpnFlushMidiEvents()
{
    const QVector<FpnMidiEvent> events = m_fpnRouter.fpnTakePendingEvents();
    if (events.isEmpty()) {
        return;
    }

    if (m_fpnCurrentInstrumentIndex >= 0 && m_fpnCurrentInstrumentIndex < m_fpnInstruments.size()
        && m_fpnInstruments.at(m_fpnCurrentInstrumentIndex).fpnKind == FpnInstrumentDescriptor::FpnKind::FpnVst2) {
        m_fpnVst2Host.fpnProcessEvents(events);
    } else {
        m_fpnVst3Host.fpnProcessEvents(events);
    }
    m_fpnAudioEngine.fpnSubmitEvents(events);
}

void FpnPianoController::fpnUpdateStatusText()
{
    const QString sourceText = fpnCurrentInstrumentName().isEmpty()
        ? QStringLiteral("Source: none")
        : QStringLiteral("Source: %1").arg(fpnCurrentInstrumentName());
    const QString instrumentStatus = m_fpnCurrentInstrumentIndex >= 0
        && m_fpnCurrentInstrumentIndex < m_fpnInstruments.size()
        && m_fpnInstruments.at(m_fpnCurrentInstrumentIndex).fpnKind == FpnInstrumentDescriptor::FpnKind::FpnVst2
        ? m_fpnVst2Host.fpnStatusText()
        : m_fpnVst3Host.fpnStatusText();
    const QString layoutStatus = m_fpnKeyboardMapper.fpnKeyboardLayoutStatus().isEmpty()
        ? QStringLiteral("Keyboard map: default")
        : m_fpnKeyboardMapper.fpnKeyboardLayoutStatus();
    const QString text = QStringLiteral("%1 | %2 | %3 | %4 | Octave shift: %5")
                             .arg(m_fpnAudioEngine.fpnStatusText(), instrumentStatus, sourceText, layoutStatus)
                             .arg(m_fpnOctaveShift);
    if (m_fpnStatusText == text) {
        return;
    }

    m_fpnStatusText = text;
    emit fpnStatusTextChanged();
}

void FpnPianoController::fpnAddInstrumentBundle(const QString &bundlePath)
{
    const QFileInfo bundleInfo(bundlePath);
    if (!bundleInfo.exists() || !bundleInfo.isDir()) {
        return;
    }

    const QString canonicalPath = bundleInfo.canonicalFilePath();
    for (const auto &instrument : m_fpnInstruments) {
        if (QFileInfo(instrument.fpnBundlePath).canonicalFilePath() == canonicalPath) {
            return;
        }
    }

    const auto instrumentClasses = FpnVst3Host::fpnScanInstrumentClasses(canonicalPath);
    for (const auto &fpnInstrumentClass : instrumentClasses) {
        QString fpnName = fpnInstrumentClass.fpnName;
        if (!fpnInstrumentClass.fpnVendor.isEmpty()) {
            fpnName = QStringLiteral("%1 - %2").arg(fpnInstrumentClass.fpnVendor, fpnName);
        }

        m_fpnInstruments.append({
            FpnInstrumentDescriptor::FpnKind::FpnVst3,
            fpnName,
            canonicalPath,
            fpnInstrumentClass.fpnName,
        });
    }
}

void FpnPianoController::fpnAddVst2Instrument(const QString &dllPath)
{
    const QFileInfo dllInfo(dllPath);
    if (!dllInfo.exists() || !dllInfo.isFile()) {
        return;
    }

    const QString canonicalPath = dllInfo.canonicalFilePath();
    for (const auto &instrument : m_fpnInstruments) {
        if (QFileInfo(instrument.fpnBundlePath).canonicalFilePath() == canonicalPath) {
            return;
        }
    }

    m_fpnInstruments.append({
        FpnInstrumentDescriptor::FpnKind::FpnVst2,
        QStringLiteral("VST2 - %1").arg(dllInfo.completeBaseName()),
        canonicalPath,
        QString(),
    });
}
