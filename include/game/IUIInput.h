#ifndef GAME_IUIINPUT_H
#define GAME_IUIINPUT_H

#include <revolution/types.h>

class IUIInput {
public:
    virtual bool IsConnected() const = 0;
    virtual f32 GetStickLength() const = 0;
    virtual f32 GetStickAngle() const = 0;
    virtual u32 GetHold() const = 0;
    virtual u32 GetTrig() const = 0;
    virtual u32 GetRelease() const = 0;
    virtual u32 GetRepeat() const = 0;
};

#endif
