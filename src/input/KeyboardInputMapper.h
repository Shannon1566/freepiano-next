#pragma once

#include <QHash>
#include <QObject>

class KeyboardInputMapper : public QObject
{
    Q_OBJECT

public:
    explicit KeyboardInputMapper(QObject *parent = nullptr);

    Q_INVOKABLE int noteForKey(int qtKey) const;
    Q_INVOKABLE bool isMapped(int qtKey) const;

private:
    QHash<int, int> m_keyToNote;
};
