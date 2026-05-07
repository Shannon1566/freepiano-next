#pragma once

#include "input/IFpnMidiInputProvider.h"

class FpnNullMidiInputProvider final : public IFpnMidiInputProvider
{
public:
    void fpnStart() override {}
    void fpnStop() override {}
};
