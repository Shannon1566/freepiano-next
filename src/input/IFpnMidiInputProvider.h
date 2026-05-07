#pragma once

class IFpnMidiInputProvider
{
public:
    virtual ~IFpnMidiInputProvider() = default;
    virtual void fpnStart() = 0;
    virtual void fpnStop() = 0;
};
