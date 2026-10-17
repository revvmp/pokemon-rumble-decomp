#include "revolution/os.h"
#include "revolution/os/OSCache.h"
#include "revolution/os/OSContext.h"
#include "revolution/os/OSReset.h"
#include "revolution/os/OSThread.h"
#include "revolution/sc.h"
#include "revolution/vi.h"
#include "revolution/wpad.h"
#include "revolution/wud/wud.h"
#include <string.h>

#define ACR_REG(offset) (*(volatile u32*)(0xCD000000 + (offset)))
#define ACR_REG_SENSOR_BAR 0xC0
#define ACR_SENSOR_BAR_POWER 0x100

#define AFH_CHANNEL_ADDR 0x31A2

#define WPAD_MANAGE_PERIOD_SENSE 10
#define WPAD_MANAGE_PERIOD_CHECK 5
#define WPAD_MANAGE_PERIOD_AFH 60000
#define WPAD_CMD_RETRY_COUNT 5

#define WPAD_MIN_DPD_SENS 1
#define WPAD_MAX_DPD_SENS 5

#define WPAD_NOISE_FILTER_ACC 0
#define WPAD_NOISE_FILTER_DPD 1
#define WPAD_NOISE_FILTER_EXT 2

#define WPAD_HIDREP_FLAG_RUMBLE 0x01
#define WPAD_HIDREP_FLAG_ACK 0x02

const char* __WPADVersion = "<< RVL_SDK - WPAD \trelease build: Jun 22 2009 18:33:21 (0x4302_145) >>";

static OSAlarm _wpadManageAlarm;
WPADControlBlock* _wpdcb[WPAD_MAX_CONTROLLERS];
static u8 __wpadManageHandlerStack[0x1000] __attribute__((aligned(32)));
static s8 _wpadHandle2PortTable[WUD_MAX_DEV_ENTRY];
static WPADControlBlock _wpd[WPAD_MAX_CONTROLLERS];

static s32 _wpadOnReconnect = -1;
static u16 _wpad_diff_count_threshold[WPAD_MAX_NOISE_FILTERS] = {6, 4, 6};
static u16 _wpad_hyst_count_threshold[WPAD_MAX_NOISE_FILTERS] = {30, 30, 30};

static u8 _wpadGameType;
static const char* _wpadGameCode;
static u8 _wpadSleepTime;
static u8 _wpadDpdSense;
static u8 _wpadSensorBarPos;
static BOOL _wpadRumbleFlag;
static u8 _wpadSpeakerVol;
static u8 _wpadSCSetting;
static u8 _wpadShutdownFlag;
static s8 _wpadAfhChannel;
static u8 _wpadIsUsedChannel[WPAD_MAX_CONTROLLERS];
static s8 _wpadInfoResult[WPAD_MAX_CONTROLLERS];
static BOOL _wpadInitialized;
static BOOL _wpadUsedCallback;
__declspec(weak) BOOL _enabledDRM;
__declspec(weak) BOOL _enabledGTR;
__declspec(weak) BOOL _enabledTRN;
__declspec(weak) BOOL _enabledVSM;
__declspec(weak) WPADInitFunc _wpadDRMInit;
__declspec(weak) WPADInitFunc _wpadGTRInit;
__declspec(weak) WPADInitFunc _wpadTRNInit;
__declspec(weak) WPADInitFunc _wpadVSMInit;
static s32 _wpadReconnectWait;
static BOOL _wpadStartup;
static u8 _wpadRumbleCnt[WPAD_MAX_CONTROLLERS];
static u8 _wpadExtCnt[WPAD_MAX_CONTROLLERS];
static u16 _wpadAfhCnt;
static u8 _wpadCheckCnt;
static u16 _wpadSenseCnt;
static u8 _wpadRegisterShutdownFunc;

static BOOL OnShutdown(BOOL final, u32 event);
static void __wpadManageHandler(OSAlarm* alarm, OSContext* context);
void __wpadConnectionCallback(WUDDevInfo* info, u8 open);
void __wpadReceiveCallback(u8 devHandle, u8* report, u16 len);
static void __wpadDisconnectCallback(s32 chan, s32 result);
static s32 __wpadGetConnectionStatus(s32 chan);
static void __wpadDisconnect(s32 chan, BOOL sleep);
static s8 __wpadGetQueueSize(WPADCmdQueue* queue);
static BOOL __wpadGetCommand(WPADCmdQueue* queue, WPADCommand* cmd);
static BOOL __wpadPopCommand(WPADCmdQueue* queue);
static BOOL __wpadSetSensorBarPower(BOOL enable);
static void __wpadSetScreenSaverFlag(BOOL flag);
static u8 __wpadGetDpdSensitivity(void);
static u8 __wpadGetSensorBarPosition(void);
static u32 __wpadGetMotorMode(void);
static u8 __wpadClampSpeakerVolume(u8 volume);
static u8 __wpadGetSpeakerVolume(void);

static void* __wpadNoAlloc(u32 size) {
#pragma unused(size)
    return NULL;
}

static u8 __wpadNoFree(void* ptr) {
#pragma unused(ptr)
    return 0;
}

BOOL WPADIsEnabledVSM(void) {
    return _enabledVSM;
}

BOOL WPADIsEnabledTRN(void) {
    return _enabledTRN;
}

BOOL WPADIsEnabledGTR(void) {
    return _enabledGTR;
}

BOOL WPADIsEnabledDRM(void) {
    return _enabledDRM;
}

__declspec(weak) s32 WBCSetupCalibration(void) {
    return 0;
}

__declspec(weak) s32 WBCGetCalibrationStatus(void) {
    return 0;
}

__declspec(weak) s32 WBCGetBatteryLevel(u8 battery) {
#pragma unused(battery)
    return 0;
}

__declspec(weak) s32 WBCRead(WPADBLStatus* status, f64* weight, s32 size) {
#pragma unused(status, weight, size)
    DEBUGPrint("WBCReadDummy\n");
    return WPAD_ERR_NO_CONTROLLER;
}

__declspec(weak) s32 WBCSetZEROPoint(f64* zero, s32 size) {
#pragma unused(zero, size)
    DEBUGPrint("WBCSetZEROPointDummy\n");
    return WPAD_ERR_NO_CONTROLLER;
}

__declspec(weak) s32 WBCGetTGCWeight(f64 weight, f64* result, WPADBLStatus* status) {
#pragma unused(weight, result, status)
    DEBUGPrint("WBCGetTGCWeightDummy\n");
    return WPAD_ERR_NO_CONTROLLER;
}

static void __wpadShutdown(BOOL saveSimpleDevs) {
    OSCancelAlarm(&_wpadManageAlarm);
    WUDSetHidRecvCallback(NULL);
    WUDShutdown(saveSimpleDevs);
}

static void __wpadCleanup(void) {
    BOOL enabled = OSDisableInterrupts();
    s32 chan;

    if (_wpadShutdownFlag) {
        OSRestoreInterrupts(enabled);
        return;
    }

    _wpadShutdownFlag = TRUE;
    WUDSetVisibility(FALSE, FALSE);

    for (chan = 0; chan < WPAD_MAX_CONTROLLERS; chan++) {
        WUDSetDeviceHistory(chan, NULL);
    }

    __wpadShutdown(FALSE);
    OSRestoreInterrupts(enabled);
}

static void __wpadReconnectStart(BOOL saveSimpleDevs) {
    BOOL enabled = OSDisableInterrupts();

    if (!_wpadStartup) {
        OSRestoreInterrupts(enabled);
        return;
    }

    if (_wpadShutdownFlag) {
        OSRestoreInterrupts(enabled);
        return;
    }

    _wpadShutdownFlag = TRUE;
    _wpadOnReconnect = saveSimpleDevs ? TRUE : FALSE;
    DEBUGPrint("Wait for %d ms until start reconnect!\n", _wpadReconnectWait);
    OSRestoreInterrupts(enabled);
}

static BOOL OnShutdown(BOOL final, u32 event) {
    BOOL cleanup;
    s32 status = WPADGetStatus();

    if (final != FALSE || status == WPAD_STATE_DISABLED) {
        return TRUE;
    }

    if (status == WPAD_STATE_DISABLING || status == WPAD_STATE_ENABLING || status == WPAD_STATE_ENABLED) {
        return FALSE;
    }

    if (WUDIsBusy()) {
        WPADCancelSyncDevice();
        return FALSE;
    }

    switch (event) {
    case OS_SHUTDOWN_EVENT_FATAL:
        WPADRegisterAllocator(__wpadNoAlloc, __wpadNoFree);
    case OS_SHUTDOWN_EVENT_SHUTDOWN:
    case OS_SHUTDOWN_EVENT_RESERVED:
        cleanup = TRUE;
        break;
    case OS_SHUTDOWN_EVENT_REBOOT:
    case OS_SHUTDOWN_EVENT_RESTART:
    case OS_SHUTDOWN_EVENT_LAUNCH:
        cleanup = FALSE;
        break;
    case OS_SHUTDOWN_EVENT_RETURN_TO_MENU:
        cleanup = __OSIsReturnToIdle ? TRUE : FALSE;
        break;
    }

    if (cleanup) {
        __wpadCleanup();
    } else {
        __wpadReconnectStart(TRUE);
    }

    return FALSE;
}

static void __wpadSyncCallback(s32 chan, s32 result) {
    _wpadInfoResult[chan] = result;
    OSWakeupThread(&_wpdcb[chan]->threadq);
}

static s32 __wpadSync(s32 chan) {
    OSSleepThread(&_wpdcb[chan]->threadq);
    return _wpadInfoResult[chan];
}

static OSShutdownFunctionInfo ShutdownFunctionInfo = {OnShutdown, 127, NULL, NULL};

static void __wpadSendDataSub(s32 chan, WPADCommand cmd) {
    BOOL enabled;
    BOOL rumble;
    s8 handle;
    WPADControlBlock* p_wpd;
    u8 rep_id;
    BT_HDR* p_buf;
    u8* report;
    s32 status;
    u8* data;
    u16 len;

    p_wpd = _wpdcb[chan];
    rep_id = cmd.command;
    data = cmd.data;
    len = cmd.len;

    enabled = OSDisableInterrupts();

    status = p_wpd->status;
    handle = p_wpd->dev_handle;
    if (handle < 0) {
        OSRestoreInterrupts(enabled);
        return;
    }

    p_wpd->status = WPAD_ERR_BUSY;
    rumble = p_wpd->motor & _wpadRumbleFlag;

    if (rep_id == WPAD_HIDREP_VIBRATOR) {
        p_wpd->status = status;
    } else if (rep_id == WPAD_HIDREP_STRM) {
        p_wpd->status = status;
        p_wpd->audioFrames--;
    } else {
        switch (rep_id) {
        case WPAD_HIDREP_WRDATA:
            break;
        case WPAD_HIDREP_RDDATA:
            p_wpd->readError = 0;
            p_wpd->readBaseAddr = cmd.readAddr;
            p_wpd->readLength = cmd.readLen;
            p_wpd->readBufPtr = cmd.readBuf;
            break;
        case WPAD_HIDREP_GETSTAT:
            p_wpd->status = status;
            p_wpd->infoBuf = cmd.info;
            p_wpd->getStatFlag = TRUE;
            break;
        default:
            data[0] |= WPAD_HIDREP_FLAG_ACK;
            break;
        }

        p_wpd->resultCallback = cmd.callback;
        p_wpd->cmdId = rep_id;
        p_wpd->cmdTimer = __OSGetSystemTime() + OSSecondsToTicks(2);
        p_wpd->cmdTimeoutAction = 0;
    }

    OSRestoreInterrupts(enabled);

    DEBUGPrint("handle = %d, repid = %02x\n", handle, rep_id);

    p_buf = (BT_HDR*)GKI_getbuf((u8)(len + 18));
    p_buf->len = (u8)(len + 1);
    p_buf->offset = 10;

    report = (u8*)(p_buf + 1) + p_buf->offset;
    report[0] = rep_id;
    memcpy(&report[1], data, len);

    if (rumble) {
        report[1] |= WPAD_HIDREP_FLAG_RUMBLE;
    } else {
        report[1] &= ~WPAD_HIDREP_FLAG_RUMBLE;
    }

    BTA_HhSendData(handle, p_buf);
}

static s32 __wpadSendData(s32 chan, WPADCommand cmd) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    s32 status = p_wpd->getStatFlag ? WPAD_ERR_BUSY : __wpadGetConnectionStatus(chan);

    OSRestoreInterrupts(enabled);

    if (status == WPAD_ERR_NONE) {
        __wpadSendDataSub(chan, cmd);
    }

    if (status == WPAD_ERR_BUSY) {
        if ((s32)OSTicksToSeconds(__OSGetSystemTime() - p_wpd->cmdTimer) > 1 && _wpadSleepTime) {
            p_wpd->cmdTimer = __OSGetSystemTime();
            __wpadDisconnect(chan, FALSE);
        }
    }

    return status;
}

static void __wpadCalcRadioQuality(s32 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    u32 half = 2;
    u16 sense;

    if (_wpadSenseCnt != WPAD_MANAGE_PERIOD_SENSE) {
        return;
    }

    sense = p_wpd->radioSense * 9;
    sense += (u16)(p_wpd->packetCnt * 100 / half);
    sense /= 10;
    sense = sense > 100 ? 100 : sense;

    p_wpd->radioSense = sense;
    p_wpd->packetCnt = 0;

    if (p_wpd->audioStop) {
        if (sense > 85) {
            p_wpd->audioStop = FALSE;
            p_wpd->audioStopCnt = 0;
        } else if (sense > 80) {
            p_wpd->audioStopCnt++;
            if (p_wpd->audioStopCnt >= 20) {
                p_wpd->audioStop = FALSE;
                p_wpd->audioStopCnt = 0;
            }
        }
    } else {
        if (sense < 75) {
            p_wpd->audioStop = TRUE;
            p_wpd->audioStopCnt = 0;
        } else if (sense < 80) {
            p_wpd->audioStopCnt++;
            if (p_wpd->audioStopCnt >= 1) {
                p_wpd->audioStop = TRUE;
                p_wpd->audioStopCnt = 0;
            }
        }
    }
}

static u8 __wpadIsButtonChanged(u16 curr, u16 prev) {
    return curr != prev ? TRUE : FALSE;
}

static u8 __wpadIsAnalogChanged(s32 curr, s32 prev, s32 threshold) {
    s32 diff = curr - prev < 0 ? prev - curr : curr - prev;

    return diff > threshold ? TRUE : FALSE;
}

static BOOL __wpadCalcAnalogNoise(WPADControlBlock* p_wpd, u8 filter, BOOL changed) {
    if (changed) {
        p_wpd->diffCount[filter]++;
        if (p_wpd->diffCount[filter] > _wpad_diff_count_threshold[filter]) {
            p_wpd->diffCount[filter] = 0;
            p_wpd->hystCount[filter] = 0;
            return TRUE;
        }
    } else {
        p_wpd->hystCount[filter] = (p_wpd->hystCount[filter] + 1) % _wpad_hyst_count_threshold[filter];
        if (p_wpd->hystCount[filter] == _wpad_hyst_count_threshold[filter] - 1 && p_wpd->diffCount[filter]) {
            p_wpd->diffCount[filter]--;
        }
    }

    return FALSE;
}

static BOOL __wpadIsControllerDataChanged(WPADControlBlock* p_wpd, void* p_curr, void* p_prev) {
    WPADStatus* curr = p_curr;
    WPADStatus* prev = p_prev;
    u8 changed = FALSE;

    if (p_wpd->dataFormat != WPAD_FMT_VSM_EXTENDED &&
        (curr->err == WPAD_ERR_NONE || curr->err == WPAD_ERR_CORRUPTED) &&
        (prev->err == WPAD_ERR_NONE || prev->err == WPAD_ERR_CORRUPTED))
    {
        changed |= __wpadIsButtonChanged(curr->button, prev->button);

        changed |= __wpadCalcAnalogNoise(p_wpd, WPAD_NOISE_FILTER_ACC,
                                         __wpadIsAnalogChanged(curr->accX, prev->accX, 12) |
                                             __wpadIsAnalogChanged(curr->accY, prev->accY, 12) |
                                             __wpadIsAnalogChanged(curr->accZ, prev->accZ, 12));

        changed |= __wpadCalcAnalogNoise(p_wpd, WPAD_NOISE_FILTER_DPD,
                                         __wpadIsAnalogChanged(curr->obj[0].x, prev->obj[0].x, 2) |
                                             __wpadIsAnalogChanged(curr->obj[0].y, prev->obj[0].y, 2) |
                                             __wpadIsAnalogChanged(curr->obj[1].x, prev->obj[1].x, 2) |
                                             __wpadIsAnalogChanged(curr->obj[1].y, prev->obj[1].y, 2) |
                                             __wpadIsAnalogChanged(curr->obj[2].x, prev->obj[2].x, 2) |
                                             __wpadIsAnalogChanged(curr->obj[2].y, prev->obj[2].y, 2) |
                                             __wpadIsAnalogChanged(curr->obj[3].x, prev->obj[3].x, 2) |
                                             __wpadIsAnalogChanged(curr->obj[3].y, prev->obj[3].y, 2));
    }

    if (curr->err == WPAD_ERR_NONE && prev->err == WPAD_ERR_NONE) {
        switch (p_wpd->dataFormat) {
        case WPAD_FMT_FREESTYLE:
        case WPAD_FMT_FREESTYLE_ACC:
        case WPAD_FMT_FREESTYLE_ACC_DPD: {
            WPADFSStatus* currFS = p_curr;
            WPADFSStatus* prevFS = p_prev;

            changed |= __wpadCalcAnalogNoise(p_wpd, WPAD_NOISE_FILTER_EXT,
                                             __wpadIsAnalogChanged(currFS->fsAccX, prevFS->fsAccX, 12) |
                                                 __wpadIsAnalogChanged(currFS->fsAccY, prevFS->fsAccY, 12) |
                                                 __wpadIsAnalogChanged(currFS->fsAccZ, prevFS->fsAccZ, 12));
            changed |= __wpadIsAnalogChanged(currFS->fsStickX, prevFS->fsStickX, 1);
            changed |= __wpadIsAnalogChanged(currFS->fsStickY, prevFS->fsStickY, 1);
            break;
        }
        case WPAD_FMT_CLASSIC:
        case WPAD_FMT_CLASSIC_ACC:
        case WPAD_FMT_CLASSIC_ACC_DPD:
        case WPAD_FMT_GUITAR:
        case WPAD_FMT_DRUM: {
            WPADCLStatus* currCL = p_curr;
            WPADCLStatus* prevCL = p_prev;
            s32 lStickDiv;
            s32 rStickDiv;
            s32 triggerDiv;

            switch (p_wpd->devMode) {
            case WPAD_DEV_MODE_CLASSIC_REDUCED:
                lStickDiv = 16;
                rStickDiv = 32;
                triggerDiv = 8;
                break;
            case WPAD_DEV_MODE_CLASSIC_STANDARD:
                lStickDiv = 4;
                rStickDiv = 4;
                triggerDiv = 1;
                break;
            default:
                lStickDiv = 1;
                rStickDiv = 1;
                triggerDiv = 1;
                break;
            }

            changed |= __wpadIsButtonChanged(currCL->clButton, prevCL->clButton);
            changed |= __wpadIsAnalogChanged(currCL->clLStickX / lStickDiv, prevCL->clLStickX / lStickDiv, 1);
            changed |= __wpadIsAnalogChanged(currCL->clLStickY / lStickDiv, prevCL->clLStickY / lStickDiv, 1);
            changed |= __wpadIsAnalogChanged(currCL->clRStickX / rStickDiv, prevCL->clRStickX / rStickDiv, 1);
            changed |= __wpadIsAnalogChanged(currCL->clRStickY / rStickDiv, prevCL->clRStickY / rStickDiv, 1);
            changed |= __wpadIsAnalogChanged(currCL->clTriggerL / triggerDiv, prevCL->clTriggerL / triggerDiv, 1);
            changed |= __wpadIsAnalogChanged(currCL->clTriggerR / triggerDiv, prevCL->clTriggerR / triggerDiv, 1);
            break;
        }
        case WPAD_FMT_TRAIN: {
            WPADTRStatus* currTR = p_curr;
            WPADTRStatus* prevTR = p_prev;

            changed |= __wpadIsButtonChanged(currTR->trButton, prevTR->trButton);
            changed |= __wpadIsAnalogChanged(currTR->brake, prevTR->brake, 1);
            changed |= __wpadIsAnalogChanged(currTR->mascon, prevTR->mascon, 1);
            break;
        }
        case WPAD_FMT_BALANCE_CHECKER: {
            WPADBLStatus* currBL = p_curr;
            WPADBLStatus* prevBL = p_prev;

            changed |= __wpadCalcAnalogNoise(p_wpd, WPAD_NOISE_FILTER_EXT,
                                             __wpadIsAnalogChanged(currBL->press[0], prevBL->press[0], 50) |
                                                 __wpadIsAnalogChanged(currBL->press[1], prevBL->press[1], 50) |
                                                 __wpadIsAnalogChanged(currBL->press[2], prevBL->press[2], 50) |
                                                 __wpadIsAnalogChanged(currBL->press[3], prevBL->press[3], 50));
            break;
        }
        case WPAD_FMT_VSM:
        case WPAD_FMT_VSM_EXTENDED: {
            WPADVSStatus* currVS = p_curr;
            WPADVSStatus* prevVS = p_prev;
            int i;

            changed |= __wpadIsButtonChanged(currVS->state, prevVS->state);
            changed |= __wpadIsAnalogChanged(currVS->level, prevVS->level, 32);
            for (i = 0; i < 5; i++) {
                changed |= __wpadIsAnalogChanged(currVS->samples[i], currVS->prevSamples[i], 32);
            }
            break;
        }
        }
    }

    return changed;
}

static void __wpadCalcRecalibration(s32 chan, void* p_status) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    WPADCLStatus* status = p_status;
    s8 pressed = 0;

    if ((p_wpd->dataFormat == WPAD_FMT_FREESTYLE || p_wpd->dataFormat == WPAD_FMT_FREESTYLE_ACC ||
         p_wpd->dataFormat == WPAD_FMT_FREESTYLE_ACC_DPD) &&
        status->button == (WPAD_BUTTON_PLUS | WPAD_BUTTON_B | WPAD_BUTTON_A | WPAD_BUTTON_MINUS))
    {
        pressed = 1;
    }

    if ((p_wpd->dataFormat == WPAD_FMT_CLASSIC || p_wpd->dataFormat == WPAD_FMT_CLASSIC_ACC ||
         p_wpd->dataFormat == WPAD_FMT_CLASSIC_ACC_DPD) &&
        (status->button == (WPAD_BUTTON_PLUS | WPAD_BUTTON_B | WPAD_BUTTON_A | WPAD_BUTTON_MINUS) ||
         (status->clButton == (WPAD_CL_BUTTON_A | WPAD_CL_BUTTON_B | WPAD_CL_BUTTON_PLUS | WPAD_CL_BUTTON_MINUS) &&
          status->err == WPAD_ERR_NONE)))
    {
        pressed = 1;
    }

    p_wpd->recalibrateCount += pressed;
    if (p_wpd->recalibrateCount > 600) {
        WPADRecalibrate(chan);
    }
}

static void __wpadCalcControllerData(s32 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL saver = FALSE;
    BOOL enabled;
    u8 index;
    WPADStatus* status;
    BOOL changed;

    if (_wpadCheckCnt != WPAD_MANAGE_PERIOD_CHECK) {
        return;
    }

    enabled = OSDisableInterrupts();
    index = p_wpd->rxBufIndex != 0 ? 0 : 1;
    status = (WPADStatus*)p_wpd->rxBuf[index];
    OSRestoreInterrupts(enabled);

    changed = __wpadIsControllerDataChanged(p_wpd, status, p_wpd->prBuf);
    __wpadCalcRecalibration(chan, status);

    if (changed) {
        saver = TRUE;
        p_wpd->lastUpdateTime = __OSGetSystemTime();
        memcpy(p_wpd->prBuf, status, WPAD_RX_DATASIZE);
    } else if (_wpadSleepTime) {
        s32 time = OSTicksToSeconds(__OSGetSystemTime() - p_wpd->lastUpdateTime);
        if (time > _wpadSleepTime * 60) {
            __wpadDisconnect(chan, FALSE);
        }
    }

    if (((WPADStatus*)p_wpd->prBuf)->err != WPAD_ERR_NONE && ((WPADStatus*)p_wpd->prBuf)->err != WPAD_ERR_CORRUPTED) {
        memcpy(p_wpd->prBuf, status, WPAD_RX_DATASIZE);
    }

    __wpadSetScreenSaverFlag(saver);
}

static BOOL __wpadProcExtCommand(s32 chan, BOOL success) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL result = FALSE;
    WPADCommand cmd;

    if (success) {
        return TRUE;
    }

    if (_wpadExtCnt[chan] == WPAD_CMD_RETRY_COUNT && __wpadGetCommand(&p_wpd->extq, &cmd) &&
        (cmd.command == WPAD_HIDREP_REPTYPE || p_wpd->infos.attach) &&
        __wpadSendData(chan, cmd) == WPAD_ERR_NONE)
    {
        __wpadPopCommand(&p_wpd->extq);
        result = TRUE;
        _wpadExtCnt[chan] = 0;
    }

    _wpadExtCnt[chan] = _wpadExtCnt[chan] == WPAD_CMD_RETRY_COUNT ? _wpadExtCnt[chan] : _wpadExtCnt[chan] + 1;
    return result;
}

static BOOL __wpadProcCommand(s32 chan, BOOL success) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    WPADCommand cmd;

    if (success) {
        return TRUE;
    }

    if (__wpadGetCommand(&p_wpd->cmdq, &cmd) && __wpadSendData(chan, cmd) == WPAD_ERR_NONE) {
        __wpadPopCommand(&p_wpd->cmdq);
        return TRUE;
    }

    return FALSE;
}

static void __wpadRumbleMotor(s32 chan, BOOL success) {
    WPADCommand cmd;

    if (success == TRUE || __wpadGetQueueSize(&_wpdcb[chan]->cmdq) > 0) {
        _wpdcb[chan]->rumble = FALSE;
    } else if (_wpadRumbleCnt[chan] == WPAD_CMD_RETRY_COUNT) {
        _wpdcb[chan]->rumble = FALSE;
        cmd.command = WPAD_HIDREP_VIBRATOR;
        cmd.len = 1;
        cmd.data[0] = 0;
        cmd.callback = NULL;
        __wpadSendDataSub(chan, cmd);
    }

    _wpadRumbleCnt[chan] = _wpdcb[chan]->rumble ? _wpadRumbleCnt[chan] + 1 : 0;
}

static void __wpadCalcSystemSettings(void) {
    if (SCCheckStatus() != SC_STATUS_OK) {
        return;
    }

    WUDUpdateSCSetting();

    if (_wpadSCSetting) {
        _wpadDpdSense = __wpadGetDpdSensitivity();
        _wpadSensorBarPos = __wpadGetSensorBarPosition();
        _wpadRumbleFlag = __wpadGetMotorMode();
        _wpadSpeakerVol = __wpadGetSpeakerVolume();
        _wpadSCSetting = FALSE;
    }
}

static void __wpadCalcAfh(void) {
    u8* channel;
    BOOL enabled;

    if (_wpadAfhCnt != WPAD_MANAGE_PERIOD_AFH) {
        return;
    }

    channel = OSPhysicalToCached(AFH_CHANNEL_ADDR);
    DCInvalidateRange(channel, sizeof(*channel));
    DEBUGPrint("Check the update of WiFi using channel\n");
    DEBUGPrint("WiFi uses channel = %d\n", *channel);

    if (_wpadAfhChannel == *channel) {
        return;
    }

    enabled = OSDisableInterrupts();
    _wpadAfhChannel = *channel;
    OSRestoreInterrupts(enabled);

    WUDSetDisableChannel(_wpadAfhChannel);
}

static void __wpadResetModule(BOOL saveSimpleDevs) {
    BTA_DmSendHciReset();
    __wpadShutdown(saveSimpleDevs);
}

static BOOL __wpadCalcReconnectWait(s32 saveSimpleDevs) {
    if (saveSimpleDevs >= 0) {
        if (--_wpadReconnectWait <= 0) {
            DEBUGPrint("Reconnect Start!!\n");
            __wpadResetModule(saveSimpleDevs);
        }
        return TRUE;
    }

    return FALSE;
}

static void __wpadManageHandler(OSAlarm* alarm, OSContext* context) {
#pragma unused(alarm, context)
    s32 status = WPADGetStatus();
    s32 chan;
    BOOL success;

    if (status != WPAD_STATE_SETUP) {
        if (status == WPAD_STATE_ENABLED && !_wpadInitialized) {
            _wpadInitialized = TRUE;
            _wpadReconnectWait = 50;
            WUDSetHidConnCallback(__wpadConnectionCallback);
            WUDSetHidRecvCallback(__wpadReceiveCallback);
        }
        return;
    }

    if (__wpadCalcReconnectWait(_wpadOnReconnect)) {
        return;
    }

    __wpadCalcAfh();
    __wpadCalcSystemSettings();

    for (chan = 0; chan < WPAD_MAX_CONTROLLERS; chan++) {
        success = FALSE;

        if (_wpdcb[chan]->used) {
            success |= __wpadProcExtCommand(chan, success);
            success |= __wpadProcCommand(chan, success);
            __wpadRumbleMotor(chan, success);
            __wpadCalcControllerData(chan);
            __wpadCalcRadioQuality(chan);
        }

        if (_wpdcb[chan]->extDisconnected && _wpdcb[chan]->extReconnectWait-- < 0) {
            _wpdcb[chan]->extDisconnected = FALSE;
        }
    }

    _wpadSenseCnt = _wpadSenseCnt == WPAD_MANAGE_PERIOD_SENSE ? 0 : _wpadSenseCnt + 1;
    _wpadCheckCnt = _wpadCheckCnt == WPAD_MANAGE_PERIOD_CHECK ? 0 : _wpadCheckCnt + 1;
    _wpadAfhCnt = _wpadAfhCnt == WPAD_MANAGE_PERIOD_AFH ? 0 : _wpadAfhCnt + 1;

    BTA_HhGetAclQueueInfo();
}

static void __wpadManageHandler0(OSAlarm* alarm, OSContext* context) {
    OSSwitchFiberEx((u32)alarm, (u32)context, 0, 0, (u32)__wpadManageHandler,
                    (u32)&__wpadManageHandlerStack[sizeof(__wpadManageHandlerStack)]);
}

static void __wpadClearControlBlock(s32 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    int i;

    p_wpd->infoBuf = NULL;
    p_wpd->motor = FALSE;
    p_wpd->resultCallback = NULL;
    p_wpd->extensionCallback = NULL;
    p_wpd->sampleCallback = NULL;
    p_wpd->samplingBufs_ptr = NULL;
    p_wpd->bufLatestIndex = 0;
    p_wpd->bufLength = 0;
    p_wpd->dataFormat = WPAD_FMT_CORE;
    p_wpd->status = WPAD_ERR_NO_CONTROLLER;
    p_wpd->devType = WPAD_DEV_NOT_FOUND;
    p_wpd->devMode = WPAD_DEV_MODE_NORMAL;
    p_wpd->isSetStickOrigin = FALSE;
    p_wpd->recalibrateCount = 0;
    p_wpd->getStatFlag = FALSE;
    p_wpd->dpdDummyObjSize = 12;
    p_wpd->dpdFormat = 0;
    p_wpd->dpdCommand = 0;

    for (i = 0; i < WPAD_MAX_NOISE_FILTERS; i++) {
        p_wpd->diffCount[i] = 0;
        p_wpd->hystCount[i] = 0;
    }

    p_wpd->lastUpdateTime = __OSGetSystemTime();
    p_wpd->cmdTimer = __OSGetSystemTime();
    p_wpd->cmdTimeoutAction = 0;
    p_wpd->reqVolume = 0;
    p_wpd->reqVolCb = NULL;
    p_wpd->readBufPtr = NULL;
    p_wpd->readBaseAddr = 0;
    p_wpd->readLength = 0;
    p_wpd->readError = 0;
    p_wpd->dev_handle = WUD_DEV_HANDLE_INVALID;
    p_wpd->used = FALSE;
    p_wpd->setup = FALSE;
    p_wpd->oldFw = FALSE;
    p_wpd->audioStop = TRUE;
    p_wpd->audioStopCnt = 0;
    p_wpd->audioFrames = 0;
    p_wpd->keyIdx = 0;
    p_wpd->radioSense = 0;
    p_wpd->packetCnt = 0;
    p_wpd->disconnect = TRUE;
    p_wpd->infoLocked = FALSE;
    p_wpd->infoCallback = NULL;
    p_wpd->pwrSave = FALSE;
    p_wpd->blcBattery = 4;
    p_wpd->savedDevType = WPAD_DEV_NOT_FOUND;
    p_wpd->extDisconnected = FALSE;
    p_wpd->extReconnectWait = 0;

    memset(&p_wpd->infos, 0, sizeof(p_wpd->infos));
    memset(p_wpd->readBuf, 0, sizeof(p_wpd->readBuf));
    memset(&p_wpd->conf, 0, sizeof(p_wpd->conf));
    memset(&p_wpd->extConf, 0, sizeof(p_wpd->extConf));
    memset(p_wpd->key, 0, sizeof(p_wpd->key));
    memset(p_wpd->ft, 0, sizeof(p_wpd->ft));
    memset(p_wpd->sb, 0, sizeof(p_wpd->sb));
    memset(&p_wpd->gameInfo, 0, sizeof(p_wpd->gameInfo));

    p_wpd->rxBufIndex = 0;
    memset(p_wpd->rxBuf, 0, sizeof(p_wpd->rxBuf));
    ((WPADStatus*)p_wpd->rxBuf[0])->err = WPAD_ERR_NO_CONTROLLER;
    ((WPADStatus*)p_wpd->rxBuf[1])->err = WPAD_ERR_NO_CONTROLLER;
    memcpy(p_wpd->prBuf, p_wpd->rxBuf[0], sizeof(p_wpd->prBuf));

    p_wpd->gameInfoErr[0] = WPAD_ERR_NO_CONTROLLER;
    p_wpd->gameInfoErr[1] = WPAD_ERR_NO_CONTROLLER;

    p_wpd->cmdq.cmd = p_wpd->cmds;
    p_wpd->cmdq.cmdlen = WPAD_COMMAND_CMD_MAX_LEN;
    p_wpd->extq.cmd = p_wpd->exts;
    p_wpd->extq.cmdlen = WPAD_COMMAND_EXT_MAX_LEN;

    WPADiClearQueue(&p_wpd->cmdq);
    WPADiClearQueue(&p_wpd->extq);
    WPADiClearMemBlock(chan);

    _wpadExtCnt[chan] = 0;
    _wpadRumbleCnt[chan] = 0;
}

static void __wpadInitSub(void) {
    s32 chan;
    int i;

    __wpadSetSensorBarPower(TRUE);

    for (i = 0; i < WUD_MAX_DEV_ENTRY; i++) {
        _wpadHandle2PortTable[i] = WUD_DEV_HANDLE_INVALID;
    }

    DEBUGPrint("WPADInit()\n");

    for (chan = 0; chan < WPAD_MAX_CONTROLLERS; chan++) {
        _wpdcb[chan] = &_wpd[chan];
        _wpadIsUsedChannel[chan] = FALSE;
        _wpdcb[chan]->connectCallback = NULL;
        __wpadClearControlBlock(chan);
        OSInitThreadQueue(&_wpd[chan].threadq);
        _wpadExtCnt[chan] = 0;
        _wpadRumbleCnt[chan] = 0;
    }

    _wpadSleepTime = 5;
    _wpadGameCode = OSGetAppGamename();
    _wpadGameType = OSGetAppType();
    _wpadDpdSense = __wpadGetDpdSensitivity();
    _wpadSensorBarPos = __wpadGetSensorBarPosition();
    _wpadRumbleFlag = __wpadGetMotorMode();
    _wpadSpeakerVol = __wpadGetSpeakerVolume();
    _wpadSenseCnt = 0;
    _wpadCheckCnt = 0;
    _wpadAfhCnt = 0;
    _wpadShutdownFlag = FALSE;
    _wpadSCSetting = TRUE;
    _wpadAfhChannel = -1;
    _wpadUsedCallback = FALSE;

    OSRegisterVersion(__WPADVersion);

    if (_wpadVSMInit) {
        _wpadVSMInit();
    }
    if (_wpadTRNInit) {
        _wpadTRNInit();
    }
    if (_wpadGTRInit) {
        _wpadGTRInit();
    }
    if (_wpadDRMInit) {
        _wpadDRMInit();
    }

    OSCreateAlarm(&_wpadManageAlarm);
    OSSetPeriodicAlarm(&_wpadManageAlarm, OSGetTime(), OSMillisecondsToTicks(1), __wpadManageHandler0);
}

void WPADInit(void) {
    _wpadStartup = TRUE;

    if (!_wpadRegisterShutdownFunc) {
        OSRegisterShutdownFunction(&ShutdownFunctionInfo);
        _wpadRegisterShutdownFunc = TRUE;
    }

    if (WUDInit()) {
        _wpadInitialized = FALSE;
        _wpadOnReconnect = -1;
        _wpadReconnectWait = 50;
        __wpadInitSub();
    }
}

BOOL WPADCancelSyncDevice(void) {
    return WUDCancelSyncDevice();
}

void WPADRegisterAllocator(WPADAlloc alloc, WPADFree free) {
    WUDRegisterAllocator(alloc, free);
}

s32 WPADGetStatus(void) {
    return WUDGetStatus();
}

void WPADGetAddress(s32 chan, u8* addr) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    s8 handle = p_wpd->dev_handle;
    u8* devAddr;

    OSRestoreInterrupts(enabled);

    devAddr = _WUDGetDevAddr(handle);
    if (devAddr) {
        memcpy(addr, devAddr, BD_ADDR_LEN);
    } else {
        memset(addr, 0, BD_ADDR_LEN);
    }
}

s32 WPADGetInfo(s32 chan, WPADInfo* info) {
    s32 result = WPADGetInfoAsync(chan, info, __wpadSyncCallback);

    if (result == WPAD_ERR_BUSY) {
        return result;
    }

    return __wpadSync(chan);
}

static s32 __wpadGetConnectionStatus(s32 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    s32 status = p_wpd->status;

    OSRestoreInterrupts(enabled);
    return status;
}

static void __wpadDisconnect(s32 chan, BOOL sleep) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled;
    u8 addr[BD_ADDR_LEN];

    if (__wpadGetConnectionStatus(chan) == WPAD_ERR_NO_CONTROLLER) {
        return;
    }

    if (sleep) {
        enabled = OSDisableInterrupts();
        if (p_wpd->disconnect) {
            OSRestoreInterrupts(enabled);
            return;
        }
        p_wpd->disconnect = TRUE;
        OSRestoreInterrupts(enabled);
        WPADControlLed(chan, 0, __wpadDisconnectCallback);
    } else {
        WPADGetAddress(chan, addr);
        btm_remove_acl(addr);
    }
}

void WPADRecalibrate(s32 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();

    p_wpd->isSetStickOrigin = FALSE;
    p_wpd->recalibrateCount = 0;
    OSRestoreInterrupts(enabled);
}

static s8 __wpadGetQueueSize(WPADCmdQueue* queue) {
    BOOL enabled = OSDisableInterrupts();
    s8 size = queue->tail - queue->head;

    if (size < 0) {
        size += queue->cmdlen;
    }

    OSRestoreInterrupts(enabled);
    return size;
}

void WPADiClearQueue(WPADCmdQueue* queue) {
    BOOL enabled = OSDisableInterrupts();

    queue->head = 0;
    queue->tail = 0;
    memset(queue->cmd, 0, sizeof(WPADCommand) * queue->cmdlen);
    OSRestoreInterrupts(enabled);
}

static BOOL __wpadGetCommand(WPADCmdQueue* queue, WPADCommand* cmd) {
    BOOL enabled;

    if (__wpadGetQueueSize(queue) == 0) {
        return FALSE;
    }

    enabled = OSDisableInterrupts();
    memcpy(cmd, &queue->cmd[queue->head], sizeof(WPADCommand));
    OSRestoreInterrupts(enabled);
    return TRUE;
}

static BOOL __wpadPopCommand(WPADCmdQueue* queue) {
    BOOL enabled = OSDisableInterrupts();

    if (__wpadGetQueueSize(queue) == 0) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    memset(&queue->cmd[queue->head], 0, sizeof(WPADCommand));
    queue->head = (u32)queue->head == queue->cmdlen - 1 ? 0 : queue->head + 1;
    OSRestoreInterrupts(enabled);
    return TRUE;
}

static BOOL __wpadSetSensorBarPower(BOOL enable) {
    BOOL enabled = OSDisableInterrupts();
    u32 reg = ACR_REG(ACR_REG_SENSOR_BAR);

    if (enable) {
        ACR_REG(ACR_REG_SENSOR_BAR) = reg | ACR_SENSOR_BAR_POWER;
    } else {
        ACR_REG(ACR_REG_SENSOR_BAR) = reg & ~ACR_SENSOR_BAR_POWER;
    }

    OSRestoreInterrupts(enabled);
    return (reg & ACR_SENSOR_BAR_POWER) ? TRUE : FALSE;
}

static void __wpadSetScreenSaverFlag(BOOL flag) {
    if (flag) {
        __VIResetRFIdle();
    }
}

static u8 __wpadGetDpdSensitivity(void) {
    u32 sense = SCGetBtDpdSensibility();

    if (sense < WPAD_MIN_DPD_SENS) {
        sense = WPAD_MIN_DPD_SENS;
    }

    if (sense > WPAD_MAX_DPD_SENS) {
        sense = WPAD_MAX_DPD_SENS;
    }

    return sense;
}

static u8 __wpadGetSensorBarPosition(void) {
    return SCGetWpadSensorBarPosition() == WPAD_SENSOR_BAR_POS_TOP ? WPAD_SENSOR_BAR_POS_TOP : WPAD_SENSOR_BAR_POS_BOTTOM;
}

static u32 __wpadGetMotorMode(void) {
    return SCGetWpadMotorMode() == 1 ? TRUE : FALSE;
}

static u8 __wpadClampSpeakerVolume(u8 volume) {
    u8 clamped = volume;

    if (volume <= 0) {
        clamped = 0;
    }

    if (volume >= 127) {
        clamped = 127;
    }

    return clamped;
}

static u8 __wpadGetSpeakerVolume(void) {
    u8 volume = SCGetWpadSpeakerVolume();

    volume = __wpadClampSpeakerVolume(volume);
    return volume;
}
