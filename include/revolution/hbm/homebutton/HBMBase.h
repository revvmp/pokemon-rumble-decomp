#ifndef HBM_HOMEBUTTON_BASE_H
#define HBM_HOMEBUTTON_BASE_H

#include "nw4hbm/lyt/lyt_drawInfo.h"
#include "revolution/hbm/homebutton/HBMGUIManager.h"
#include "revolution/hbm/homebutton/HBMCommon.h"

#include "revolution/axfx.h"
#include "revolution/ax.h"
#include "revolution/gx.h"
#include "revolution/os.h"
#include "revolution/os/OSMutex.h"
#include "revolution/hbm/HBMBase.h"
#include "revolution/wpad.h"

namespace nw4hbm {
namespace lyt {
class ArcResourceAccessor;
class ArcResourceLink;
class Layout;
class MultiArcResourceAccessor;
class Pane;
}
namespace ut {
class ResFont;
}
}

namespace homebutton {

class HomeButton;
class Controller;
class GroupAnmController;
class RemoteSpk;

class HomeButtonEventHandler : public gui::EventHandler {
public:
    HomeButtonEventHandler(HomeButton* pHomeButton) : mpHomeButton(pHomeButton) {}

    virtual void onEvent(u32 uID, u32 uEvent, void* pData);

    HomeButton* getHomeButton() { return mpHomeButton; }

private:
    HomeButton* mpHomeButton;
};

class HomeButton {
private:
    enum eSeq {
        eSeq_Normal,
        eSeq_Control,
        eSeq_Cmn,
        eSeq_Seq3,
        eSeq_Seq4
    };

    class BlackFader {
    public:
        BlackFader(int maxFrame) {
            init(maxFrame);
            setColor(0, 0, 0);
            fadeColorEnable_ = true;
        }

        int getFrame() const { return frame_; }
        int getMaxFrame() const { return maxFrame_; }
        int getFadeColorEnable() const { return fadeColorEnable_; }

        void setFadeColorEnable(bool enable) { fadeColorEnable_ = enable; }
        void setColor(u8 r, u8 g, u8 b) {
            red_ = r;
            green_ = g;
            blue_ = b;
        }

        bool isDone();
        void init(int maxFrame);
        void calc();
        void draw();

        void start() { state_ = 1; }

    private:
        int frame_;
        int maxFrame_;
        int state_;
        bool fadeColorEnable_;
        u8 red_;
        u8 green_;
        u8 blue_;
    };

public:
    HomeButton(const HBMDataInfo* pHBInfo);
    ~HomeButton();

    const HBMDataInfo* getHBMDataInfo() { return mpHBInfo; }
    Controller* getController(int chan) { return mpController[chan]; }
    int getVolume();
    HBMSelectBtnNum getSelectBtnNum();
    const char* getFuncPaneName(int no) { return scFuncTouchPaneName[no]; }
    const char* getPaneName(int no) { return scBtnName[no]; }
    const char* getBtnWarePaneName(int no) { return scBtnWareName[no]; }
    bool getReassignedFlag() const { return mReassignedFlag; }
    HomeButtonEventHandler* getEventHandler() const { return mpHomeButtonEventHandler; }
    bool isActive() const;
    bool isUpBarActive() const;
    bool isDownBarActive();
    bool isThirdBarActive() const;
    int getPaneNo(const nw4hbm::lyt::Pane* pPane);

    void setAdjustFlag(int flag);
    void setEndSimpleSyncFlag(bool flag) { mEndSimpleSyncFlag = flag; }
    void setForcusSE();
    void setReassignedFlag(bool flag) { mReassignedFlag = flag; }
    void setSimpleSyncAlarm(int type);
    void setSimpleSyncFlag(bool flag) { mSimpleSyncFlag = flag; }
    void setSpeakerAlarm(int chan, int msec);
    void setVolume(int vol);

    bool getVibFlag();
    void setVibFlag(bool flag);

    void create();
    void init();
    void calc(const HBMControllerData* pController);
    void draw();
    void update(const HBMControllerData* pController);
    void updateTrigPane();

    void startPointEvent(const nw4hbm::lyt::Pane* pPane, void* pData);
    void startLeftEvent(const nw4hbm::lyt::Pane* pPane);
    void startTrigEvent(const nw4hbm::lyt::Pane* pPane);

    int findAnimator(int pane, int anm);
    int findGroupAnimator(int pane, int anm);

    void callSimpleSyncCallback(s32 result, s32 num);

    void startBlackOut();
    void setBlackOutColor(u8 r, u8 g, u8 b) { mFader.setColor(r, g, b); }

    void createInfoEx(const HBMDataInfoEx* pHBInfoEx);
    void destroyInfoEx();

    static void createInstance(const HBMDataInfo* pHBInfo);
    static HomeButton* getInstance();
    static void deleteInstance();

    void update_sound();

private:
    void init_battery(const HBMControllerData* pController);
    void calc_battery(int chan);
    void reset_battery();

    void init_sound();
    void play_sound(int id);
    void fadeout_sound(f32 gain);

    void init_msg();
    void init_vib();
    void init_volume();

    void set_config();
    void set_text();
    void set_text_ex();

    void calc_fadeoutAnm();

    void update_controller(int id);
    void update_posController(int id);

    void reset_btn();
    void reset_control();
    void reset_guiManager(int num);
    void reset_window();

    static HomeButton* spHomeButtonObj;
    static OSMutex sMutex;

    static const int scReConnectTime;
    static const int scReConnectTime2;
    static const int scPadDrawWaitTime;
    static const int scGetPadInfoTime;
    static const int scForcusSEWaitTime;
    static const f32 scOnPaneVibTime;
    static const f32 scOnPaneVibWaitTime;
    static const int scWaitStopMotorTime;
    static const int scWaitDisConnectTime;

    static const char* scCursorLytName[WPAD_MAX_CONTROLLERS];
    static const char* scCursorPaneName;
    static const char* scCursorRotPaneName;
    static const char* scCursorSRotPaneName;

    static const char* scBtnName[4];
    static const char* scTxtName[4];
    static const char* scGrName[8];
    static const char* scAnimName[3];
    static const char* scPairGroupAnimName[15];
    static const char* scPairGroupName[15];
    static const char* scGroupAnimName[22];
    static const char* scGroupName[35];
    static const char* scFuncPaneName[5];
    static const char* scFuncTouchPaneName[10];
    static const char* scFuncTextPaneName[3];
    static const char* scBatteryPaneName[WPAD_MAX_CONTROLLERS][4];
    static const char* scBtnWareName[2];
    static const char* scButtonBarMenuName[7];
    static const char* scButtonBarMenuGroupName[7];

    static const int scSoundHeapSize_but2;
    static const int scSoundHeapSize_but3;
    static const int scSoundThreadPrio;
    static const int scDvdThreadPrio;

    eSeq mSequence;
    const HBMDataInfo* mpHBInfo;
    const HBMDataInfoEx* mpHBInfoEx;
    int mButtonNum;
    int mAnmNum;
    int mState;
    int mSelectAnmNum;
    int mMsgCount;
    int mPaneCounter[14];
    int mPaneCounterEx[2];
    int mPadDrawTime[WPAD_MAX_CONTROLLERS];
    int mForcusSEWaitTime;
    int mBar0AnmRev;
    int mBar1AnmRev;
    int mBar0AnmRevHold;
    int mBar1AnmRevHold;
    int mGetPadInfoTime;
    bool mControllerFlag[WPAD_MAX_CONTROLLERS];
    int mVolumeNum;
    int mConnectNum;
    bool mVibFlag;
    bool mControlFlag;
    bool mLetterFlag;
    bool mAdjustFlag;
    bool mReassignedFlag;
    bool mSimpleSyncFlag;
    bool mEndSimpleSyncFlag;
    bool mForthConnectFlag;
    bool mInitFlag;
    bool mForceSttInitProcFlag;
    bool mForceSttFadeInProcFlag;
    bool mEndInitSoundFlag;
    bool mForceStopSyncFlag;
    bool mForceEndMsgAnmFlag;
    bool mStartBlackOutFlag;
    int mSoundRetryCnt;
    int mDialogFlag[4];
    char* mpLayoutName;
    char* mpAnmName;
    HBMSelectBtnNum mSelectBtnNum;
    wchar_t* mpText[10][6];
    WPADInfo mWpadInfo[WPAD_MAX_CONTROLLERS];
    WPADSyncDeviceCallback mSimpleSyncCallback;
    f32 mOnPaneVibFrame[WPAD_MAX_CONTROLLERS];
    f32 mOnPaneVibWaitFrame[WPAD_MAX_CONTROLLERS];
    int mWaitStopMotorCount;
    int mDisConnectCount;
    nw4hbm::lyt::Layout* mpLayout;
    nw4hbm::lyt::Layout* mpLayoutEx;
    nw4hbm::lyt::Layout* mpCursorLayout[WPAD_MAX_CONTROLLERS];
    nw4hbm::lyt::ArcResourceAccessor* mpResAccessor;
    nw4hbm::lyt::MultiArcResourceAccessor* mpMultiResAccessor;
    nw4hbm::lyt::ArcResourceLink* mpHBInfoExLink;
    nw4hbm::lyt::ArcResourceLink* mpHBInfoLink;
    gui::PaneManager* mpPaneManager;
    nw4hbm::ut::ResFont* mpResFont;
    HomeButtonEventHandler* mpHomeButtonEventHandler;
    nw4hbm::lyt::DrawInfo mDrawInfo;
    Controller* mpController[WPAD_MAX_CONTROLLERS];
    RemoteSpk* mpRemoteSpk;
    wchar_t* mpTextEx[10];
    nw4hbm::ut::ResFont* mpResFontEx;
    GXTexObj mTexObj;
    int mBar2AnmRev;
    int mBar2AnmRevHold;
    GroupAnmController* mpAnmController[12];
    GroupAnmController* mpGroupAnmController[74];
    GroupAnmController* mpPairGroupAnmController[15];
    GroupAnmController* mpAnmControllerEx[7];
    BlackFader mFader;
    OSAlarm mAlarm[WPAD_MAX_CONTROLLERS];
    OSAlarm mSpeakerAlarm[WPAD_MAX_CONTROLLERS];
    OSAlarm mSimpleSyncAlarm;
    int iVISetBlackFlag;
    int iReConnectTime;
    int iReConnectTime2;
    u16 mAppVolume[3];
    u16 mBatteryCheck;
    AXFXAllocHook mAxFxAlloc;
    AXFXFreeHook mAxFxFree;
    AXFX_REVERBHI mAxFxReverb;
    AXAuxCallback mAuxCallback;
    void* mpAuxContext;
    f32 mFadeOutSeTime;
};

}

#endif
