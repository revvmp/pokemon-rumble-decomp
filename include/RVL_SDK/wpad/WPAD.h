#ifndef RVL_SDK_WPAD_WPAD_H
#define RVL_SDK_WPAD_WPAD_H

#include "revolution/os.h"
#include "revolution/wpad.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WPAD_RX_DATASIZE 0x60
#define WPAD_RPT_DATASIZE 0x15
#define WPAD_COMMAND_CMD_MAX_LEN 24
#define WPAD_COMMAND_EXT_MAX_LEN 12
#define WPAD_MAX_NOISE_FILTERS 3

#define WPAD_NOISE_FILTER_ACC 0
#define WPAD_NOISE_FILTER_DPD 1
#define WPAD_NOISE_FILTER_EXT 2

#define WPAD_RPT_SET_RUMBLE 0x10
#define WPAD_RPT_SET_DATA_REPORT_MODE 0x12
#define WPAD_RPT_REQUEST_STATUS 0x15
#define WPAD_RPT_WRITE_DATA 0x16
#define WPAD_RPT_READ_DATA 0x17
#define WPAD_RPT_SPEAKER_DATA 0x18

#define WPAD_RPT_FLAG_RUMBLE 0x01
#define WPAD_RPT_FLAG_ACK 0x02

typedef struct WPADCommand {
    u32 command;
    u8 data[WPAD_RPT_DATASIZE];
    u16 len;
    void* readBuf;
    u16 readLen;
    u32 readAddr;
    WPADInfo* info;
    WPADCallback callback;
} WPADCommand;

typedef struct WPADCmdQueue {
    s8 head;
    s8 tail;
    WPADCommand* cmd;
    u32 cmdlen;
} WPADCmdQueue;

typedef struct WPADAcc {
    s16 x;
    s16 y;
    s16 z;
} WPADAcc;

typedef struct WPADConfig {
    DPDObject obj[WPAD_MAX_DPD_OBJECTS];
    WPADAcc acc_0g;
    WPADAcc acc_1g;
    u8 motor;
    u8 volume;
} WPADConfig;

typedef struct WPADStick {
    s16 x;
    s16 x_min;
    s16 x_max;
    s16 y;
    s16 y_min;
    s16 y_max;
} WPADStick;

typedef struct WPADFsConfig {
    WPADStick stick;
    WPADAcc acc_0g;
    WPADAcc acc_1g;
} WPADFsConfig;

typedef struct WPADClConfig {
    WPADStick lStick;
    WPADStick rStick;
    u8 triggerL;
    u8 triggerR;
} WPADClConfig;

typedef struct WPADExtConfig {
    union {
        WPADFsConfig fs;
        WPADClConfig cl;
    };
} WPADExtConfig;

typedef struct WPADMEMGameInfo {
    OSTime timestamp;
    u16 gameTitle[17];
    char gameCode[4];
    u8 gameType;
    u8 checksum;
    u8 reserved[8];
} WPADMEMGameInfo;

typedef struct WPADMEMControlBlock {
    BOOL used;
    u8* p_buf;
    u16 len;
    u32 addr;
    WPADCallback callback;
} WPADMEMControlBlock;

typedef struct WPADControlBlock {
    WPADMEMGameInfo gameInfo;
    s32 gameInfoErr[2];
    u8 prBuf[WPAD_RX_DATASIZE];
    u8 rxBuf[2][WPAD_RX_DATASIZE];
    WPADCmdQueue cmdq;
    WPADCommand cmds[WPAD_COMMAND_CMD_MAX_LEN];
    WPADCmdQueue extq;
    WPADCommand exts[WPAD_COMMAND_EXT_MAX_LEN];
    WPADInfo infos;
    WPADInfo* infoBuf;
    WPADConfig conf;
    WPADExtConfig extConf;
    WPADCallback resultCallback;
    WPADExtensionCallback extensionCallback;
    WPADConnectCallback connectCallback;
    WPADSamplingCallback sampleCallback;
    void* samplingBufs_ptr;
    u32 bufLatestIndex;
    u32 bufLength;
    u32 dataFormat;
    s32 status;
    u8 getStatFlag;
    u8 devType;
    u8 devMode;
    s8 dev_handle;
    WPADCallback reqVolCb;
    u8 rxBufIndex;
    u8 reqVolume;
    u8 dpdDummyObjSize;
    u8 dpdFormat;
    u8 dpdCommand;
    u8 audioStop;
    u8 audioStopCnt;
    u8 audioFrames;
    BOOL rumble;
    BOOL motor;
    BOOL used;
    BOOL setup;
    BOOL oldFw;
    OSThreadQueue threadq;
    OSTime lastUpdateTime;
    u16 diffCount[WPAD_MAX_NOISE_FILTERS];
    u16 hystCount[WPAD_MAX_NOISE_FILTERS];
    OSTime cmdTimer;
    u8 cmdTimeoutAction;
    u8 isSetStickOrigin;
    u16 recalibrateCount;
    u8 key[16];
    u8 ft[8];
    u8 sb[8];
    u8 readBuf[64];
    void* readBufPtr;
    u32 readBaseAddr;
    s32 readError;
    u16 readLength;
    u8 keyIdx;
    u8 radioSense;
    u16 packetCnt;
    u8 disconnect;
    u8 cmdId;
    WPADCallback infoCallback;
    u8 infoLocked;
    u8 extEnc;
    u8 pwrSave;
    u8 blcBattery;
    u8 savedDevType;
    u8 extDisconnected;
    s16 extReconnectWait;
    u8 dpdBusy;
    WPADMEMControlBlock memBlock;
} __attribute__((aligned(32))) WPADControlBlock;

extern WPADControlBlock* __rvl_p_wpadcb[WPAD_MAX_CONTROLLERS];

void DEBUGPrint(const char* fmt, ...);

void WPADiClearQueue(WPADCmdQueue* queue);
void WPADiClearMemBlock(s32 chan);

#ifdef __cplusplus
}
#endif

#endif
