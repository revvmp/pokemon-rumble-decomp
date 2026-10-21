#ifndef GAME_BCONTROLLER_H
#define GAME_BCONTROLLER_H

#include <game/IController.h>
#include <nw4r/math/math_types.h>

class BController : public IController {
public:
    enum Cross {
        CROSS_UP = 1 << 0,
        CROSS_RIGHT = 1 << 1,
        CROSS_DOWN = 1 << 2,
        CROSS_LEFT = 1 << 3,
        CROSS_MASK = CROSS_UP | CROSS_RIGHT | CROSS_DOWN | CROSS_LEFT
    };

    BController();

    void SetChannel(s32 channel) { mChannel = channel; }

    virtual s32 GetChannel() const { return mChannel; }
    virtual bool IsConnected() const { return mConnected; }
    virtual f32 GetStickLength() const { return mStickLength; }
    virtual f32 GetStickAngle() const { return mStickAngle; }
    virtual u32 GetHold() const { return mHold; }
    virtual u32 GetTrig() const { return mTrig; }
    virtual u32 GetRelease() const { return mRelease; }
    virtual u32 GetRepeat() const { return mRepeat; }
    virtual u32 GetDisconnectSerial() const { return mDisconnectSerial; }
    virtual void StartMotor(const MotorPattern& rPattern) {}
    virtual void StopMotor() {}
    virtual const wchar_t* GetButtonSymbol(u32 button) const { return L""; }

    bool IsLost(bool detected);
    void SetButton(u32 button);
    void SetDisconnected();
    void SetStickFromCross();
    void SetStick(const nw4r::math::VEC2& rStick);
    void UpdateButton(u32 button);

protected:
    s32 mChannel;
    bool mConnected;
    s32 mLostCount;
    f32 mStickLength;
    f32 mStickAngle;
    u32 mHold;
    u32 mTrig;
    u32 mRelease;
    u32 mRepeat;
    u32 mRepeatButton;
    s32 mRepeatWait;
    u32 mDisconnectSerial;
};

#endif
