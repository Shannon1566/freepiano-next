#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

#include "audio/WasapiAudioEngine.h"
#include "input/KeyboardInputMapper.h"
#include "midi/MidiEventRouter.h"
#include "vst2/Vst2Host.h"
#include "vst3/Vst3Host.h"

class PianoController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool audioRunning READ audioRunning NOTIFY audioRunningChanged)
    Q_PROPERTY(bool instrumentLoaded READ instrumentLoaded NOTIFY instrumentLoadedChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(QObject *keyboardMapper READ keyboardMapper CONSTANT)
    Q_PROPERTY(QStringList availableInstruments READ availableInstruments NOTIFY availableInstrumentsChanged)
    Q_PROPERTY(int currentInstrumentIndex READ currentInstrumentIndex NOTIFY currentInstrumentChanged)
    Q_PROPERTY(QString currentInstrumentName READ currentInstrumentName NOTIFY currentInstrumentChanged)

public:
    explicit PianoController(QObject *parent = nullptr);

    bool audioRunning() const;
    bool instrumentLoaded() const;
    QString statusText() const;
    QObject *keyboardMapper();
    QStringList availableInstruments() const;
    int currentInstrumentIndex() const;
    QString currentInstrumentName() const;

    Q_INVOKABLE void noteOn(int note, int velocity = 100);
    Q_INVOKABLE void noteOff(int note);
    Q_INVOKABLE void setSustain(bool enabled);
    Q_INVOKABLE void setOctaveShift(int semitones);
    Q_INVOKABLE void panic();
    Q_INVOKABLE void loadDefaultInstrument();
    Q_INVOKABLE void loadInstrument(int index);
    Q_INVOKABLE void refreshInstruments();

signals:
    void audioRunningChanged();
    void instrumentLoadedChanged();
    void statusTextChanged();
    void availableInstrumentsChanged();
    void currentInstrumentChanged();

private:
    struct InstrumentDescriptor {
        enum class Kind {
            Vst3,
            Vst2
        };

        Kind kind = Kind::Vst3;
        QString name;
        QString bundlePath;
        QString className;
    };

    void flushMidiEvents();
    void updateStatusText();
    void addInstrumentBundle(const QString &bundlePath);
    void addVst2Instrument(const QString &dllPath);

    int m_octaveShift = 0;
    int m_currentInstrumentIndex = -1;
    QString m_statusText;
    QVector<InstrumentDescriptor> m_instruments;
    KeyboardInputMapper m_keyboardMapper;
    MidiEventRouter m_router;
    WasapiAudioEngine m_audioEngine;
    Vst2Host m_vst2Host;
    Vst3Host m_vst3Host;
};
