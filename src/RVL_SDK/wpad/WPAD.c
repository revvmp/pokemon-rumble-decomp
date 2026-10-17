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

#define WPAD_REPORT_CORE 0x30
#define WPAD_REPORT_CORE_ACC 0x31
#define WPAD_REPORT_CORE_EXT8 0x32
#define WPAD_REPORT_CORE_ACC_DPD 0x33
#define WPAD_REPORT_CORE_EXT19 0x34
#define WPAD_REPORT_CORE_ACC_EXT16 0x35
#define WPAD_REPORT_CORE_ACC_DPD_EXT9 0x37
#define WPAD_REPORT_CORE_EXT21 0x3D
#define WPAD_REPORT_CORE_ACC_DPD_FULL 0x3E

#define WPAD_FW_CHECK_ADDR 0x1770
#define WPAD_CONFIG_ADDR 0x0000
#define WPAD_CONFIG_ADDR_OLD_FW 0x176C
#define WPAD_CONFIG_SIZE 42
#define WPAD_CONFIG_SIZE_OLD_FW 20
#define WPAD_GAME_INFO_ADDR 0x002A

#define DPD_CTRL_REG 0x04B00030
#define DPD_CTRL_ON 0x01
#define DPD_CTRL_DONE 0x08
#define DPD_SENS_BLOCK1_REG 0x04B00000
#define DPD_SENS_BLOCK2_REG 0x04B0001A
#define DPD_MODE_REG 0x04B00033

#define WPAD_BLC_REG 0xF1

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
static void __wpadConnectionCallback(WUDDevInfo* info, u8 open);
static void __wpadReceiveCallback(u8 devHandle, u8* report, u16 len);
static void __wpadDisconnectCallback(s32 chan, s32 result);
static s32 __wpadGetConnectionStatus(s32 chan);
static void __wpadDisconnect(s32 chan, BOOL sleep);
static s8 __wpadGetQueueSize(WPADCmdQueue* queue);
static BOOL __wpadGetCommand(WPADCmdQueue* queue, WPADCommand* cmd);
static BOOL __wpadPopCommand(WPADCmdQueue* queue);
static BOOL __wpadPushCommand(WPADCmdQueue* queue, WPADCommand cmd);
static BOOL __wpadSetSensorBarPower(BOOL enable);
static void __wpadSetScreenSaverFlag(BOOL flag);
static u8 __wpadGetDpdSensitivity(void);
static u8 __wpadGetSensorBarPosition(void);
static u32 __wpadGetMotorMode(void);
static u8 __wpadClampSpeakerVolume(u8 volume);
static u8 __wpadGetSpeakerVolume(void);
static u16 __wpadGetBTEBufferStatus(s32 chan);
static u16 __wpadGetBTMBufferStatus(s32 chan);

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

BOOL WPADStartFastSimpleSync(void) {
    return WUDStartFastSyncSimple();
}

BOOL WPADStopSimpleSync(void) {
    return WUDStopSyncSimple();
}

BOOL WPADCancelSyncDevice(void) {
    return WUDCancelSyncDevice();
}

WPADSyncDeviceCallback WPADSetSimpleSyncCallback(WPADSyncDeviceCallback callback) {
    return WUDSetSyncSimpleCallback(callback);
}

void WPADRegisterAllocator(WPADAlloc alloc, WPADFree free) {
    WUDRegisterAllocator(alloc, free);
}

s32 WPADGetStatus(void) {
    return WUDGetStatus();
}

u8 WPADGetRadioSensitivity(s32 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    u8 sense = p_wpd->radioSense;

    OSRestoreInterrupts(enabled);
    return sense;
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

u8 WPADGetSensorBarPosition(void) {
    BOOL enabled = OSDisableInterrupts();
    u8 pos = _wpadSensorBarPos;

    OSRestoreInterrupts(enabled);
    return pos;
}

static void __wpadSetupConnectionCallback(s32 chan, s32 result) {
    WPADControlBlock* p_wpd = _wpdcb[chan];

    if (result == WPAD_ERR_NONE) {
        p_wpd->setup = TRUE;
        if (p_wpd->connectCallback) {
            p_wpd->connectCallback(chan, result);
        }
    } else {
        __wpadDisconnect(chan, FALSE);
    }
}

static void __wpadAbortConnectionCallback(s32 chan, s32 result) {
    WPADControlBlock* p_wpd = _wpdcb[chan];

    if (result != WPAD_ERR_NONE) {
        WPADiClearQueue(&p_wpd->cmdq);
        __wpadDisconnect(chan, FALSE);
    }
}

static void __wpadInitConnectionCallback(s32 chan, s32 result) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled;
    u16 size;
    u32 addr;
    u8 port;

    if (result == WPAD_ERR_NO_CONTROLLER) {
        return;
    }

    enabled = OSDisableInterrupts();
    p_wpd->oldFw = result == WPAD_ERR_NONE ? TRUE : FALSE;
    p_wpd->status = WPAD_ERR_NONE;
    OSRestoreInterrupts(enabled);

    size = result == WPAD_ERR_NONE ? WPAD_CONFIG_SIZE_OLD_FW : WPAD_CONFIG_SIZE;
    addr = result == WPAD_ERR_NONE ? WPAD_CONFIG_ADDR_OLD_FW : WPAD_CONFIG_ADDR;

    DEBUGPrint(" ==>this error means that the firmware is for NDEV %s\n", p_wpd->oldFw ? "2.0" : "2.1 or later");

    port = p_wpd->devType == WPAD_DEV_BALANCE_CHECKER ? 1 : 1 << chan;

    WPADiSendSetReportType(&p_wpd->cmdq, WPAD_FMT_CORE, p_wpd->pwrSave, __wpadAbortConnectionCallback);
    WPADiSendDPDCSB(&p_wpd->cmdq, FALSE, __wpadAbortConnectionCallback);
    WPADiSendSetPort(&p_wpd->cmdq, port, __wpadAbortConnectionCallback);
    WPADiSendReadData(&p_wpd->cmdq, p_wpd->readBuf, sizeof(WPADMEMGameInfo), WPAD_GAME_INFO_ADDR,
                      __wpadAbortConnectionCallback);
    WPADiSendReadData(&p_wpd->cmdq, p_wpd->readBuf, sizeof(WPADMEMGameInfo),
                      WPAD_GAME_INFO_ADDR + sizeof(WPADMEMGameInfo), __wpadAbortConnectionCallback);
    WPADiSendReadData(&p_wpd->cmdq, p_wpd->readBuf, size, addr, __wpadSetupConnectionCallback);
    WPADiSendGetContStat(&p_wpd->cmdq, NULL, NULL);
}

static s32 __wpadRetrieveChannel(WUDDevInfo* info) {
    u8* devAddr;
    int i;
    s32 chan = WPAD_CHAN_INVALID;

    devAddr = _WUDGetDevAddr(info->devHandle);

    if (!memcmp(info->conf.devName, "Nintendo RVL-WBC", 16)) {
        chan = WPAD_CHAN3;
        if (_wpdcb[chan]->used) {
            btm_remove_acl(devAddr);
            return WPAD_CHAN_INVALID;
        }
    } else {
        for (i = 0; i < WPAD_MAX_CONTROLLERS; i++) {
            if (WUDIsLatestDevice(i, devAddr) && !_wpadIsUsedChannel[i]) {
                chan = i;
                break;
            }

            if (!_wpadIsUsedChannel[i] && chan < WPAD_CHAN0) {
                chan = i;
            }
        }
    }

    if (!WUDIsLatestDevice(chan, devAddr)) {
        WUDSetDeviceHistory(chan, devAddr);
    }

    _wpadIsUsedChannel[chan] = TRUE;
    return chan;
}

static void __wpadConnectionCallback(WUDDevInfo* info, u8 open) {
    u8 handle = info->devHandle;
    s32 chan;
    WPADControlBlock* p_wpd;
    WPADCommand cmd;

    DEBUGPrint("connection is %s\n", open ? "opened" : "closed");

    if (open) {
        chan = __wpadRetrieveChannel(info);
        if (chan < 0) {
            return;
        }

        _wpadHandle2PortTable[handle] = chan;
        __wpadClearControlBlock(chan);
        p_wpd = _wpdcb[chan];

        if (!memcmp(info->conf.devName, "Nintendo RVL-CNT", 16)) {
            p_wpd->devType = WPAD_DEV_CORE;
            p_wpd->dataFormat = WPAD_FMT_CORE;
        } else if (!memcmp(info->conf.devName, "Nintendo RVL-WBC", 16) && WUDIsLinkedWBC()) {
            p_wpd->devType = WPAD_DEV_BALANCE_CHECKER;
            p_wpd->dataFormat = WPAD_FMT_BALANCE_CHECKER;
        } else {
            p_wpd->devType = WPAD_DEV_FUTURE;
            p_wpd->dataFormat = WPAD_FMT_CORE;
        }

        p_wpd->dev_handle = handle;
        p_wpd->used = TRUE;
        p_wpd->status = WPAD_ERR_NONE;
        p_wpd->radioSense = 100;
        p_wpd->disconnect = FALSE;
        p_wpd->extEnc = 0;
        p_wpd->savedDevType = p_wpd->devType;

        WPADiSendReadData(&p_wpd->cmdq, p_wpd->readBuf, 1, WPAD_FW_CHECK_ADDR, __wpadInitConnectionCallback);
        __wpadSetScreenSaverFlag(TRUE);
    } else {
        chan = _wpadHandle2PortTable[handle];
        _wpadHandle2PortTable[handle] = WUD_DEV_HANDLE_INVALID;

        if (chan != WPAD_CHAN_INVALID) {
            p_wpd = _wpdcb[chan];
            p_wpd->status = WPAD_ERR_NO_CONTROLLER;

            if (p_wpd->resultCallback) {
                p_wpd->resultCallback(chan, WPAD_ERR_NO_CONTROLLER);
            } else if (p_wpd->memBlock.callback) {
                p_wpd->memBlock.callback(chan, WPAD_ERR_NO_CONTROLLER);
            }

            while (__wpadGetCommand(&p_wpd->cmdq, &cmd)) {
                if (cmd.callback) {
                    cmd.callback(chan, WPAD_ERR_NO_CONTROLLER);
                }
                __wpadPopCommand(&p_wpd->cmdq);
            }

            DEBUGPrint("clean up command queue\n");

            if (p_wpd->samplingBufs_ptr) {
                WPADSetAutoSamplingBuf(chan, p_wpd->samplingBufs_ptr, p_wpd->bufLength);
            }

            __wpadClearControlBlock(chan);
            _wpadIsUsedChannel[chan] = FALSE;

            if (p_wpd->connectCallback) {
                p_wpd->connectCallback(chan, WPAD_ERR_NO_CONTROLLER);
            }
        } else {
            DEBUGPrint("WARNING: disconnection for device handle not assigned to channel.\n");
        }
    }
}

static void __wpadReceiveCallback(u8 devHandle, u8* report, u16 len) {
#pragma unused(len)
    u8 port = _wpadHandle2PortTable[devHandle];
    s32 result;

    if (port < WPAD_MAX_CONTROLLERS) {
        result = WPADiHIDParser(port, report);
        if (result) {
            DEBUGPrint("HID Parser reports: %d\n", result);
        }
    }
}

void WPADGetAccGravityUnit(s32 chan, u32 type, WPADAcc* acc) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();

    if (acc) {
        switch (type) {
        case WPAD_DEV_CORE:
            acc->x = p_wpd->conf.acc_1g.x - p_wpd->conf.acc_0g.x;
            acc->y = p_wpd->conf.acc_1g.y - p_wpd->conf.acc_0g.y;
            acc->z = p_wpd->conf.acc_1g.z - p_wpd->conf.acc_0g.z;
            break;
        case WPAD_DEV_FREESTYLE:
            acc->x = p_wpd->extConf.fs.acc_1g.x - p_wpd->extConf.fs.acc_0g.x;
            acc->y = p_wpd->extConf.fs.acc_1g.y - p_wpd->extConf.fs.acc_0g.y;
            acc->z = p_wpd->extConf.fs.acc_1g.z - p_wpd->extConf.fs.acc_0g.z;
            break;
        }
    }

    OSRestoreInterrupts(enabled);
}

static s32 __wpadGetConnectionStatus(s32 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    s32 status = p_wpd->status;

    OSRestoreInterrupts(enabled);
    return status;
}

static void __wpadDisconnectCallback(s32 chan, s32 result) {
    WPADControlBlock* p_wpd = _wpdcb[chan];

    if (result != WPAD_ERR_NO_CONTROLLER) {
        BTA_HhClose(p_wpd->dev_handle);
    }
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

void WPADDisconnect(s32 chan) {
    if (__wpadGetConnectionStatus(chan) == WPAD_ERR_NO_CONTROLLER) {
        return;
    }

    WUDSetDeviceHistory(chan, NULL);
    __wpadDisconnect(chan, TRUE);
}

s32 WPADProbe(s32 chan, u32* type) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    s32 status;

    if (type) {
        *type = p_wpd->devType;
    }

    status = p_wpd->status;
    if (status != WPAD_ERR_NO_CONTROLLER) {
        if (p_wpd->devType == WPAD_DEV_NOT_FOUND) {
            status = WPAD_ERR_NO_CONTROLLER;
        } else if (!p_wpd->setup) {
            status = WPAD_ERR_BUSY;
        }
    }

    OSRestoreInterrupts(enabled);
    return status;
}

WPADSamplingCallback WPADSetSamplingCallback(s32 chan, WPADSamplingCallback callback) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    WPADSamplingCallback old;
    BOOL enabled;

    if (WPADIsUsedCallbackByKPAD()) {
        OSReport("WARNING: Overwritten the callback needed by KPAD.\n");
        OSReport("         Please call KPADSetSamplingCallback instead of WPADSetSamplingCallback.\n");
    }

    enabled = OSDisableInterrupts();
    old = p_wpd->sampleCallback;
    p_wpd->sampleCallback = callback;
    OSRestoreInterrupts(enabled);
    return old;
}

WPADConnectCallback WPADSetConnectCallback(s32 chan, WPADConnectCallback callback) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    WPADConnectCallback old;
    BOOL enabled;

    if (WPADIsUsedCallbackByKPAD()) {
        OSReport("WARNING: Overwritten the callback needed by KPAD.\n");
        OSReport("         Please call KPADSetConnectCallback instead of WPADSetConnectCallback.\n");
    }

    enabled = OSDisableInterrupts();
    old = p_wpd->connectCallback;
    p_wpd->connectCallback = callback;
    OSRestoreInterrupts(enabled);
    return old;
}

WPADExtensionCallback WPADSetExtensionCallback(s32 chan, WPADExtensionCallback callback) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    WPADExtensionCallback old;
    BOOL enabled = OSDisableInterrupts();

    old = p_wpd->extensionCallback;
    p_wpd->extensionCallback = callback;
    OSRestoreInterrupts(enabled);
    return old;
}

u32 WPADGetDataFormat(s32 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    u32 fmt = p_wpd->dataFormat;

    OSRestoreInterrupts(enabled);
    return fmt;
}

s32 WPADSetDataFormat(s32 chan, u32 fmt) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    s32 status;
    BOOL enabled = OSDisableInterrupts();
    BOOL setup = p_wpd->setup;
    u32 currFmt;

    status = p_wpd->status;
    currFmt = p_wpd->dataFormat;
    OSRestoreInterrupts(enabled);

    if (status == WPAD_ERR_NO_CONTROLLER) {
        return WPAD_ERR_NO_CONTROLLER;
    }

    if (!setup) {
        return WPAD_ERR_BUSY;
    }

    if (currFmt == fmt) {
        return WPAD_ERR_NONE;
    }

    if (WPADiSendSetReportType(&p_wpd->cmdq, fmt, p_wpd->pwrSave, NULL)) {
        enabled = OSDisableInterrupts();
        p_wpd->dataFormat = fmt;
        OSRestoreInterrupts(enabled);
        return WPAD_ERR_NONE;
    }

    return WPAD_ERR_BUSY;
}

s32 WPADGetInfo(s32 chan, WPADInfo* info) {
    s32 result = WPADGetInfoAsync(chan, info, __wpadSyncCallback);

    if (result == WPAD_ERR_BUSY) {
        return result;
    }

    return __wpadSync(chan);
}

static void __wpadInfoCallback(s32 chan, s32 result) {
    WPADControlBlock* p_wpd = _wpdcb[chan];

    if (p_wpd->infoCallback) {
        p_wpd->infoCallback(chan, result);
    }

    p_wpd->infoCallback = NULL;
    p_wpd->infoLocked = FALSE;
}

s32 WPADGetInfoAsync(s32 chan, WPADInfo* info, WPADCallback callback) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled;
    BOOL setup;
    s32 status;
    u8 locked;

    enabled = OSDisableInterrupts();
    setup = p_wpd->setup;
    status = p_wpd->status;
    locked = p_wpd->infoLocked;
    OSRestoreInterrupts(enabled);

    if (status == WPAD_ERR_NO_CONTROLLER) {
        goto end;
    }

    if (!setup || locked) {
        status = WPAD_ERR_BUSY;
        goto end;
    }

    enabled = OSDisableInterrupts();
    p_wpd->infoLocked = TRUE;
    p_wpd->infoCallback = callback;
    OSRestoreInterrupts(enabled);

    if (WPADiSendGetContStat(&p_wpd->cmdq, info, __wpadInfoCallback)) {
        status = WPAD_ERR_NONE;
        goto end;
    }

    status = WPAD_ERR_BUSY;

    enabled = OSDisableInterrupts();
    p_wpd->infoLocked = FALSE;
    p_wpd->infoCallback = NULL;
    OSRestoreInterrupts(enabled);

end:
    if (status != WPAD_ERR_NONE && callback) {
        callback(chan, status);
    }

    return status;
}

void WPADControlMotor(s32 chan, u32 command) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();

    if (p_wpd->status == WPAD_ERR_NO_CONTROLLER) {
        OSRestoreInterrupts(enabled);
        return;
    }

    if (!_wpadRumbleFlag && (command != WPAD_MOTOR_STOP || p_wpd->motor != TRUE)) {
        OSRestoreInterrupts(enabled);
        return;
    }

    if ((command == WPAD_MOTOR_STOP && p_wpd->motor == FALSE) ||
        (command == WPAD_MOTOR_RUMBLE && p_wpd->motor == TRUE))
    {
        OSRestoreInterrupts(enabled);
        return;
    }

    p_wpd->motor = command != WPAD_MOTOR_STOP ? TRUE : FALSE;
    p_wpd->rumble = TRUE;
    OSRestoreInterrupts(enabled);
}

void WPADEnableMotor(BOOL enable) {
    BOOL enabled = OSDisableInterrupts();

    _wpadRumbleFlag = enable;
    OSRestoreInterrupts(enabled);
}

BOOL WPADIsMotorEnabled(void) {
    BOOL enabled = OSDisableInterrupts();
    BOOL flag = _wpadRumbleFlag;

    OSRestoreInterrupts(enabled);
    return flag;
}

s32 WPADControlLed(s32 chan, u8 pattern, WPADCallback callback) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL success;
    BOOL enabled;
    BOOL setup;
    s32 status;

    status = WPAD_ERR_NONE;
    enabled = OSDisableInterrupts();
    status = p_wpd->status;
    setup = p_wpd->setup;
    OSRestoreInterrupts(enabled);

    if (status == WPAD_ERR_NO_CONTROLLER) {
        goto end;
    }

    if (!setup) {
        status = WPAD_ERR_BUSY;
        goto end;
    }

    success = WPADiSendSetPort(&p_wpd->cmdq, pattern, callback);
    status = success ? WPAD_ERR_NONE : WPAD_ERR_BUSY;

end:
    if (status != WPAD_ERR_NONE && callback) {
        callback(chan, status);
    }

    return status;
}

BOOL WPADSaveConfig(SCFlushCallback callback) {
    BOOL success = TRUE;
    BOOL enabled;
    u8 volume;
    u8 motor;

    if (SCCheckStatus() != SC_STATUS_OK) {
        return FALSE;
    }

    enabled = OSDisableInterrupts();
    volume = _wpadSpeakerVol;
    motor = _wpadRumbleFlag ? TRUE : FALSE;
    OSRestoreInterrupts(enabled);

    success &= SCSetWpadSpeakerVolume(volume);
    success &= SCSetWpadMotorMode(motor);

    if (success) {
        SCFlushAsync(callback);
    } else if (callback) {
        callback(SC_STATUS_ERROR);
    }

    return success;
}

static u32 __wpadFmt2Size(u32 fmt) {
    switch (fmt) {
    case WPAD_FMT_FREESTYLE:
    case WPAD_FMT_FREESTYLE_ACC:
    case WPAD_FMT_FREESTYLE_ACC_DPD:
        return sizeof(WPADFSStatus);
    case WPAD_FMT_CLASSIC:
    case WPAD_FMT_CLASSIC_ACC:
    case WPAD_FMT_CLASSIC_ACC_DPD:
    case WPAD_FMT_GUITAR:
    case WPAD_FMT_DRUM:
        return sizeof(WPADCLStatus);
    case WPAD_FMT_TRAIN:
        return sizeof(WPADTRStatus);
    case WPAD_FMT_BALANCE_CHECKER:
        return sizeof(WPADBLStatus);
    case WPAD_FMT_VSM:
    case WPAD_FMT_VSM_EXTENDED:
        return sizeof(WPADVSStatus);
    case WPAD_FMT_CORE_ACC_DPD_FULL:
        return sizeof(WPADStatusEx);
    default:
        return sizeof(WPADStatus);
    }
}

void WPADRead(s32 chan, void* status) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    u8 index = p_wpd->rxBufIndex != 0 ? 0 : 1;
    WPADStatus* rxStatus = (WPADStatus*)p_wpd->rxBuf[index];
    u32 size = __wpadFmt2Size(p_wpd->dataFormat);

    if (rxStatus->err != WPAD_ERR_NONE) {
        size = sizeof(WPADStatus);
    }

    memcpy(status, rxStatus, size);
    OSRestoreInterrupts(enabled);
}

void WPADSetAutoSamplingBuf(s32 chan, void* buf, u32 length) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    s8 err = p_wpd->status == WPAD_ERR_NO_CONTROLLER ? WPAD_ERR_NO_CONTROLLER : WPAD_ERR_INVALID;
    u32 size = __wpadFmt2Size(p_wpd->dataFormat);
    int i;

    if (buf) {
        memset(buf, 0, size * length);

        for (i = 0; i < length; i++) {
            ((WPADStatus*)((u32)buf + i * size))->err = err;
        }

        p_wpd->bufLatestIndex = -1;
        p_wpd->bufLength = length;
    }

    p_wpd->samplingBufs_ptr = buf;
    OSRestoreInterrupts(enabled);
}

void WPADiExcludeButton(s32 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    u8 index = p_wpd->rxBufIndex != 0 ? 0 : 1;
    void* rxBuf = p_wpd->rxBuf[index];
    WPADStatus* status = rxBuf;
    WPADCLStatus* clStatus;
    WPADTRStatus* trStatus;

    if ((status->button & WPAD_BTN_HCURSOR_MASK) == WPAD_BTN_HCURSOR_MASK) {
        status->button &= ~WPAD_BUTTON_RIGHT;
    }

    if ((status->button & WPAD_BTN_VCURSOR_MASK) == WPAD_BTN_VCURSOR_MASK) {
        status->button &= ~WPAD_BUTTON_DOWN;
    }

    if (p_wpd->dataFormat == WPAD_FMT_CLASSIC || p_wpd->dataFormat == WPAD_FMT_CLASSIC_ACC ||
        p_wpd->dataFormat == WPAD_FMT_CLASSIC_ACC_DPD || p_wpd->dataFormat == WPAD_FMT_GUITAR ||
        p_wpd->dataFormat == WPAD_FMT_DRUM)
    {
        clStatus = rxBuf;

        if ((clStatus->clButton & WPAD_CLB_HCURSOR_MASK) == WPAD_CLB_HCURSOR_MASK) {
            clStatus->clButton &= ~WPAD_CL_BUTTON_RIGHT;
        }

        if ((clStatus->clButton & WPAD_CLB_VCURSOR_MASK) == WPAD_CLB_VCURSOR_MASK) {
            clStatus->clButton &= ~WPAD_CL_BUTTON_DOWN;
        }
    }

    if (p_wpd->dataFormat == WPAD_FMT_TRAIN) {
        trStatus = rxBuf;

        if ((trStatus->trButton & WPAD_CLB_HCURSOR_MASK) == WPAD_CLB_HCURSOR_MASK) {
            trStatus->trButton &= ~WPAD_CL_BUTTON_RIGHT;
        }

        if ((trStatus->trButton & WPAD_CLB_VCURSOR_MASK) == WPAD_CLB_VCURSOR_MASK) {
            trStatus->trButton &= ~WPAD_CL_BUTTON_DOWN;
        }
    }

    OSRestoreInterrupts(enabled);
}

void WPADiCopyOut(s32 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    u32 size;
    u8 index = p_wpd->rxBufIndex != 0 ? 0 : 1;
    WPADStatus* status = (WPADStatus*)p_wpd->rxBuf[index];
    WPADStatus* out;

    size = __wpadFmt2Size(p_wpd->dataFormat);

    if (p_wpd->samplingBufs_ptr) {
        p_wpd->bufLatestIndex++;
        if (p_wpd->bufLatestIndex >= p_wpd->bufLength) {
            p_wpd->bufLatestIndex = 0;
        }

        out = (WPADStatus*)((u8*)p_wpd->samplingBufs_ptr + p_wpd->bufLatestIndex * size);

        if (status->err != WPAD_ERR_NONE) {
            size = sizeof(WPADStatus);
        }

        memcpy(out, status, size);
    }

    if (p_wpd->sampleCallback) {
        p_wpd->sampleCallback(chan);
    }

    p_wpd->packetCnt++;
    OSRestoreInterrupts(enabled);
}

BOOL WPADIsSpeakerEnabled(s32 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    BOOL speaker = p_wpd->infos.speaker;

    OSRestoreInterrupts(enabled);
    return speaker;
}

s32 WPADControlSpeaker(s32 chan, u32 command, WPADCallback callback) {
    u8 data[7] = {0x00, 0x00, 0xD0, 0x07, 0x40, 0x0C, 0x0E};
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    BOOL speaker = p_wpd->infos.speaker;
    BOOL setup;
    s32 status = p_wpd->status;

    setup = p_wpd->setup;
    OSRestoreInterrupts(enabled);

    if (status == WPAD_ERR_NO_CONTROLLER) {
        goto end;
    }

    if (!setup) {
        status = WPAD_ERR_BUSY;
        goto end;
    }

    if (command == WPAD_SPEAKER_OFF) {
        if (!speaker) {
            status = WPAD_ERR_NONE;
            goto end;
        }

        enabled = OSDisableInterrupts();

        if (WPADiIsAvailableCmdQueue(&p_wpd->cmdq, 5)) {
            WPADiSendMuteSpeaker(&p_wpd->cmdq, TRUE, NULL);
            WPADiSendWriteDataCmd(&p_wpd->cmdq, SPK_DERESET, SPK_RST_REG, NULL);
            WPADiSendWriteDataCmd(&p_wpd->cmdq, SPK_CLK_OFF, SPK_CLK_REG, NULL);
            WPADiSendEnableSpeaker(&p_wpd->cmdq, FALSE, NULL);
            WPADiSendGetContStat(&p_wpd->cmdq, NULL, callback);
            OSRestoreInterrupts(enabled);
            return WPAD_ERR_NONE;
        }

        status = WPAD_ERR_BUSY;
        OSRestoreInterrupts(enabled);
    } else {
        switch (command) {
        case WPAD_SPEAKER_ON:
        case WPAD_SPEAKER_REINIT:
            enabled = OSDisableInterrupts();

            if (WPADiIsAvailableCmdQueue(&p_wpd->cmdq, 7)) {
                WPADiSendEnableSpeaker(&p_wpd->cmdq, TRUE, NULL);
                WPADiSendMuteSpeaker(&p_wpd->cmdq, TRUE, NULL);
                WPADiSendWriteDataCmd(&p_wpd->cmdq, SPK_CLK_ON, SPK_CLK_REG, NULL);
                WPADiSendWriteDataCmd(&p_wpd->cmdq, SPK_RESET, SPK_RST_REG, NULL);
                data[4] = _wpadSpeakerVol;
                WPADiSendWriteData(&p_wpd->cmdq, data, sizeof(data), SPK_RST_REG, NULL);
                WPADiSendMuteSpeaker(&p_wpd->cmdq, FALSE, NULL);
                WPADiSendGetContStat(&p_wpd->cmdq, NULL, callback);
                OSRestoreInterrupts(enabled);
                return WPAD_ERR_NONE;
            }

            status = WPAD_ERR_BUSY;
            OSRestoreInterrupts(enabled);
            break;
        case WPAD_SPEAKER_MUTE:
            if (!WPADiSendMuteSpeaker(&p_wpd->cmdq, TRUE, callback)) {
                status = WPAD_ERR_BUSY;
                goto end;
            }
            return WPAD_ERR_NONE;
        case WPAD_SPEAKER_UNMUTE:
            if (!WPADiSendMuteSpeaker(&p_wpd->cmdq, FALSE, callback)) {
                status = WPAD_ERR_BUSY;
                goto end;
            }
            return WPAD_ERR_NONE;
        case WPAD_SPEAKER_PLAY:
            if (!WPADiSendWriteDataCmd(&p_wpd->cmdq, SPK_CTRL_PLAY, SPK_CTRL_REG, callback)) {
                status = WPAD_ERR_BUSY;
                goto end;
            }
            return WPAD_ERR_NONE;
        }
    }

end:
    if (callback) {
        callback(chan, status);
    }

    return status;
}

u8 WPADGetSpeakerVolume(void) {
    BOOL enabled = OSDisableInterrupts();
    u8 volume = _wpadSpeakerVol;

    OSRestoreInterrupts(enabled);
    return volume;
}

void WPADSetSpeakerVolume(u8 volume) {
    BOOL enabled = OSDisableInterrupts();

    _wpadSpeakerVol = __wpadClampSpeakerVolume(volume);
    OSRestoreInterrupts(enabled);
}

static BOOL __wpadIsBusyStream(s32 chan) {
    BOOL enabled;
    WPADControlBlock* p_wpd = _wpdcb[chan];
    u8 audioStop;
    u8 devType;
    u8 bufStatus;
    u16 queuedSize;
    u16 notAckedSize;
    u8 frames;
    s8 queueSize;
    u8 linkNum;

    enabled = OSDisableInterrupts();
    audioStop = p_wpd->audioStop;
    devType = p_wpd->devType;
    bufStatus = WUDGetBufferStatus();
    queueSize = __wpadGetQueueSize(&p_wpd->cmdq);
    queuedSize = __wpadGetBTEBufferStatus(chan);
    notAckedSize = __wpadGetBTMBufferStatus(chan);
    frames = p_wpd->audioFrames;
    linkNum = _WUDGetLinkNumber();
    OSRestoreInterrupts(enabled);

    if (audioStop || notAckedSize > 3 || bufStatus == 10 || bufStatus >= linkNum * 2 + 2 ||
        devType == WPAD_DEV_UNKNOWN || queueSize >= WPAD_COMMAND_CMD_MAX_LEN - 3 || frames >= 1)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL WPADCanSendStreamData(s32 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    BOOL setup;
    s32 status = p_wpd->status;

    setup = p_wpd->setup;
    OSRestoreInterrupts(enabled);

    if (status == WPAD_ERR_NO_CONTROLLER || !setup || __wpadIsBusyStream(chan)) {
        return FALSE;
    }

    return TRUE;
}

s32 WPADSendStreamData(s32 chan, void* p_buf, u16 len) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    BOOL setup;
    s32 status = p_wpd->status;

    setup = p_wpd->setup;
    OSRestoreInterrupts(enabled);

    if (status == WPAD_ERR_NO_CONTROLLER) {
        return WPAD_ERR_NO_CONTROLLER;
    }

    if (!setup) {
        return WPAD_ERR_BUSY;
    }

    if (__wpadIsBusyStream(chan)) {
        return WPAD_ERR_BUSY;
    }

    if (!WPADiSendStreamData(&p_wpd->cmdq, p_buf, len)) {
        return WPAD_ERR_BUSY;
    }

    enabled = OSDisableInterrupts();
    p_wpd->audioFrames++;
    OSRestoreInterrupts(enabled);
    return WPAD_ERR_NONE;
}

u8 WPADGetDpdSensitivity(void) {
    BOOL enabled = OSDisableInterrupts();
    u8 sense = _wpadDpdSense;

    OSRestoreInterrupts(enabled);
    return sense;
}

BOOL WPADIsDpdEnabled(s32 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    BOOL dpd = p_wpd->infos.dpd;

    OSRestoreInterrupts(enabled);
    return dpd;
}

u32 WPADGetDpdFormat(s32 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    u8 format;

    if (p_wpd->infos.dpd) {
        format = p_wpd->dpdFormat;
    } else {
        format = WPAD_DPD_DISABLE;
    }

    OSRestoreInterrupts(enabled);
    return format;
}

static void __wpadDpdCallback(s32 chan, s32 result) {
#pragma unused(result)
    WPADControlBlock* p_wpd = _wpdcb[chan];

    p_wpd->dpdFormat = p_wpd->dpdCommand;
    p_wpd->dpdBusy = FALSE;
    p_wpd->infos.dpd = p_wpd->dpdCommand != WPAD_DPD_DISABLE ? TRUE : FALSE;
}

s32 WPADControlDpd(s32 chan, u32 command, WPADCallback callback) {
    static const u8 cfg1[WPAD_MAX_DPD_SENS][9] = {
        {0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0x64, 0x00, 0xFE},
        {0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0x96, 0x00, 0xB4},
        {0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0xAA, 0x00, 0x64},
        {0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0xC8, 0x00, 0x36},
        {0x07, 0x00, 0x00, 0x71, 0x01, 0x00, 0x72, 0x00, 0x20},
    };
    static const u8 cfg2[WPAD_MAX_DPD_SENS][2] = {
        {0xFD, 0x05}, {0xB3, 0x04}, {0x63, 0x03}, {0x35, 0x03}, {0x1F, 0x03},
    };

    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    BOOL dpd = p_wpd->infos.dpd;
    BOOL setup;
    u8 pending = p_wpd->dpdCommand;
    s32 status = p_wpd->status;

    setup = p_wpd->setup;
    OSRestoreInterrupts(enabled);

    if (status == WPAD_ERR_NO_CONTROLLER) {
        goto end;
    }

    if (!setup) {
        status = WPAD_ERR_BUSY;
        goto end;
    }

    if (command == WPAD_DPD_DISABLE) {
        if (!dpd) {
            status = WPAD_ERR_NONE;
            goto end;
        }

        enabled = OSDisableInterrupts();

        if (WPADiIsAvailableCmdQueue(&p_wpd->cmdq, 3)) {
            p_wpd->dpdCommand = command;
            WPADiSendEnableDPD(&p_wpd->cmdq, FALSE, NULL);
            WPADiSendDPDCSB(&p_wpd->cmdq, FALSE, __wpadDpdCallback);
            WPADiSendGetContStat(&p_wpd->cmdq, NULL, callback);
            OSRestoreInterrupts(enabled);
            return WPAD_ERR_NONE;
        }

        status = WPAD_ERR_BUSY;
        OSRestoreInterrupts(enabled);
    } else {
        if (command == pending) {
            goto end;
        }

        enabled = OSDisableInterrupts();

        if (WPADiIsAvailableCmdQueue(&p_wpd->cmdq, 8)) {
            p_wpd->dpdCommand = command;
            p_wpd->dpdBusy = TRUE;
            WPADiSendEnableDPD(&p_wpd->cmdq, TRUE, NULL);
            WPADiSendDPDCSB(&p_wpd->cmdq, TRUE, NULL);
            WPADiSendWriteDataCmd(&p_wpd->cmdq, DPD_CTRL_ON, DPD_CTRL_REG, NULL);
            WPADiSendWriteData(&p_wpd->cmdq, (void*)cfg1[_wpadDpdSense - 1], sizeof(cfg1[0]), DPD_SENS_BLOCK1_REG,
                               NULL);
            WPADiSendWriteData(&p_wpd->cmdq, (void*)cfg2[_wpadDpdSense - 1], sizeof(cfg2[0]), DPD_SENS_BLOCK2_REG,
                               NULL);
            WPADiSendWriteDataCmd(&p_wpd->cmdq, command, DPD_MODE_REG, NULL);
            WPADiSendWriteDataCmd(&p_wpd->cmdq, DPD_CTRL_DONE, DPD_CTRL_REG, __wpadDpdCallback);
            WPADiSendGetContStat(&p_wpd->cmdq, NULL, callback);
            OSRestoreInterrupts(enabled);
            return WPAD_ERR_NONE;
        }

        status = WPAD_ERR_BUSY;
        OSRestoreInterrupts(enabled);
    }

end:
    if (callback) {
        callback(chan, status);
    }

    return status;
}

s32 WPADControlBLC(s32 chan, u8 command, WPADCallback callback) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    s32 status = p_wpd->status;
    BOOL setup = p_wpd->setup;
    u8 data[7];

    OSRestoreInterrupts(enabled);

    if (status == WPAD_ERR_NO_CONTROLLER) {
        goto end;
    }

    if (!setup || !WUDIsLinkedWBC()) {
        status = WPAD_ERR_BUSY;
        goto end;
    }

    data[0] = data[1] = data[2] = 0xAA;
    data[3] = 0x55;
    data[4] = data[5] = data[6] = command;

    enabled = OSDisableInterrupts();

    switch (command) {
    case WPAD_BLC_ENABLE:
        if (WPADiIsAvailableCmdQueue(&p_wpd->cmdq, 4)) {
            WPADWriteExtReg(chan, data, sizeof(data), WPAD_BLC_REG, NULL);
            WPADWriteExtReg(chan, data, 1, WPAD_BLC_REG, NULL);
            WPADWriteExtReg(chan, data, 1, WPAD_BLC_REG, NULL);
            WPADWriteExtReg(chan, data, 1, WPAD_BLC_REG, callback);
            OSRestoreInterrupts(enabled);
            return WPAD_ERR_NONE;
        }

        status = WPAD_ERR_BUSY;
        break;
    case WPAD_BLC_UPDATE:
        status = WPADWriteExtReg(chan, data, sizeof(data), WPAD_BLC_REG, callback);
        if (status == WPAD_ERR_NONE) {
            OSRestoreInterrupts(enabled);
            return WPAD_ERR_NONE;
        }
        break;
    case WPAD_BLC_DISABLE:
        status = WPADWriteExtReg(chan, data, 1, WPAD_BLC_REG, callback);
        if (status == WPAD_ERR_NONE) {
            OSRestoreInterrupts(enabled);
            return WPAD_ERR_NONE;
        }
        break;
    default:
        status = WPAD_ERR_BUSY;
        break;
    }

    OSRestoreInterrupts(enabled);

end:
    if (callback) {
        callback(chan, status);
    }

    return status;
}

BOOL WPADiSendSetPort(WPADCmdQueue* queue, u8 pattern, WPADCallback callback) {
    WPADCommand cmd;

    cmd.command = WPAD_HIDREP_PORT;
    cmd.len = 1;
    cmd.data[0] = pattern << 4;
    cmd.callback = callback;

    return __wpadPushCommand(queue, cmd);
}

BOOL WPADiSendSetReportType(WPADCmdQueue* queue, u32 fmt, BOOL powerSave, WPADCallback callback) {
    WPADCommand cmd;

    cmd.command = WPAD_HIDREP_REPTYPE;
    cmd.len = 2;
    cmd.data[0] = powerSave ? 0 : 4;
    cmd.callback = callback;

    switch (fmt) {
    case WPAD_FMT_CORE:
        cmd.data[1] = WPAD_REPORT_CORE;
        break;
    case WPAD_FMT_CORE_ACC:
        cmd.data[1] = WPAD_REPORT_CORE_ACC;
        break;
    case WPAD_FMT_CORE_ACC_DPD:
        cmd.data[1] = WPAD_REPORT_CORE_ACC_DPD;
        break;
    case WPAD_FMT_FREESTYLE:
        cmd.data[1] = WPAD_REPORT_CORE_EXT8;
        break;
    case WPAD_FMT_FREESTYLE_ACC:
        cmd.data[1] = WPAD_REPORT_CORE_ACC_EXT16;
        break;
    case WPAD_FMT_FREESTYLE_ACC_DPD:
        cmd.data[1] = WPAD_REPORT_CORE_ACC_DPD_EXT9;
        break;
    case WPAD_FMT_CLASSIC:
        cmd.data[1] = WPAD_REPORT_CORE_EXT8;
        break;
    case WPAD_FMT_CLASSIC_ACC:
        cmd.data[1] = WPAD_REPORT_CORE_ACC_EXT16;
        break;
    case WPAD_FMT_CLASSIC_ACC_DPD:
        cmd.data[1] = WPAD_REPORT_CORE_ACC_DPD_EXT9;
        break;
    case WPAD_FMT_CORE_ACC_DPD_FULL:
        cmd.data[1] = WPAD_REPORT_CORE_ACC_DPD_FULL;
        break;
    case WPAD_FMT_TRAIN:
        cmd.data[1] = WPAD_REPORT_CORE_EXT8;
        break;
    case WPAD_FMT_GUITAR:
        cmd.data[1] = WPAD_REPORT_CORE_ACC_DPD_EXT9;
        break;
    case WPAD_FMT_DRUM:
        cmd.data[1] = WPAD_REPORT_CORE_ACC_DPD_EXT9;
        break;
    case WPAD_FMT_BALANCE_CHECKER:
        cmd.data[1] = WPAD_REPORT_CORE_EXT19;
        break;
    case WPAD_FMT_VSM:
        cmd.data[1] = WPAD_REPORT_CORE_ACC_EXT16;
        break;
    case WPAD_FMT_VSM_EXTENDED:
        cmd.data[1] = WPAD_REPORT_CORE_EXT21;
        break;
    }

    return __wpadPushCommand(queue, cmd);
}

BOOL WPADiSendStreamData(WPADCmdQueue* queue, void* p_buf, u16 len) {
    WPADCommand cmd;
    u8 length = len << 3;

    cmd.command = WPAD_HIDREP_STRM;
    cmd.len = 21;
    cmd.data[0] = length;
    cmd.callback = NULL;
    memcpy(&cmd.data[1], p_buf, len);

    return __wpadPushCommand(queue, cmd);
}

BOOL WPADiSendMuteSpeaker(WPADCmdQueue* queue, BOOL mute, WPADCallback callback) {
    WPADCommand cmd;

    cmd.command = WPAD_HIDREP_MUTE;
    cmd.len = 1;
    cmd.data[0] = mute ? 4 : 0;
    cmd.callback = callback;

    return __wpadPushCommand(queue, cmd);
}

BOOL WPADiSendDPDCSB(WPADCmdQueue* queue, BOOL enable, WPADCallback callback) {
    WPADCommand cmd;

    cmd.command = WPAD_HIDREP_DPDCSB;
    cmd.len = 1;
    cmd.data[0] = enable ? 4 : 0;
    cmd.callback = callback;

    return __wpadPushCommand(queue, cmd);
}

BOOL WPADiSendEnableDPD(WPADCmdQueue* queue, BOOL enable, WPADCallback callback) {
    WPADCommand cmd;

    cmd.command = WPAD_HIDREP_ENDPD;
    cmd.len = 1;
    cmd.data[0] = enable ? 4 : 0;
    cmd.callback = callback;

    return __wpadPushCommand(queue, cmd);
}

BOOL WPADiSendEnableSpeaker(WPADCmdQueue* queue, BOOL enable, WPADCallback callback) {
    WPADCommand cmd;

    cmd.command = WPAD_HIDREP_ENSPK;
    cmd.len = 1;
    cmd.data[0] = enable ? 4 : 0;
    cmd.callback = callback;

    return __wpadPushCommand(queue, cmd);
}

BOOL WPADiSendGetContStat(WPADCmdQueue* queue, WPADInfo* info, WPADCallback callback) {
    WPADCommand cmd;

    cmd.command = WPAD_HIDREP_GETSTAT;
    cmd.len = 1;
    cmd.data[0] = 0;
    cmd.callback = callback;
    cmd.info = info;

    return __wpadPushCommand(queue, cmd);
}

BOOL WPADiSendWriteDataCmd(WPADCmdQueue* queue, u8 cmd, u32 addr, WPADCallback callback) {
    return WPADiSendWriteData(queue, &cmd, sizeof(cmd), addr, callback);
}

BOOL WPADiSendWriteData(WPADCmdQueue* queue, void* p_buf, u16 len, u32 addr, WPADCallback callback) {
    WPADCommand cmd;
    u8 length = len & WPAD_WRITE_LEN_MASK;

    cmd.command = WPAD_HIDREP_WRDATA;
    cmd.len = 21;
    cmd.callback = callback;
    memcpy(&cmd.data[0], &addr, sizeof(addr));
    memcpy(&cmd.data[4], &length, sizeof(length));
    memcpy(&cmd.data[5], p_buf, len);

    return __wpadPushCommand(queue, cmd);
}

BOOL WPADiSendReadData(WPADCmdQueue* queue, void* p_buf, u16 len, u32 addr, WPADCallback callback) {
    WPADCommand cmd;

    cmd.command = WPAD_HIDREP_RDDATA;
    cmd.len = 6;
    cmd.callback = callback;
    memcpy(&cmd.data[0], &addr, sizeof(addr));
    memcpy(&cmd.data[4], &len, sizeof(len));
    cmd.readBuf = p_buf;
    cmd.readLen = len;
    cmd.readAddr = addr;

    return __wpadPushCommand(queue, cmd);
}

BOOL WPADiIsAvailableCmdQueue(WPADCmdQueue* queue, s8 num) {
    s8 size = __wpadGetQueueSize(queue);

    if ((u32)(size + num) <= queue->cmdlen - 1) {
        return TRUE;
    }

    return FALSE;
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

static BOOL __wpadPushCommand(WPADCmdQueue* queue, WPADCommand cmd) {
    BOOL enabled = OSDisableInterrupts();

    if (queue->cmdlen - 1 == __wpadGetQueueSize(queue)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    memset(&queue->cmd[queue->tail], 0, sizeof(WPADCommand));
    memcpy(&queue->cmd[queue->tail], &cmd, sizeof(WPADCommand));
    queue->tail = (u32)queue->tail == queue->cmdlen - 1 ? 0 : queue->tail + 1;
    OSRestoreInterrupts(enabled);
    return TRUE;
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
    u32 sense = (u8)SCGetBtDpdSensibility();

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

static u16 __wpadGetBTEBufferStatus(s32 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    s32 status = p_wpd->status;
    s8 handle = p_wpd->dev_handle;

    OSRestoreInterrupts(enabled);

    if (status == WPAD_ERR_NO_CONTROLLER) {
        return 0;
    }

    return _WUDGetQueuedSize(handle);
}

static u16 __wpadGetBTMBufferStatus(s32 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();
    s32 status = p_wpd->status;
    s8 handle = p_wpd->dev_handle;

    OSRestoreInterrupts(enabled);

    if (status == WPAD_ERR_NO_CONTROLLER) {
        return 0;
    }

    return _WUDGetNotAckedSize(handle);
}

void WPADRecalibrate(s32 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled = OSDisableInterrupts();

    p_wpd->isSetStickOrigin = FALSE;
    p_wpd->recalibrateCount = 0;
    OSRestoreInterrupts(enabled);
}

BOOL WPADIsUsedCallbackByKPAD(void) {
    return _wpadUsedCallback;
}

void WPADSetCallbackByKPAD(BOOL isKPAD) {
    _wpadUsedCallback = isKPAD;
}

u8 WPADiGetGameType(void) {
    return _wpadGameType;
}

const char* WPADiGetGameCode(void) {
    return _wpadGameCode;
}
