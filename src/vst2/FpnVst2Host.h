#pragma once

#include <QObject>
#include <QString>
#include <QVector>

#include <memory>

#include "midi/FpnMidiEvent.h"

class FpnVst2Host : public QObject
{
    Q_OBJECT

public:
    explicit FpnVst2Host(QObject *parent = nullptr);
    ~FpnVst2Host() override;

    bool fpnLoadInstrument(const QString &dllPath);
    void fpnUnloadInstrument();
    bool fpnIsInstrumentLoaded() const;
    QString fpnStatusText() const;
    void fpnProcessEvents(const QVector<FpnMidiEvent> &events);
    bool fpnRender(float *interleavedOutput, int frames, int channels, double sampleRate);

signals:
    void fpnInstrumentLoadedChanged();
    void fpnStatusTextChanged();

private:
    struct FpnImpl;

    void fpnSetInstrumentLoaded(bool loaded);
    void fpnSetStatusText(const QString &text);

    std::unique_ptr<FpnImpl> m_fpnImpl;
    bool m_fpnInstrumentLoaded = false;
    QString m_fpnStatusText;
};
