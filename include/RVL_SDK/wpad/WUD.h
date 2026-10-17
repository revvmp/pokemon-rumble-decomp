#ifndef RVL_SDK_WPAD_WUD_H
#define RVL_SDK_WPAD_WUD_H

#include "revolution/types.h"
#include "revolution/wpad.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WUD_MAX_DEV_ENTRY 16
#define WUD_DEV_HANDLE_INVALID -1

#define BD_ADDR_LEN 6

typedef struct BT_HDR {
    u16 event;
    u16 len;
    u16 offset;
    u16 layer_specific;
} BT_HDR;

typedef struct WUDDevInfo WUDDevInfo;

typedef void (*WUDHidConnectCallback)(WUDDevInfo* info, u8 open);
typedef void (*WUDHidReceiveCallback)(u8 devHandle, u8* report, u16 len);

BOOL WUDInit(void);
s32 WUDGetStatus(void);
BOOL WUDIsBusy(void);
BOOL WUDCancelSyncDevice(void);
void WUDRegisterAllocator(WPADAlloc alloc, WPADFree free);
void WUDShutdown(BOOL saveSimpleDevs);
WUDHidReceiveCallback WUDSetHidRecvCallback(WUDHidReceiveCallback callback);
WUDHidConnectCallback WUDSetHidConnCallback(WUDHidConnectCallback callback);
void WUDSetDisableChannel(s8 afhChannel);
void WUDSetVisibility(u8 discoverable, u8 connectable);
void WUDUpdateSCSetting(void);
void WUDSetDeviceHistory(s32 chan, u8* addr);
u8* _WUDGetDevAddr(u8 devHandle);

BT_HDR* GKI_getbuf(u16 size);
void BTA_HhSendData(u8 devHandle, BT_HDR* buf);
void BTA_HhGetAclQueueInfo(void);
void BTA_DmSendHciReset(void);
void btm_remove_acl(u8* addr);

#ifdef __cplusplus
}
#endif

#endif
