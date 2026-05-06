#pragma once

#include <QObject>
#include <QString>
#include <QVector>

#include <memory>

#include "midi/MidiEvent.h"

class Vst2Host : public QObject
{
    Q_OBJECT

public:
    explicit Vst2Host(QObject *parent = nullptr);
    ~Vst2Host() override;

    bool loadInstrument(const QString &dllPath);
    void unloadInstrument();
    bool isInstrumentLoaded() const;
    QString statusText() const;
    void processEvents(const QVector<MidiEvent> &events);
    bool render(float *interleavedOutput, int frames, int channels, double sampleRate);

signals:
    void instrumentLoadedChanged();
    void statusTextChanged();

private:
    struct Impl;

    void setInstrumentLoaded(bool loaded);
    void setStatusText(const QString &text);

    std::unique_ptr<Impl> m_impl;
    bool m_instrumentLoaded = false;
    QString m_statusText;
};
