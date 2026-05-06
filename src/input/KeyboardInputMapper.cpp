#include "input/KeyboardInputMapper.h"

#include <Qt>

KeyboardInputMapper::KeyboardInputMapper(QObject *parent)
    : QObject(parent)
{
    m_keyToNote.insert(Qt::Key_A, 60);
    m_keyToNote.insert(Qt::Key_W, 61);
    m_keyToNote.insert(Qt::Key_S, 62);
    m_keyToNote.insert(Qt::Key_E, 63);
    m_keyToNote.insert(Qt::Key_D, 64);
    m_keyToNote.insert(Qt::Key_F, 65);
    m_keyToNote.insert(Qt::Key_T, 66);
    m_keyToNote.insert(Qt::Key_G, 67);
    m_keyToNote.insert(Qt::Key_Y, 68);
    m_keyToNote.insert(Qt::Key_H, 69);
    m_keyToNote.insert(Qt::Key_U, 70);
    m_keyToNote.insert(Qt::Key_J, 71);
    m_keyToNote.insert(Qt::Key_K, 72);
}

int KeyboardInputMapper::noteForKey(int qtKey) const
{
    return m_keyToNote.value(qtKey, -1);
}

bool KeyboardInputMapper::isMapped(int qtKey) const
{
    return m_keyToNote.contains(qtKey);
}
