#ifndef WUDINTERNAL_H
#define WUDINTERNAL_H

#include "revolution/bte.h"
#include "revolution/os.h"
#include "revolution/sc.h"
#include "revolution/wud/wud.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WUD_BDCMP(LHS, RHS) memcmp(LHS, RHS, BD_ADDR_LEN)
#define WUD_BDCPY(DST, SRC) memcpy(DST, SRC, BD_ADDR_LEN)

#define WUD_DEV_NAME_IS(NAME, VALUE)                                           \
    (memcmp(NAME, VALUE, sizeof(VALUE) - 1) == 0)

#define WUD_DEV_NAME_IS_CNT(NAME)                                              \
    WUD_DEV_NAME_IS(NAME, "Nintendo RVL-CNT")

#define WUD_DEV_NAME_IS_CNT_01(NAME)                                           \
    WUD_DEV_NAME_IS(NAME, "Nintendo RVL-CNT-01")

typedef enum {
    WUD_STATE_SYNC_START = 0,
    WUD_STATE_SYNC_PREPARE_SEARCH = 1,
    WUD_STATE_SYNC_START_SEARCH = 2,
    WUD_STATE_SYNC_WAIT_FOR_SEARCH_RESULT = 3,
    WUD_STATE_SYNC_CHECK_SEARCH_RESULT = 4,
    WUD_STATE_SYNC_IS_EXISTED_DEVICE = 5,
    WUD_STATE_SYNC_6 = 6,
    WUD_STATE_SYNC_STORED_LINK_KEY_TO_EEPROM = 7,
    WUD_STATE_SYNC_WAIT_FOR_STORING = 8,
    WUD_STATE_SYNC_WAIT_FOR_READING = 10,
    WUD_STATE_SYNC_11 = 11,
    WUD_STATE_SYNC_12 = 12,
    WUD_STATE_SYNC_13 = 13,
    WUD_STATE_SYNC_DONE = 14,
    WUD_STATE_SYNC_TRY_CONNECT = 15,
    WUD_STATE_SYNC_PREPARE_FOR_EXISTED_DEVICE = 16,
    WUD_STATE_SYNC_PREPARE_FOR_UNKNOWN_DEVICE = 17,
    WUD_STATE_SYNC_REGISTER_DEVICE = 18,
    WUD_STATE_SYNC_VIRGIN_SIMPLE = 19,
    WUD_STATE_SYNC_VIRGIN_STANDARD = 20,
    WUD_STATE_SYNC_CHANGE_SIMPLE_TO_STANDARD = 21,
    WUD_STATE_SYNC_STORED_DEV_INFO_TO_NAND = 22,
    WUD_STATE_SYNC_COMPLETE = 23,
    WUD_STATE_SYNC_WAIT_FOR_INCOMING = 24,
    WUD_STATE_SYNC_SC_FLUSH = 25,
    WUD_STATE_SYNC_CANCEL_SEARCH = 26,
    WUD_STATE_SYNC_WAIT_FOR_START_SEARCH = 29,
    WUD_STATE_SYNC_ERROR = 255,
} WUDSyncState;

typedef enum {
    WUD_STATE_DELETE_START = 0,
    WUD_STATE_DELETE_DISALLOW_INCOMING = 1,
    WUD_STATE_DELETE_DISCONNECT_ALL = 2,
    WUD_STATE_DELETE_CLEANUP_DATABASE = 3,
    WUD_STATE_DELETE_CLEANUP_SETTING = 5,
    WUD_STATE_DELETE_6 = 6,
    WUD_STATE_DELETE_7 = 7,
    WUD_STATE_DELETE_DONE = 8
} WUDDeleteState;

typedef enum {
    WUD_RESULT_DELETE_BUSY = -1,
    WUD_RESULT_DELETE_WAITING,
    WUD_RESULT_DELETE_COMPLETE,
} WUDDeleteResult;

typedef enum {
    WUD_STATE_LINK_KEY_START = 0,
    WUD_STATE_LINK_KEY_READING = 1,
    WUD_STATE_LINK_KEY_WRITING = 2,
    WUD_STATE_LINK_KEY_DELETING = 3,
    WUD_STATE_LINK_KEY_ERROR = 255,
} WUDLinkKeyState;

typedef enum {
    WUD_STATE_STACK_START = 0,
    WUD_STATE_STACK_GET_STORED_LINK_KEY = 1,
    WUD_STATE_STACK_CHECK_DEVICE_INFO = 2,
    WUD_STATE_STACK_DONE = 3,
    WUD_STATE_STACK_INITIALIZED = 4,
    WUD_STATE_STACK_ERROR = 255,
} WUDStackState;

typedef enum {
    WUD_STATE_INIT_START = 0,
    WUD_STATE_INIT_WAIT_FOR_INITIALIZATION = 1,
    WUD_STATE_INIT_GET_DEV_INFO = 2,
    WUD_STATE_INIT_DONE = 3,
    WUD_STATE_INIT_INITIALIZED = 4,
    WUD_STATE_INIT_ERROR = 255,
} WUDInitState;

typedef enum {
    WUD_STATE_SHUTDOWN_START = 0,
    WUD_STATE_SHUTDOWN_STORE_SETTINGS = 1,
    WUD_STATE_SHUTDOWN_FLUSH_SETTINGS = 2,
    WUD_STATE_SHUTDOWN_DONE = 3,
    WUD_STATE_SHUTDOWN_ERROR = 255,
} WUDShutdownState;

typedef enum {
    WUD_VSE_INITIATE_PAIRING = 8,
    WUD_VSE_DELETE_ALL_KEYS,
    WUD_VSE_SI_PORT_STATUS,
    WUD_VSE_WATCH_DOG_RESET_HW = 16,
} WUDVendorSpecificEvent;

typedef struct WUDDevInfoList {
    WUDDevInfo* devInfo;
    struct WUDDevInfoList* prev;
    struct WUDDevInfoList* next;
} WUDDevInfoList;

typedef struct WUDDiscResp {
    BD_ADDR devAddr;
    char devName[64];
    u8 UNK_0x46[0xBA];
    tBTA_SERVICE_MASK services;
    u8 UNK_0x104[0x4];
} WUDDiscResp;

typedef struct WUDPatchCmd {
    u8 slot;
    u8 littleEndianAddress[4];
    u8 thumbCode[8];
} WUDPatchCmd;
typedef struct WUDPatchList {
    u8 num;
    WUDPatchCmd cmds[];
} WUDPatchList;
#define WUD_PATCH_BUFFER_SIZE 0xFF
#define WUD_MAX_PATCHES (WUD_PATCH_BUFFER_SIZE / (s32)sizeof(WUDPatchCmd))

typedef struct WUDCB {
    WUDSyncDeviceCallback syncStdCB;
    WUDSyncDeviceCallback syncSmpCB;
    WUDClearDeviceCallback clearDevCB;

    u8 syncState;
    u8 deleteState;
    u8 linkKeyState;
    u8 stackState;
    u8 initState;
    u8 shutdownState;

    u8 devNums;
    u8 devSmpNums;

    WUDDevInfoList* smpListHead;
    WUDDevInfoList* smpListTail;
    WUDDevInfoList smpList[WUD_MAX_DEV_ENTRY_FOR_SMP];

    WUDDevInfoList* stdListHead;
    WUDDevInfoList* stdListTail;
    WUDDevInfoList stdList[WUD_MAX_DEV_ENTRY_FOR_STD];

    WUDDevInfo stdDevs[WUD_MAX_DEV_ENTRY_FOR_STD];
    WUDDevInfo smpDevs[WUD_MAX_DEV_ENTRY_FOR_SMP];

    u8 connectedNum;
    u8 linkedNum;
    u8 syncedNum;

    u8 syncSkipChecks;
    s8 syncLoopNum;
    u8 syncType;

    u8 connectable;
    u8 discoverable;

    WUDHidRecvCallback hidRecvCB;
    WUDHidConnCallback hidConnCB;

    WUDAllocFunc allocFunc;
    WUDFreeFunc freeFunc;

    BD_ADDR pairAddr;
    BD_ADDR hostAddr;

    s8 libStatus;

    u8 serialPortStatus;
    UINT8 pmID;
    s8 syncRssi;
    OSAlarm alarm;
    u32 hhFlags;

    u16 bufferStatus0;
    u16 bufferStatus1;

    s16 delayCnt;
    s16 incomeCnt;
} WUDCB;

extern WUDCB _wcb;
extern WUDDevInfo _work;

extern SCBtDeviceInfoArray _scArray;

extern BD_ADDR_PTR _dev_handle_to_bda[WUD_MAX_DEV_ENTRY];
extern u16 _dev_handle_queue_size[WUD_MAX_DEV_ENTRY];
extern u16 _dev_handle_notack_num[WUD_MAX_DEV_ENTRY];

#ifdef __cplusplus
}
#endif

#endif
