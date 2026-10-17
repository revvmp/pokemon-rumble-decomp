#include "revolution/wud.h"
#include <string.h>

#define DEBUGPrint WUD_DEBUGPrint

enum {
    WUD_INIT_FLUSH_COMPLETE = 5,
    WUD_INIT_READY = 6
};

WUDCB _wcb;
WUDDevInfo _work;
SCBtDeviceInfoArray _scArray;
BD_ADDR_PTR _dev_handle_to_bda[WUD_MAX_DEV_ENTRY];
u16 _dev_handle_queue_size[WUD_MAX_DEV_ENTRY];
u16 _dev_handle_notack_num[WUD_MAX_DEV_ENTRY];
static u8 sWudSettingsDirty = FALSE;
static u8 sWudSyncTarget;
static BOOL sWudAbortSync;

void __wudSyncHandler0(OSAlarm* pAlarm, OSContext* pContext);

__declspec(weak) BOOL _linkedWBC = FALSE;
static BOOL sWudInitialized = FALSE;
static BOOL sWudReadNand = FALSE;

void WUDVendorSpecificCallback(UINT8 len, UINT8* pData);
void __wudDeviceStatusEventStackCallback(tBTM_DEV_STATUS status);
void __wudPowerMangeEventStackCallback(BD_ADDR addr, tBTM_PM_STATUS status, UINT16 value, UINT8 hciStatus);

static BOOL __wudStopSyncDevice(void)
{
	WUDCB *p_wcb = &_wcb;

	BOOL success = FALSE;
	BOOL intrStatus = OSDisableInterrupts();

	if (p_wcb->libStatus == WUD_LIB_STATUS_3)
	{
		if (p_wcb->syncLoopNum)
		{
			if (p_wcb->syncState == WUD_STATE_SYNC_WAIT_FOR_SEARCH_RESULT)
				BTA_DmSearchCancel();

			p_wcb->syncLoopNum = 0;
		}

		success = TRUE;
	}

	OSRestoreInterrupts(intrStatus);

	return success;
}

#pragma force_active on

BOOL WUDIsLinkedWBC(void) {
    return _linkedWBC;
}

void* App_MEMalloc(u32 size) {
    DEBUGPrint("App_MEMalloc\n");
    return _wcb.allocFunc(size);
}

u8 App_MEMfree(void* ptr) {
    WUDCB* p_wcb = &_wcb;
    u8 ret;

    WUD_DEBUGPrint("App_MEMfree\n");
    ret = p_wcb->freeFunc(ptr);

    return ret;
}

static void __wudSyncFlushCallback(u32 status) {
    WUDCB* p = &_wcb;

    DEBUGPrint("__wudSyncFlushCallback() : %d, Sync: %d\n", status, _wcb.syncState);

    if (p->syncState == WUD_STATE_SYNC_START) {
        return;
    }

    if (status == SC_STATUS_OK) {
        p->syncState = WUD_STATE_SYNC_COMPLETE;
    } else {
        p->syncState = WUD_STATE_SYNC_ERROR;
    }
}

static void __wudDeleteFlushCallback(u32 status) {
    WUDCB* p = &_wcb;

    DEBUGPrint("__wudDeleteFlushCallback() : %d, Delete: %d\n", status, _wcb.deleteState);

    if (p->deleteState == WUD_STATE_DELETE_START) {
        return;
    }

    if (status == SC_STATUS_OK) {
        p->deleteState = WUD_STATE_DELETE_DONE;
    } else {
        p->deleteState = WUD_STATE_DELETE_DONE;
    }
}

static WUDDeleteState WUDiTerminateDevice(void) {
    int i;
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++) {
        pInfo = &p->stdDevs[i];

        if (pInfo->status > 1) {
            btm_remove_acl(pInfo->devAddr);
        }
    }

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++) {
        pInfo = &p->smpDevs[i];

        if (pInfo->status > 1) {
            btm_remove_acl(pInfo->devAddr);
        }
    }

    return WUD_STATE_DELETE_CLEANUP_DATABASE;
}

static void __wudInitFlushCallback(u32 result) {
    WUDCB* p_wcb = &_wcb;

    WUD_DEBUGPrint("__wudInitFlushCallback() : %d, Init: %d\n", result, p_wcb->initState);

    p_wcb->initState = WUD_INIT_FLUSH_COMPLETE;
}

static void __wudShutdownFlushCallback(u32 result) {
    WUDCB* p_wcb = &_wcb;

    WUD_DEBUGPrint("__wudShutdownFlushCallback() : %d, Shutdown: %d\n", result, p_wcb->shutdownState);

    p_wcb->shutdownState = 3;
}

void WUDRegisterAllocator(WUDAllocFunc pAllocFunc, WUDFreeFunc pFreeFunc) {
    WUDCB* p = &_wcb;
    BOOL enabled;

    enabled = OSDisableInterrupts();

    p->allocFunc = pAllocFunc;
    p->freeFunc = pFreeFunc;

    OSRestoreInterrupts(enabled);
}

WUDLibStatus WUDGetStatus(void) {
    WUDCB* p = &_wcb;
    BOOL enabled = OSDisableInterrupts();

    s8 libStatus = p->libStatus;

    OSRestoreInterrupts(enabled);
    return libStatus;
}

u8 WUDGetBufferStatus(void) {
    WUDCB* p = &_wcb;
    u8 status;
    BOOL enabled = OSDisableInterrupts();

    status = p->bufferStatus1 - p->bufferStatus0;

    OSRestoreInterrupts(enabled);
    return status;
}

void WUDSetSniffMode(BD_ADDR addr, s32 interval) {
    WUDCB* p = &_wcb;
    tBTM_PM_PWR_MD block;

    if (interval > 0) {
        block.mode = BTM_PM_MD_SNIFF;
    } else {
        block.mode = BTM_PM_MD_ACTIVE;
    }

    block.max = interval;
    block.min = interval;
    block.attempt = 1;
    block.timeout = 0;

    BTM_SetPowerMode(p->pmID, addr, &block);
}

WUDSyncDeviceCallback WUDSetSyncSimpleCallback(WUDSyncDeviceCallback pCallback) {
    WUDCB* p = &_wcb;
    BOOL enabled;
    WUDSyncDeviceCallback pOldCallback;

    DEBUGPrint("WUDSetSyncDeviceCallback\n");

    enabled = OSDisableInterrupts();

    pOldCallback = p->syncSmpCB;
    p->syncSmpCB = pCallback;

    OSRestoreInterrupts(enabled);
    return pOldCallback;
}

static BOOL __wudStartSyncDevice(u8 syncType, s8 syncLoopNum, u8 target, BOOL fast) {
    WUDCB* p = &_wcb;
    BOOL success = FALSE;
    BOOL enabled;
    u32 libStatus;

    enabled = OSDisableInterrupts();
    libStatus = p->libStatus;
    OSRestoreInterrupts(enabled);

    if (libStatus == WUD_LIB_STATUS_3 && !WUDIsBusy()) {
        enabled = OSDisableInterrupts();
        sWudSyncTarget = target;
        p->syncState = WUD_STATE_SYNC_PREPARE_SEARCH;
        p->syncLoopNum = syncLoopNum;
        p->syncType = syncType;
        p->syncSkipChecks = fast ? TRUE : FALSE;
        p->syncedNum = 0;
        p->delayCnt = 50;
        p->incomeCnt = 200;

        OSCreateAlarm(&p->alarm);
        OSSetPeriodicAlarm(&p->alarm, OSGetTime(), OSMillisecondsToTicks(20), __wudSyncHandler0);

        OSRestoreInterrupts(enabled);

        success = TRUE;
    }

    return success;
}

BOOL WUDStartFastSyncSimple(void) {
    return __wudStartSyncDevice(WUD_SYNC_TYPE_SIMPLE, -1, 0, TRUE);
}

BOOL WUDCancelSyncDevice(void)
{
	sWudAbortSync = TRUE;

	return __wudStopSyncDevice();
}

BOOL WUDStopSyncSimple(void)
{
	return __wudStopSyncDevice();
}

BOOL WUDSetDisableChannel(s8 afhChannel) {
    int first = 0;
    int last = 0;
    BOOL success = FALSE;
    WUDCB* p = &_wcb;
    BOOL enabled;
    u32 libStatus;
    tBTM_STATUS status;

    DEBUGPrint("WUDSetDisableChannel()\n");

    if ((UINT8)afhChannel > 13) {
        return FALSE;
    }

    enabled = OSDisableInterrupts();
    libStatus = p->libStatus;
    OSRestoreInterrupts(enabled);

    if (libStatus == WUD_LIB_STATUS_3) {
        if (!afhChannel) {
            first = 0xFF;
            last = 0xFF;
        } else {
            first = (afhChannel + 1) * 5 - 14;
            last = (afhChannel + 1) * 5 + 14;

            if (first < 0) {
                first = 0;
            }

            if (last > 78) {
                last = 78;
            }
        }

        status = BTM_SetAfhChannels(first, last);
        DEBUGPrint("BTM_SetAfhChannels() : %d\n", status);

        success = TRUE;
    }

    return success;
}

WUDHidRecvCallback WUDSetHidRecvCallback(WUDHidRecvCallback pCallback) {
    WUDCB* p = &_wcb;
    BOOL enabled;
    WUDHidRecvCallback pOldCallback;

    DEBUGPrint("WUDSetHidRecvCallback()\n");

    enabled = OSDisableInterrupts();

    pOldCallback = p->hidRecvCB;
    p->hidRecvCB = pCallback;

    OSRestoreInterrupts(enabled);
    return pOldCallback;
}

WUDHidConnCallback WUDSetHidConnCallback(WUDHidConnCallback pCallback) {
    WUDCB* p = &_wcb;
    BOOL enabled;
    WUDHidConnCallback pOldCallback;

    DEBUGPrint("WUDSetHidConnCallback()\n");

    enabled = OSDisableInterrupts();

    pOldCallback = p->hidConnCB;
    p->hidConnCB = pCallback;

    OSRestoreInterrupts(enabled);
    return pOldCallback;
}

void WUDSetVisibility(u8 disc, u8 conn) {
    WUDCB* p = &_wcb;
    BOOL enabled = OSDisableInterrupts();

    p->discoverable = disc;
    p->connectable = conn;

    OSRestoreInterrupts(enabled);
    BTA_DmSetVisibility(disc, conn);
}

void __wudInitSub(void) {
    WUDCB* p_wcb = &_wcb;
    const char RVL_DEV_NAME[] = "Wii";
    const DEV_CLASS RVL_DEV_CLASS = {0x00, 0x04, 0x48};

    int i;
    BOOL enable;

    WUD_DEBUGPrint("start __wudInitSub()\n");

    BTA_DmSetDeviceName((char*)RVL_DEV_NAME);

    BTM_SetDeviceClass((u8*)RVL_DEV_CLASS);

    BTM_RegisterForVSEvents(&WUDVendorSpecificCallback);

    BTM_RegisterForDeviceStatusNotif(&__wudDeviceStatusEventStackCallback);

    BTM_PmRegister(BTM_PM_REG_SET | BTM_PM_REG_NOTIF, &p_wcb->pmID, &__wudPowerMangeEventStackCallback);

    BTM_WritePageTimeout(32768);

    BTM_SetDefaultLinkPolicy(HCI_ENABLE_SNIFF_MODE | HCI_ENABLE_MASTER_SLAVE_SWITCH);

    BTM_SetDefaultLinkSuperTout(3200);

    for (i = 0; i < 10; i++) {
        if (p_wcb->stdDevs[i].status == 1) {
            WUDiRegisterDevice(p_wcb->stdDevs[i].devAddr);
        }
    }

    for (i = 0; i < 6; i++) {
        if (p_wcb->smpDevs[i].status == 1) {
            WUDiRegisterDevice(p_wcb->smpDevs[i].devAddr);
        }
    }

    enable = OSDisableInterrupts();
    p_wcb->libStatus = 3;
    sWudReadNand = TRUE;
    OSRestoreInterrupts(enable);

    WUDSetVisibility(FALSE, TRUE);
}

WUDDevInfo* WUDiGetDevInfo(BD_ADDR addr) {
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    BOOL enabled;
    int i;

    pInfo = NULL;
    enabled = OSDisableInterrupts();

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++) {
        if (WUD_BDCMP(p->stdDevs[i].devAddr, addr) == 0) {
            pInfo = &p->stdDevs[i];
            break;
        }
    }

    if (pInfo == NULL) {
        for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++) {
            if (WUD_BDCMP(p->smpDevs[i].devAddr, addr) == 0) {
                pInfo = &p->smpDevs[i];
                break;
            }
        }
    }

    OSRestoreInterrupts(enabled);
    return pInfo;
}

void WUDiMoveTopSmpDevInfoPtr(WUDDevInfo* pInfo) {
    WUDCB* p = &_wcb;
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++) {
        if (WUD_BDCMP(p->smpList[i].devInfo->devAddr, pInfo->devAddr) != 0) {
            continue;
        }

        if (WUD_BDCMP(p->smpListHead->devInfo->devAddr, p->smpList[i].devInfo->devAddr) == 0) {
            break;
        }

        p->smpList[i].prev->next = p->smpList[i].next;

        if (WUD_BDCMP(p->smpListTail->devInfo->devAddr, p->smpList[i].devInfo->devAddr) == 0) {
            p->smpListTail = p->smpList[i].prev;
        } else {
            p->smpList[i].next->prev = p->smpList[i].prev;
        }

        p->smpList[i].next = p->smpListHead;
        p->smpListHead->prev = &p->smpList[i];
        p->smpListHead = &p->smpList[i];

        p->smpList[i].prev = NULL;

        break;
    }

    OSRestoreInterrupts(enabled);
}

void WUDiMoveBottomSmpDevInfoPtr(WUDDevInfo* pInfo) {
    WUDCB* p = &_wcb;
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++) {
        if (WUD_BDCMP(p->smpList[i].devInfo->devAddr, pInfo->devAddr) != 0) {
            continue;
        }

        if (WUD_BDCMP(p->smpListTail->devInfo->devAddr, p->smpList[i].devInfo->devAddr) == 0) {
            break;
        }

        p->smpList[i].next->prev = p->smpList[i].prev;

        if (WUD_BDCMP(p->smpListHead->devInfo->devAddr, p->smpList[i].devInfo->devAddr) == 0) {
            p->smpListHead = p->smpList[i].next;
        } else {
            p->smpList[i].prev->next = p->smpList[i].next;
        }

        p->smpList[i].prev = p->smpListTail;
        p->smpListTail->next = &p->smpList[i];
        p->smpListTail = &p->smpList[i];

        p->smpList[i].next = NULL;

        break;
    }

    OSRestoreInterrupts(enabled);
}

void WUDiMoveTopStdDevInfoPtr(WUDDevInfo* pInfo) {
    WUDCB* p = &_wcb;
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++) {
        if (WUD_BDCMP(p->stdList[i].devInfo->devAddr, pInfo->devAddr) != 0) {
            continue;
        }

        if (WUD_BDCMP(p->stdListHead->devInfo->devAddr, p->stdList[i].devInfo->devAddr) == 0) {
            break;
        }

        p->stdList[i].prev->next = p->stdList[i].next;

        if (WUD_BDCMP(p->stdListTail->devInfo->devAddr, p->stdList[i].devInfo->devAddr) == 0) {
            p->stdListTail = p->stdList[i].prev;
        } else {
            p->stdList[i].next->prev = p->stdList[i].prev;
        }

        p->stdList[i].next = p->stdListHead;
        p->stdListHead->prev = &p->stdList[i];
        p->stdListHead = &p->stdList[i];

        p->stdList[i].prev = NULL;

        break;
    }

    OSRestoreInterrupts(enabled);
}

void WUDiMoveBottomStdDevInfoPtr(WUDDevInfo* pInfo) {
    WUDCB* p = &_wcb;
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++) {
        if (WUD_BDCMP(p->stdList[i].devInfo->devAddr, pInfo->devAddr) != 0) {
            continue;
        }

        if (WUD_BDCMP(p->stdListTail->devInfo->devAddr, p->stdList[i].devInfo->devAddr) == 0) {
            break;
        }

        p->stdList[i].next->prev = p->stdList[i].prev;

        if (WUD_BDCMP(p->stdListHead->devInfo->devAddr, p->stdList[i].devInfo->devAddr) == 0) {
            p->stdListHead = p->stdList[i].next;
        } else {
            p->stdList[i].prev->next = p->stdList[i].next;
        }

        p->stdList[i].prev = p->stdListTail;
        p->stdListTail->next = &p->stdList[i];
        p->stdListTail = &p->stdList[i];

        p->stdList[i].next = NULL;

        break;
    }

    OSRestoreInterrupts(enabled);
}

BOOL WUDIsBusy(void) {
    WUDCB* p = &_wcb;
    BOOL enabled;

    enabled = OSDisableInterrupts();

    if (p->syncState == WUD_STATE_SYNC_START && p->deleteState == WUD_STATE_DELETE_START && p->stackState == WUD_STATE_STACK_INITIALIZED &&
        p->initState == WUD_INIT_READY) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}

static void __wudCleanupStackCallback(tBTA_STATUS status) {
    WUDCB* p = &_wcb;

    if (status == BTA_SUCCESS) {
        sWudInitialized = FALSE;
        p->libStatus = WUD_LIB_STATUS_0;
        return;
    }

    DEBUGPrint("WARNING: USB_CLOSE_FAILURE!\n");
}

void __wudDeviceStatusEventStackCallback(tBTM_DEV_STATUS status) {
    DEBUGPrint("__wudDeviceStatusEventStackCallback\n");

    if (status == BTM_DEV_STATUS_CMD_TOUT) {
        OSReport("---- WARNING: USB FATAL ERROR! ----\n");
    }
}

BD_ADDR_PTR _WUDGetDevAddr(UINT8 handle) {
    BD_ADDR_PTR pAddr;
    BOOL enabled = OSDisableInterrupts();

    if (handle < WUD_MAX_DEV_ENTRY) {
        pAddr = _dev_handle_to_bda[handle];

    } else {
        pAddr = NULL;
    }

    OSRestoreInterrupts(enabled);
    return pAddr;
}

u16 _WUDGetQueuedSize(s8 handle) {
    u16 queuedSize;
    BOOL enabled = OSDisableInterrupts();

    if ((UINT8)handle <= WUD_MAX_DEV_ENTRY - 1) {
        queuedSize = _dev_handle_queue_size[(UINT8)handle];

    } else {
        queuedSize = 0;
    }

    OSRestoreInterrupts(enabled);
    return queuedSize;
}

u16 _WUDGetNotAckedSize(s8 handle) {
    u16 notAckedSize;
    BOOL enabled = OSDisableInterrupts();

    if ((UINT8)handle <= WUD_MAX_DEV_ENTRY - 1) {
        notAckedSize = _dev_handle_notack_num[(UINT8)handle];
    } else {
        notAckedSize = 0;
    }

    OSRestoreInterrupts(enabled);
    return notAckedSize;
}

u8 _WUDGetLinkNumber(void) {
    WUDCB* p = &_wcb;
    BOOL enabled = OSDisableInterrupts();

    u8 num = p->linkedNum;

    OSRestoreInterrupts(enabled);
    return num;
}

WUDDevInfo *WUDiGetDiscoverDevice(void)
{
	return &_work;
}

void WUDSetDeviceHistory(s32 chan, BD_ADDR dev_addr)
{
	if (!dev_addr)
	{
		memset(&_scArray.info[10 + chan], 0,
		       sizeof _scArray.info[10 + chan]);
	}
	else
	{
		WUD_BDCPY(_scArray.info[10 + chan].bd_addr, dev_addr);
	}

	sWudSettingsDirty = TRUE;
}

BOOL WUDIsLatestDevice(s32 chan, BD_ADDR devAddr)
{
	if (!devAddr)
		return FALSE;

	if (WUD_BDCMP(_scArray.info[10 + chan].bd_addr, devAddr) == 0)
		return TRUE;
	else
		return FALSE;
}

void WUDUpdateSCSetting(void)
{
	if (!sWudSettingsDirty)
		return;

	if (SCSetBtDeviceInfoArray(&_scArray))
	{
		SCFlushAsync(NULL);
		sWudSettingsDirty = FALSE;
	}
}

void WUDiSetDevAddrForHandle(UINT8 dev_handle, BD_ADDR dev_addr)
{
	_dev_handle_to_bda[dev_handle] = dev_addr;
}

BD_ADDR_PTR WUDiGetDevAddrForHandle(UINT8 dev_handle)
{
	return _dev_handle_to_bda[dev_handle];
}

void WUDiSetQueueSizeForHandle(UINT8 dev_handle, u16 size)
{
	_dev_handle_queue_size[dev_handle] = size;
}

void WUDiSetNotAckNumForHandle(UINT8 dev_handle, u16 count)
{
	_dev_handle_notack_num[dev_handle] = count;
}

#pragma force_active off
