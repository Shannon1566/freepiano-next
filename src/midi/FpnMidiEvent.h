#pragma once

#include <cstdint>

enum class FpnInputSource : std::uint8_t {
    FpnMouse,
    FpnKeyboard,
    FpnMidiDevice,
    FpnSystem
};

struct FpnMidiEvent {
    std::uint8_t fpnStatus = 0;
    std::uint8_t fpnData1 = 0;
    std::uint8_t fpnData2 = 0;
    std::uint8_t fpnData3 = 0;
    std::uint32_t fpnSampleOffset = 0;
    FpnInputSource fpnSource = FpnInputSource::FpnSystem;
};
