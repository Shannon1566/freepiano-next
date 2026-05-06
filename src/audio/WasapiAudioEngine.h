#pragma once

#include <QObject>
#include <QVector>

#include <atomic>
#include <thread>

#include "midi/MidiEvent.h"

class Vst3Host;

class WasapiAudioEngine : public QObject
{
    Q_OBJECT

public:
    explicit WasapiAudioEngine(QObject *parent = nullptr);
    ~WasapiAudioEngine() override;

    bool start();
    void stop();
    bool isRunning() const;
    QString statusText() const;
    void setVst3Host(Vst3Host *host);
    void submitEvents(const QVector<MidiEvent> &events);

signals:
    void runningChanged();
    void statusTextChanged();

private:
    void audioLoop();
    void setRunning(bool running);
    void setStatusText(const QString &text);

    Vst3Host *m_vst3Host = nullptr;
    std::atomic_bool m_stopRequested = false;
    std::thread m_audioThread;
    double m_sampleRate = 44100.0;
    bool m_running = false;
    QString m_statusText;
};
