#pragma once

#include <QSet>
#include <QVector>

#include "midi/FpnMidiEvent.h"

class FpnMidiEventRouter
{
public:
    QVector<FpnMidiEvent> fpnTakePendingEvents();

    void fpnNoteOn(int note, int velocity, FpnInputSource source);
    void fpnNoteOff(int note, FpnInputSource source);
    void fpnSustain(bool enabled, FpnInputSource source);
    void fpnAllNotesOff(FpnInputSource source);

private:
    void fpnPushController(int controller, int value, FpnInputSource source);
    void fpnPushNoteOff(int note, FpnInputSource source);

    QVector<FpnMidiEvent> m_fpnPendingEvents;
    QSet<int> m_fpnActiveNotes;
    bool m_fpnSustainEnabled = false;
};
