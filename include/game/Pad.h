#ifndef GAME_PAD_H
#define GAME_PAD_H

#include <revolution/os.h>
#include <revolution/pad.h>
#include <revolution/kpad.h>
#include <revolution/wpad.h>

struct RumbleStep {
    unsigned short mOnMilliseconds : 12;
    bool mStopHard : 1;
    unsigned char mFlags : 3;
    unsigned short mOffMilliseconds : 12;
    unsigned short mEndFlags : 4;
};

struct WiiPadState : public KPADStatus {
    unsigned int mRepeat;
    unsigned int mHold;
    unsigned int mTrigger;
    unsigned int mRelease;
    unsigned int mButtonRepeat;
    unsigned int mRepeatButtons;
    unsigned short mRepeatDelay;
    unsigned char mProcessingState;
    Vec2 mStablePosition;
    unsigned int mPointerIdleFrames;
    unsigned int mIdleFrames;
    unsigned int mNunchukIdleFrames;
    unsigned int mShake;
    unsigned int mShakeTrigger;
    int mProbeResult;
    unsigned int mDeviceType;
    void UpdateButtons(unsigned int hold);
    void Update(const KPADStatus* pStatus, unsigned int activityMask);
};

struct GCPadState : public PADStatus {
    unsigned int mHold;
    unsigned int mTrigger;
    unsigned int mRelease;
    unsigned int mRepeat;
    unsigned int mRepeatButtons;
    short mStickDeltaX;
    short mStickDeltaY;
    short mSubstickDeltaX;
    short mSubstickDeltaY;
    unsigned short mRepeatDelay;
    Vec2 mStick;
    Vec2 mSubstick;
    float mLeftTrigger;
    float mRightTrigger;
    void UpdateButtons(unsigned int hold);
    void Update(const PADStatus* pStatus);
};

class RumbleBlocker {
public:
    RumbleBlocker();
    ~RumbleBlocker();
};

void InitializePad(OSTime sampleInterval, int channels);
void SetPadReadDelay(int frames);
void UpdatePad();
WiiPadState* GetWiiPad(int channel);
int GetWiiPadDevice(int channel, unsigned int* pDeviceType);
GCPadState* GetGCPad(int channel);
void StartWiiRumble(int channel, const RumbleStep* pPattern);
void StopWiiRumble(int channel);
void StartGCRumble(int channel, const RumbleStep* pPattern);
void StopGCRumble(int channel);
void SetGCRumbleEnabled(bool enabled);
void SetPadConnectionCallback(int channel, WPADCallback callback);
OSTime GetPadSampleInterval();

#endif
