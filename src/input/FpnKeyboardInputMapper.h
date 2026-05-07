#pragma once

#include <QHash>
#include <QObject>

class FpnKeyboardInputMapper : public QObject
{
    Q_OBJECT

public:
    explicit FpnKeyboardInputMapper(QObject *parent = nullptr);

    Q_INVOKABLE int fpnNoteForKey(int qtKey) const;
    Q_INVOKABLE bool fpnIsMapped(int qtKey) const;

private:
    QHash<int, int> m_fpnKeyToNote;
};
