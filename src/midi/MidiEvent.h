#pragma once

#include <cstdint>

enum class InputSource : std::uint8_t {
    Mouse,
    Keyboard,
    MidiDevice,
    System
};

struct MidiEvent {
    std::uint8_t status = 0;
    std::uint8_t data1 = 0;
    std::uint8_t data2 = 0;
    std::uint8_t data3 = 0;
    std::uint32_t sampleOffset = 0;
    InputSource source = InputSource::System;
};
