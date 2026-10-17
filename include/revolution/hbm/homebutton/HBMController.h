#ifndef HBM_CONTROLLER_H
#define HBM_CONTROLLER_H

#include "revolution/mem.h"
#include "revolution/os.h"
#include "revolution/hbm/HBMBase.h"
#include "revolution/wpad.h"

struct HBController {
    int chan;
    f32 spVol;
    f32 x;
    f32 y;
    u32 trig;
    u32 hold;
    u32 release;
    bool rumble;
};

namespace homebutton {

class RemoteSpk;

class Controller {
public:
    Controller(int chan, RemoteSpk* pSpk);
    ~Controller();

    HBController* getController();
    int getChan() const;
    f32 getSpeakerVol() const;
    void setSpeakerVol(f32 vol);

    RemoteSpk* getRemoteSpk() const { return remotespk; }
    bool isRumbling() { return mHBController.rumble; }
    void setRumble() { mHBController.rumble = true; }
    void clrRumble() { mHBController.rumble = false; }
    void setEnableRumble(bool flag) { mRumbleFlag = flag; }

    bool getBatteryFlag() const;
    s32 getInfoAsync(WPADInfo* pInfo);
    bool isPlayReady() const;
    bool isPlayingSound() const;
    bool isPlayingSoundId(int id) const;

    void setKpad(const HBMKPadData* pPadData, bool updatePos);
    void setInValidPos();
    void clrBatteryFlag();
    void clrKpadButton();

    void connect();
    void disconnect();

    void initSound();
    void updateSound();
    void playSound(int id);

    void soundOn();
    void soundOff(int msec);

    void startMotor();
    void stopMotor();

    void initCallback();
    void clearCallback();

private:
    static void wpadConnectCallback(s32 chan, s32 result);
    static void wpadExtensionCallback(s32 chan, s32 result);
    static void soundOnCallback(OSAlarm* pAlarm, OSContext* pContext);
    static void ControllerCallback(s32 chan, s32 result);

    static bool sBatteryFlag[WPAD_MAX_CONTROLLERS];
    static OSAlarm sAlarm[WPAD_MAX_CONTROLLERS];
    static OSAlarm sAlarmSoundOff[WPAD_MAX_CONTROLLERS];
    static Controller* sThis[WPAD_MAX_CONTROLLERS];

    HBController mHBController;
    RemoteSpk* remotespk;
    WPADConnectCallback mOldConnectCallback;
    WPADExtensionCallback mOldExtensionCallback;
    OSTime mPlaySoundTime;
    OSTime mStopSoundTime;
    bool mCallbackFlag;
    bool mSoundOffFlag;
    bool mCheckSoundTimeFlag;
    bool mCheckSoundIntervalFlag;
    bool mRumbleFlag;
};

}

#endif
