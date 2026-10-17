#ifndef REVOLUTION_WPAD_H
#define REVOLUTION_WPAD_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WPAD_MAX_CONTROLLERS 4
#define WPAD_MAX_DPD_OBJECTS 4

#define WPAD_MIN_DPD_SENS 1
#define WPAD_MAX_DPD_SENS 5

#define WPAD_ERR_NONE 0
#define WPAD_ERR_NO_CONTROLLER -1
#define WPAD_ERR_BUSY -2
#define WPAD_ERR_TRANSFER -3
#define WPAD_ERR_INVALID -4
#define WPAD_ERR_CORRUPTED -7

#define WPAD_STATUS_NONE 0
#define WPAD_STATUS_SETUP 1
#define WPAD_STATUS_INITIALIZED 2
#define WPAD_STATUS_ENABLED 3
#define WPAD_STATUS_DISABLED 4

#define WPAD_DEV_CORE 0
#define WPAD_DEV_FREESTYLE 1
#define WPAD_DEV_CLASSIC 2
#define WPAD_DEV_BALANCE_CHECKER 3
#define WPAD_DEV_VSM 4
#define WPAD_DEV_TRAIN 16
#define WPAD_DEV_GUITAR 17
#define WPAD_DEV_DRUM 18
#define WPAD_DEV_FUTURE 251
#define WPAD_DEV_NOT_SUPPORTED 252
#define WPAD_DEV_NOT_FOUND 253
#define WPAD_DEV_UNKNOWN 255

#define WPAD_DEV_MODE_NORMAL 0
#define WPAD_DEV_MODE_CLASSIC_REDUCED 1
#define WPAD_DEV_MODE_CLASSIC_EXTENDED 2
#define WPAD_DEV_MODE_CLASSIC_STANDARD 3

#define WPAD_FMT_CORE 0
#define WPAD_FMT_CORE_ACC 1
#define WPAD_FMT_CORE_ACC_DPD 2
#define WPAD_FMT_FREESTYLE 3
#define WPAD_FMT_FREESTYLE_ACC 4
#define WPAD_FMT_FREESTYLE_ACC_DPD 5
#define WPAD_FMT_CLASSIC 6
#define WPAD_FMT_CLASSIC_ACC 7
#define WPAD_FMT_CLASSIC_ACC_DPD 8
#define WPAD_FMT_CORE_ACC_DPD_FULL 9
#define WPAD_FMT_TRAIN 10
#define WPAD_FMT_GUITAR 11
#define WPAD_FMT_BALANCE_CHECKER 12
#define WPAD_FMT_VSM 13
#define WPAD_FMT_VSM_EXTENDED 14
#define WPAD_FMT_DRUM 15

#define WPAD_BUTTON_LEFT 0x0001
#define WPAD_BUTTON_RIGHT 0x0002
#define WPAD_BUTTON_DOWN 0x0004
#define WPAD_BUTTON_UP 0x0008
#define WPAD_BUTTON_PLUS 0x0010
#define WPAD_BUTTON_2 0x0100
#define WPAD_BUTTON_1 0x0200
#define WPAD_BUTTON_B 0x0400
#define WPAD_BUTTON_A 0x0800
#define WPAD_BUTTON_MINUS 0x1000
#define WPAD_BUTTON_Z 0x2000
#define WPAD_BUTTON_C 0x4000
#define WPAD_BUTTON_HOME 0x8000

#define WPAD_BUTTON_CL_UP 0x0001
#define WPAD_BUTTON_CL_LEFT 0x0002
#define WPAD_BUTTON_CL_ZR 0x0004
#define WPAD_BUTTON_CL_X 0x0008
#define WPAD_BUTTON_CL_A 0x0010
#define WPAD_BUTTON_CL_Y 0x0020
#define WPAD_BUTTON_CL_B 0x0040
#define WPAD_BUTTON_CL_ZL 0x0080
#define WPAD_BUTTON_CL_FULL_R 0x0200
#define WPAD_BUTTON_CL_PLUS 0x0400
#define WPAD_BUTTON_CL_HOME 0x0800
#define WPAD_BUTTON_CL_MINUS 0x1000
#define WPAD_BUTTON_CL_FULL_L 0x2000
#define WPAD_BUTTON_CL_DOWN 0x4000
#define WPAD_BUTTON_CL_RIGHT 0x8000

typedef void* (*WPADAlloc)(u32 size);
typedef u8 (*WPADFree)(void* ptr);
typedef void (*WPADInitFunc)(void);
typedef void (*WPADCallback)(s32 chan, s32 result);
typedef void (*WPADExtensionCallback)(s32 chan, s32 devType);
typedef void (*WPADConnectCallback)(s32 chan, s32 reason);
typedef void (*WPADSamplingCallback)(s32 chan);

typedef struct DPDObject {
    s16 x;
    s16 y;
    u16 size;
    u8 traceId;
} DPDObject;

typedef struct WPADInfo {
    BOOL dpd;
    BOOL speaker;
    BOOL attach;
    BOOL lowBat;
    BOOL nearempty;
    u8 battery;
    u8 led;
    u8 protocol;
    u8 firmware;
} WPADInfo;

typedef struct WPADStatus {
    u16 button;
    s16 accX;
    s16 accY;
    s16 accZ;
    DPDObject obj[WPAD_MAX_DPD_OBJECTS];
    u8 dev;
    s8 err;
} WPADStatus;

typedef struct WPADFSStatus {
    u16 button;
    s16 accX;
    s16 accY;
    s16 accZ;
    DPDObject obj[WPAD_MAX_DPD_OBJECTS];
    u8 dev;
    s8 err;
    s16 fsAccX;
    s16 fsAccY;
    s16 fsAccZ;
    s8 fsStickX;
    s8 fsStickY;
} WPADFSStatus;

typedef struct WPADCLStatus {
    u16 button;
    s16 accX;
    s16 accY;
    s16 accZ;
    DPDObject obj[WPAD_MAX_DPD_OBJECTS];
    u8 dev;
    s8 err;
    u16 clButton;
    s16 clLStickX;
    s16 clLStickY;
    s16 clRStickX;
    s16 clRStickY;
    u8 clTriggerL;
    u8 clTriggerR;
} WPADCLStatus;

typedef struct WPADTRStatus {
    u16 button;
    s16 accX;
    s16 accY;
    s16 accZ;
    DPDObject obj[WPAD_MAX_DPD_OBJECTS];
    u8 dev;
    s8 err;
    u16 trButton;
    u8 brake;
    u8 mascon;
} WPADTRStatus;

typedef struct WPADBLStatus {
    u16 button;
    s16 accX;
    s16 accY;
    s16 accZ;
    DPDObject obj[WPAD_MAX_DPD_OBJECTS];
    u8 dev;
    s8 err;
    u16 press[4];
    s8 temp;
    u8 battery;
} WPADBLStatus;

typedef struct WPADVSStatus {
    u16 button;
    s16 accX;
    s16 accY;
    s16 accZ;
    DPDObject obj[WPAD_MAX_DPD_OBJECTS];
    u8 dev;
    s8 err;
    u16 samples[5];
    u8 sampleCount;
    u16 prevSamples[5];
    u8 prevSampleCount;
    u16 baseline;
    u16 level;
    u8 state;
} WPADVSStatus;

BOOL WPADIsEnabledVSM(void);
BOOL WPADIsEnabledTRN(void);
BOOL WPADIsEnabledGTR(void);
BOOL WPADIsEnabledDRM(void);

s32 WBCSetupCalibration(void);
s32 WBCGetCalibrationStatus(void);
s32 WBCGetBatteryLevel(u8 battery);
s32 WBCRead(WPADBLStatus* status, f64* weight, s32 size);
s32 WBCSetZEROPoint(f64* zero, s32 size);
s32 WBCGetTGCWeight(f64 weight, f64* result, WPADBLStatus* status);

void WPADInit(void);
s32 WPADGetStatus(void);
BOOL WPADCancelSyncDevice(void);
void WPADRegisterAllocator(WPADAlloc alloc, WPADFree free);
void WPADGetAddress(s32 chan, u8* addr);
void WPADRecalibrate(s32 chan);
s32 WPADGetInfo(s32 chan, WPADInfo* info);
s32 WPADGetInfoAsync(s32 chan, WPADInfo* info, WPADCallback callback);
s32 WPADControlLed(s32 chan, u8 flags, WPADCallback callback);

#ifdef __cplusplus
}
#endif

#endif
