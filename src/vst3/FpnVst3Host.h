#pragma once

#include <QObject>
#include <QString>
#include <QVector>

#include <memory>

#include "midi/FpnMidiEvent.h"

class FpnVst3Host : public QObject
{
    Q_OBJECT

public:
    struct FpnInstrumentClass {
        QString fpnName;
        QString fpnVendor;
        QString fpnSubCategories;
    };

    explicit FpnVst3Host(QObject *parent = nullptr);
    ~FpnVst3Host() override;

    static QVector<FpnInstrumentClass> fpnScanInstrumentClasses(const QString &bundlePath);

    bool fpnLoadDefaultInstrument();
    bool fpnLoadInstrument(const QString &bundlePath, const QString &className, const QString &displayName);
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
