#include "input/FpnKeyboardInputMapper.h"

#include <Qt>

FpnKeyboardInputMapper::FpnKeyboardInputMapper(QObject *parent)
    : QObject(parent)
{
    m_fpnKeyToNote.insert(Qt::Key_A, 60);
    m_fpnKeyToNote.insert(Qt::Key_W, 61);
    m_fpnKeyToNote.insert(Qt::Key_S, 62);
    m_fpnKeyToNote.insert(Qt::Key_E, 63);
    m_fpnKeyToNote.insert(Qt::Key_D, 64);
    m_fpnKeyToNote.insert(Qt::Key_F, 65);
    m_fpnKeyToNote.insert(Qt::Key_T, 66);
    m_fpnKeyToNote.insert(Qt::Key_G, 67);
    m_fpnKeyToNote.insert(Qt::Key_Y, 68);
    m_fpnKeyToNote.insert(Qt::Key_H, 69);
    m_fpnKeyToNote.insert(Qt::Key_U, 70);
    m_fpnKeyToNote.insert(Qt::Key_J, 71);
    m_fpnKeyToNote.insert(Qt::Key_K, 72);
}

int FpnKeyboardInputMapper::fpnNoteForKey(int qtKey) const
{
    return m_fpnKeyToNote.value(qtKey, -1);
}

bool FpnKeyboardInputMapper::fpnIsMapped(int qtKey) const
{
    return m_fpnKeyToNote.contains(qtKey);
}
