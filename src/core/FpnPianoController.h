#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

#include "audio/FpnWasapiAudioEngine.h"
#include "input/FpnKeyboardInputMapper.h"
#include "midi/FpnMidiEventRouter.h"
#include "vst2/FpnVst2Host.h"
#include "vst3/FpnVst3Host.h"

class FpnPianoController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool fpnAudioRunning READ fpnAudioRunning NOTIFY fpnAudioRunningChanged)
    Q_PROPERTY(bool fpnInstrumentLoaded READ fpnInstrumentLoaded NOTIFY fpnInstrumentLoadedChanged)
    Q_PROPERTY(QString fpnStatusText READ fpnStatusText NOTIFY fpnStatusTextChanged)
    Q_PROPERTY(QObject *fpnKeyboardMapper READ fpnKeyboardMapper CONSTANT)
    Q_PROPERTY(QStringList fpnAvailableInstruments READ fpnAvailableInstruments NOTIFY fpnAvailableInstrumentsChanged)
    Q_PROPERTY(int fpnCurrentInstrumentIndex READ fpnCurrentInstrumentIndex NOTIFY fpnCurrentInstrumentChanged)
    Q_PROPERTY(QString fpnCurrentInstrumentName READ fpnCurrentInstrumentName NOTIFY fpnCurrentInstrumentChanged)

public:
    explicit FpnPianoController(QObject *parent = nullptr);

    bool fpnAudioRunning() const;
    bool fpnInstrumentLoaded() const;
    QString fpnStatusText() const;
    QObject *fpnKeyboardMapper();
    QStringList fpnAvailableInstruments() const;
    int fpnCurrentInstrumentIndex() const;
    QString fpnCurrentInstrumentName() const;

    Q_INVOKABLE void fpnNoteOn(int note, int velocity = 100);
    Q_INVOKABLE void fpnNoteOff(int note);
    Q_INVOKABLE void fpnSetSustain(bool enabled);
    Q_INVOKABLE void fpnSetOctaveShift(int semitones);
    Q_INVOKABLE void fpnPanic();
    Q_INVOKABLE void fpnLoadDefaultInstrument();
    Q_INVOKABLE void fpnLoadInstrument(int index);
    Q_INVOKABLE void fpnRefreshInstruments();

signals:
    void fpnAudioRunningChanged();
    void fpnInstrumentLoadedChanged();
    void fpnStatusTextChanged();
    void fpnAvailableInstrumentsChanged();
    void fpnCurrentInstrumentChanged();

private:
    struct FpnInstrumentDescriptor {
        enum class FpnKind {
            FpnVst3,
            FpnVst2
        };

        FpnKind fpnKind = FpnKind::FpnVst3;
        QString fpnName;
        QString fpnBundlePath;
        QString fpnClassName;
    };

    void fpnFlushMidiEvents();
    void fpnUpdateStatusText();
    void fpnAddInstrumentBundle(const QString &bundlePath);
    void fpnAddVst2Instrument(const QString &dllPath);

    int m_fpnOctaveShift = 0;
    int m_fpnCurrentInstrumentIndex = -1;
    QString m_fpnStatusText;
    QVector<FpnInstrumentDescriptor> m_fpnInstruments;
    FpnKeyboardInputMapper m_fpnKeyboardMapper;
    FpnMidiEventRouter m_fpnRouter;
    FpnWasapiAudioEngine m_fpnAudioEngine;
    FpnVst2Host m_fpnVst2Host;
    FpnVst3Host m_fpnVst3Host;
};
