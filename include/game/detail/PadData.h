#ifndef GAME_DETAIL_PADDATA_H
#define GAME_DETAIL_PADDATA_H

#include <game/Pad.h>
#include <revolution/os.h>
#include <revolution/pad.h>
#include <revolution/kpad.h>
#include <revolution/wpad.h>
#include <revolution/vi.h>
#include <string.h>
#include <new>
#include <memory>

namespace {

class NonCopyable {
protected:
    NonCopyable() {}
    ~NonCopyable() {}
private:
    NonCopyable(const NonCopyable&);
    NonCopyable& operator=(const NonCopyable&);
};

class InterruptLock : private NonCopyable {
    BOOL mPrevious;
public:
    InterruptLock() : mPrevious(OSDisableInterrupts()) {}
    ~InterruptLock() { OSRestoreInterrupts(mPrevious); }
};

class BRumble {
public:
    bool mEnabled;
    int mBlockCount;
    int mChannel;
    int mState;
    const RumbleStep* mpPattern;
    float mCooldown;
    OSAlarm mAlarm;
    OSTime mStart;
    BRumble() : mEnabled(true), mBlockCount(0), mChannel(0), mState(0), mpPattern(nullptr), mCooldown(0.2f) {
        memset(&mAlarm, 0, sizeof(mAlarm));
        OSCreateAlarm(&mAlarm);
        OSSetAlarmUserData(&mAlarm, this);
    }
    virtual ~BRumble() {}
    virtual bool CanRumble() const;
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void StopHard() = 0;
    void StopPattern();
    void StartPattern(const RumbleStep* pPattern);
    void UpdatePattern();
    void Schedule(float seconds);
    void Shutdown();
    void Block();
    void Unblock();
    void SetEnabled(bool enabled);
    static void AlarmCallback(OSAlarm* pAlarm, OSContext* pContext);

};

class CWiiRemoteRumble : public BRumble {
public:
    CWiiRemoteRumble() {}
    virtual ~CWiiRemoteRumble() {}
    virtual bool CanRumble() const { return BRumble::CanRumble() && WPADIsMotorEnabled(); }
    virtual void Start() { WPADControlMotor(mChannel, 1); }
    virtual void Stop() { WPADControlMotor(mChannel, 0); }
    virtual void StopHard() { WPADControlMotor(mChannel, 0); }
};

class CGCPadRumble : public BRumble {
public:
    CGCPadRumble() {}
    virtual ~CGCPadRumble() {}
    virtual void Start() { PADControlMotor(mChannel, PAD_MOTOR_RUMBLE); }
    virtual void Stop() { PADControlMotor(mChannel, PAD_MOTOR_STOP); }
    virtual void StopHard() { PADControlMotor(mChannel, PAD_MOTOR_STOP_HARD); }
};

struct PadData {
    OSAlarm mAlarm;
    int mWriteIndex;
    int mReadIndex;
    unsigned int mDirectionHistory[60][4];
    int mProbeHistory[60][4];
    u32 mDeviceHistory[60][4];
    KPADStatus mWiiHistory[60][4];
    WiiPadState mWiiWorking[4];
    WiiPadState mWiiPublic[4];
    CWiiRemoteRumble mWiiRumble[4];
    unsigned int mActivityMask[4];
    WPADCallback mConnectCallbacks[4];
    int mChannelCount;
    PADStatus mGCRawHistory[60][4];
    GCPadState mGCWorking[4];
    GCPadState mGCPublic[4];
    CGCPadRumble mGCRumble[4];
    unsigned int mMinPointerIdleFrames;
    unsigned int mMinButtonIdleFrames;
    unsigned int mMinNunchukIdleFrames;
    unsigned int mMinInputIdleFrames;
    int mClampMode;
    int mClampRegion;
    int mShutdownCallbackId;
    PadData();
};

extern std::tr1::shared_ptr<PadData> spPadData;

}

#endif
