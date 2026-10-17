#ifndef GAME_ICONTROLLER_H
#define GAME_ICONTROLLER_H

#include <game/IUIInput.h>

struct MotorPattern;

class IController : public IUIInput {
public:
    enum Type {
        TYPE_GC_PAD,
        TYPE_WII_REMOTE,
        TYPE_CLASSIC,
        TYPE_NUNCHAKU,
        TYPE_COMPOSITION
    };

    virtual Type GetType() const = 0;
    virtual s32 GetChannel() const = 0;
    virtual void StartMotor(const MotorPattern& rPattern) = 0;
    virtual void StopMotor() = 0;
    virtual const wchar_t* GetButtonSymbol(u32 button) const = 0;
    virtual u32 GetDisconnectSerial() const = 0;
    virtual void Update() = 0;
};

#endif
