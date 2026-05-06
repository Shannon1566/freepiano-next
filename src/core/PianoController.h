#pragma once

#include <QObject>
#include <QString>

#include "audio/WasapiAudioEngine.h"
#include "input/KeyboardInputMapper.h"
#include "midi/MidiEventRouter.h"
#include "vst3/Vst3Host.h"

class PianoController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool audioRunning READ audioRunning NOTIFY audioRunningChanged)
    Q_PROPERTY(bool instrumentLoaded READ instrumentLoaded NOTIFY instrumentLoadedChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(QObject *keyboardMapper READ keyboardMapper CONSTANT)

public:
    explicit PianoController(QObject *parent = nullptr);

    bool audioRunning() const;
    bool instrumentLoaded() const;
    QString statusText() const;
    QObject *keyboardMapper();

    Q_INVOKABLE void noteOn(int note, int velocity = 100);
    Q_INVOKABLE void noteOff(int note);
    Q_INVOKABLE void setSustain(bool enabled);
    Q_INVOKABLE void setOctaveShift(int semitones);
    Q_INVOKABLE void panic();
    Q_INVOKABLE void loadDefaultInstrument();

signals:
    void audioRunningChanged();
    void instrumentLoadedChanged();
    void statusTextChanged();

private:
    void flushMidiEvents();
    void updateStatusText();

    int m_octaveShift = 0;
    QString m_statusText;
    KeyboardInputMapper m_keyboardMapper;
    MidiEventRouter m_router;
    WasapiAudioEngine m_audioEngine;
    Vst3Host m_vst3Host;
};
