#pragma once

class IMidiInputProvider
{
public:
    virtual ~IMidiInputProvider() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
};
