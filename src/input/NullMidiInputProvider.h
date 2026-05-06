#pragma once

#include "input/IMidiInputProvider.h"

class NullMidiInputProvider final : public IMidiInputProvider
{
public:
    void start() override {}
    void stop() override {}
};
