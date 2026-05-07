#pragma once

#include <QObject>
#include <QVector>

#include <atomic>
#include <thread>

#include "midi/FpnMidiEvent.h"

class FpnVst3Host;
class FpnVst2Host;

class FpnWasapiAudioEngine : public QObject
{
    Q_OBJECT

public:
    explicit FpnWasapiAudioEngine(QObject *parent = nullptr);
    ~FpnWasapiAudioEngine() override;

    bool fpnStart();
    void fpnStop();
    bool fpnIsRunning() const;
    QString fpnStatusText() const;
    void fpnSetVst3Host(FpnVst3Host *host);
    void fpnSetVst2Host(FpnVst2Host *host);
    void fpnSetUseVst2Host(bool useVst2Host);
    void fpnSubmitEvents(const QVector<FpnMidiEvent> &events);

signals:
    void fpnRunningChanged();
    void fpnStatusTextChanged();

private:
    void fpnAudioLoop();
    void fpnSetRunning(bool running);
    void fpnSetStatusText(const QString &text);

    FpnVst3Host *m_fpnVst3Host = nullptr;
    FpnVst2Host *m_fpnVst2Host = nullptr;
    std::atomic_bool m_fpnUseVst2Host = false;
    std::atomic_bool m_fpnStopRequested = false;
    std::thread m_fpnAudioThread;
    double m_fpnSampleRate = 44100.0;
    bool m_fpnRunning = false;
    QString m_fpnStatusText;
};
