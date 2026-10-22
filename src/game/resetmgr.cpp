#include <game/NResetMgr.h>
#include <game/CScopedActivity.h>
#include <game/ResetCallback.h>

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <revolution/os.h>
#include <revolution/os/OSReset.h>
#include <revolution/os/OSResetSW.h>
#include <revolution/pad.h>
#include <revolution/vi.h>

extern "C" void OSLaunchManualViewer(u32);
extern "C" void OSReturnToDataManager();
extern "C" void OSReturnToSetting(u32);

namespace NResetMgr {
namespace {

enum State {
    STATE_IDLE,
    STATE_LAUNCH_MANUAL_VIEWER,
    STATE_RESTART,
    STATE_RETURN_TO_DATA_MANAGER,
    STATE_RETURN_TO_MENU,
    STATE_RETURN_TO_SETTING,
    STATE_SHUTDOWN
};

struct IDCallback {
    IDCallback(int id, int ignoreMask, const std::tr1::function<void()>& rFunction) : mId(id), mIgnoreMask(ignoreMask) {
        mFunction = rFunction;
    }

    void Call(int ignoreBit) const {
        if (!(mIgnoreMask & ignoreBit)) {
            mFunction();
        }
    }

    int mId;
    int mIgnoreMask;
    std::tr1::function<void()> mFunction;
};

typedef std::list<IDCallback> CallbackList;

int sState;
union {
    u32 mValue;
    u8 mSettingType;
} sArgument;
int sDisableCount;
std::tr1::shared_ptr<CallbackList> sCallbacks;

void OnReset() {
    if (sState == STATE_IDLE) {
        sState = STATE_RESTART;
        sArgument.mValue = 0;
    }
}

void OnPowerOff() {
    if (sState == STATE_IDLE) {
        sState = STATE_SHUTDOWN;
    }
}

inline void NotifyAndBlankScreen(int ignoreBit) {
    std::tr1::shared_ptr<CallbackList> callbacks = sCallbacks;
    sCallbacks.reset();
    for (CallbackList::iterator it = callbacks->end(); it != callbacks->begin(); --it) {
        CallbackList::const_iterator current = it;
        --current;
        (*current).Call(ignoreBit);
    }
    VISetBlack(TRUE);
    VIFlush();
    VIWaitForRetrace();
}

}

void Initialize() {
    sState = STATE_IDLE;
    sDisableCount = 0;
    OSSetResetCallback(OnReset);
    OSSetPowerCallback(OnPowerOff);
}

void Update() {
    if (sDisableCount != 0) {
        return;
    }
    switch (sState) {
    case STATE_LAUNCH_MANUAL_VIEWER:
        sState = STATE_IDLE;
        NotifyAndBlankScreen(1 << 0);
        OSLaunchManualViewer(sArgument.mValue);
        break;
    case STATE_RESTART:
        sState = STATE_IDLE;
        NotifyAndBlankScreen(1 << 1);
        PADRecalibrate(0xF0000000);
        OSRestart(sArgument.mValue);
        break;
    case STATE_RETURN_TO_DATA_MANAGER:
        NotifyAndBlankScreen(1 << 2);
        OSReturnToDataManager();
        break;
    case STATE_RETURN_TO_MENU:
        NotifyAndBlankScreen(1 << 3);
        OSReturnToMenu();
        break;
    case STATE_RETURN_TO_SETTING:
        NotifyAndBlankScreen(1 << 4);
        OSReturnToSetting(sArgument.mSettingType);
        break;
    case STATE_SHUTDOWN:
        sState = STATE_IDLE;
        NotifyAndBlankScreen(1 << 5);
        OSShutdownSystem();
        break;
    }
}

bool IsEnabled() {
    return sDisableCount == 0;
}

void ClearDisableCount() {
    sDisableCount = 0;
}

CDisable::CDisable() {
    ++sDisableCount;
}

CDisable::~CDisable() {
    --sDisableCount;
    sDisableCount = std::max(0, sDisableCount);
}

bool RequestLaunchManualViewer(u32 argument) {
    if (sState != STATE_IDLE) {
        return false;
    }
    sArgument.mValue = argument;
    sState = STATE_LAUNCH_MANUAL_VIEWER;
    return true;
}

bool RequestRestart(u32 argument) {
    if (sState != STATE_IDLE) {
        return false;
    }
    sArgument.mValue = argument;
    sState = STATE_RESTART;
    return true;
}

bool RequestReturnToDataManager() {
    if (sState != STATE_IDLE) {
        return false;
    }
    sState = STATE_RETURN_TO_DATA_MANAGER;
    return true;
}

bool RequestReturnToMenu() {
    if (sState != STATE_IDLE) {
        return false;
    }
    sState = STATE_RETURN_TO_MENU;
    return true;
}

bool IsRestartRequested() {
    return sState == STATE_RESTART;
}

bool IsShutdownRequested() {
    return sState == STATE_SHUTDOWN;
}

bool CancelRestart() {
    if (sState == STATE_RESTART) {
        sState = STATE_IDLE;
        return true;
    }
    return false;
}

}

int RegisterResetCallback(const std::tr1::function<void()>& rCallback, int ignoreMask) {
    using namespace NResetMgr;
    int id;
    if (!sCallbacks) {
        sCallbacks.reset(new CallbackList);
        id = 1;
    } else {
        id = sCallbacks->back().mId + 1;
    }
    sCallbacks->push_front(IDCallback(id, ignoreMask, rCallback));
    return id;
}
