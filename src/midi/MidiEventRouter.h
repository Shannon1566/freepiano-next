#pragma once

#include <QSet>
#include <QVector>

#include "midi/MidiEvent.h"

class MidiEventRouter
{
public:
    QVector<MidiEvent> takePendingEvents();

    void noteOn(int note, int velocity, InputSource source);
    void noteOff(int note, InputSource source);
    void sustain(bool enabled, InputSource source);
    void allNotesOff(InputSource source);

private:
    void pushController(int controller, int value, InputSource source);
    void pushNoteOff(int note, InputSource source);

    QVector<MidiEvent> m_pendingEvents;
    QSet<int> m_activeNotes;
    bool m_sustainEnabled = false;
};
