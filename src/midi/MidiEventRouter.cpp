#include "midi/MidiEventRouter.h"

#include <QtGlobal>

namespace {
constexpr std::uint8_t channelStatus(std::uint8_t status)
{
    return status;
}

int clampMidiValue(int value)
{
    return qBound(0, value, 127);
}
}

QVector<MidiEvent> MidiEventRouter::takePendingEvents()
{
    QVector<MidiEvent> events;
    events.swap(m_pendingEvents);
    return events;
}

void MidiEventRouter::noteOn(int note, int velocity, InputSource source)
{
    note = clampMidiValue(note);
    velocity = clampMidiValue(velocity);
    if (m_activeNotes.contains(note)) {
        return;
    }

    m_activeNotes.insert(note);
    m_pendingEvents.push_back(MidiEvent{
        channelStatus(0x90),
        static_cast<std::uint8_t>(note),
        static_cast<std::uint8_t>(velocity),
        0,
        0,
        source,
    });
}

void MidiEventRouter::noteOff(int note, InputSource source)
{
    note = clampMidiValue(note);
    if (!m_activeNotes.remove(note)) {
        return;
    }

    pushNoteOff(note, source);
}

void MidiEventRouter::sustain(bool enabled, InputSource source)
{
    if (m_sustainEnabled == enabled) {
        return;
    }

    m_sustainEnabled = enabled;
    pushController(64, enabled ? 127 : 0, source);
}

void MidiEventRouter::allNotesOff(InputSource source)
{
    const auto notes = m_activeNotes.values();
    for (int note : notes) {
        pushNoteOff(note, source);
    }

    m_activeNotes.clear();
    if (m_sustainEnabled) {
        m_sustainEnabled = false;
        pushController(64, 0, source);
    }
    pushController(123, 0, source);
}

void MidiEventRouter::pushController(int controller, int value, InputSource source)
{
    m_pendingEvents.push_back(MidiEvent{
        channelStatus(0xB0),
        static_cast<std::uint8_t>(clampMidiValue(controller)),
        static_cast<std::uint8_t>(clampMidiValue(value)),
        0,
        0,
        source,
    });
}

void MidiEventRouter::pushNoteOff(int note, InputSource source)
{
    m_pendingEvents.push_back(MidiEvent{
        channelStatus(0x80),
        static_cast<std::uint8_t>(clampMidiValue(note)),
        0,
        0,
        0,
        source,
    });
}
