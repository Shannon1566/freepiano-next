#include "midi/FpnMidiEventRouter.h"

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

QVector<FpnMidiEvent> FpnMidiEventRouter::fpnTakePendingEvents()
{
    QVector<FpnMidiEvent> events;
    events.swap(m_fpnPendingEvents);
    return events;
}

void FpnMidiEventRouter::fpnNoteOn(int note, int velocity, FpnInputSource source)
{
    note = clampMidiValue(note);
    velocity = clampMidiValue(velocity);
    if (m_fpnActiveNotes.contains(note)) {
        return;
    }

    m_fpnActiveNotes.insert(note);
    m_fpnPendingEvents.push_back(FpnMidiEvent{
        channelStatus(0x90),
        static_cast<std::uint8_t>(note),
        static_cast<std::uint8_t>(velocity),
        0,
        0,
        source,
    });
}

void FpnMidiEventRouter::fpnNoteOff(int note, FpnInputSource source)
{
    note = clampMidiValue(note);
    if (!m_fpnActiveNotes.remove(note)) {
        return;
    }

    fpnPushNoteOff(note, source);
}

void FpnMidiEventRouter::fpnSustain(bool enabled, FpnInputSource source)
{
    if (m_fpnSustainEnabled == enabled) {
        return;
    }

    m_fpnSustainEnabled = enabled;
    fpnPushController(64, enabled ? 127 : 0, source);
}

void FpnMidiEventRouter::fpnAllNotesOff(FpnInputSource source)
{
    const auto notes = m_fpnActiveNotes.values();
    for (int note : notes) {
        fpnPushNoteOff(note, source);
    }

    m_fpnActiveNotes.clear();
    if (m_fpnSustainEnabled) {
        m_fpnSustainEnabled = false;
        fpnPushController(64, 0, source);
    }
    fpnPushController(123, 0, source);
}

void FpnMidiEventRouter::fpnPushController(int controller, int value, FpnInputSource source)
{
    m_fpnPendingEvents.push_back(FpnMidiEvent{
        channelStatus(0xB0),
        static_cast<std::uint8_t>(clampMidiValue(controller)),
        static_cast<std::uint8_t>(clampMidiValue(value)),
        0,
        0,
        source,
    });
}

void FpnMidiEventRouter::fpnPushNoteOff(int note, FpnInputSource source)
{
    m_fpnPendingEvents.push_back(FpnMidiEvent{
        channelStatus(0x80),
        static_cast<std::uint8_t>(clampMidiValue(note)),
        0,
        0,
        0,
        source,
    });
}
