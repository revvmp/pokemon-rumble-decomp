#include "revolution/wud.h"
#include <string.h>

#define DEBUGPrint WUD_DEBUGPrint

enum {
    WUD_INIT_FLUSH_COMPLETE = 5,
    WUD_INIT_READY = 6
};

static u8 _wudWiiRemoteDescriptor[] = {
    0x05, 0x01,
    0x09, 0x05,

    0xA1, 0x01,

    0x85, 0x10,
    0x15, 0x00,
    0x26, 0xFF, 0x00,
    0x75, 0x08,
    0x95, 0x01,
    0x06, 0x00, 0xFF,
    0x09, 0x01,
    0x91, 0x00,

    0x85, 0x11,
    0x95, 0x01,
    0x09, 0x01,
    0x91, 0x00,

    0x85, 0x12,
    0x95, 0x02,
    0x09, 0x01,
    0x91, 0x00,

    0x85, 0x13,
    0x95, 0x01,
    0x09, 0x01,
    0x91, 0x00,

    0x85, 0x14,
    0x95, 0x01,
    0x09, 0x01,
    0x91, 0x00,

    0x85, 0x15,
    0x95, 0x01,
    0x09, 0x01,
    0x91, 0x00,

    0x85, 0x16,
    0x95, 0x15,
    0x09, 0x01,
    0x91, 0x00,

    0x85, 0x17,
    0x95, 0x06,
    0x09, 0x01,
    0x91, 0x00,

    0x85, 0x18,
    0x95, 0x15,
    0x09, 0x01,
    0x91, 0x00,

    0x85, 0x19,
    0x95, 0x01,
    0x09, 0x01,
    0x91, 0x00,

    0x85, 0x1A,
    0x95, 0x01,
    0x09, 0x01,
    0x91, 0x00,

    0x85, 0x20,
    0x95, 0x06,
    0x09, 0x01,
    0x81, 0x00,

    0x85, 0x21,
    0x95, 0x15,
    0x09, 0x01,
    0x81, 0x00,

    0x85, 0x22,
    0x95, 0x04,
    0x09, 0x01,
    0x81, 0x00,

    0x85, 0x30,
    0x95, 0x02,
    0x09, 0x01,
    0x81, 0x00,

    0x85, 0x31,
    0x95, 0x05,
    0x09, 0x01,
    0x81, 0x00,

    0x85, 0x32,
    0x95, 0x0A,
    0x09, 0x01,
    0x81, 0x00,

    0x85, 0x33,
    0x95, 0x11,
    0x09, 0x01,
    0x81, 0x00,

    0x85, 0x34,
    0x95, 0x15,
    0x09, 0x01,
    0x81, 0x00,

    0x85, 0x35,
    0x95, 0x15,
    0x09, 0x01,
    0x81, 0x00,

    0x85, 0x36,
    0x95, 0x15,
    0x09, 0x01,
    0x81, 0x00,

    0x85, 0x37,
    0x95, 0x15,
    0x09, 0x01,
    0x81, 0x00,

    0x85, 0x3D,
    0x95, 0x15,
    0x09, 0x01,
    0x81, 0x00,

    0x85, 0x3E,
    0x95, 0x15,
    0x09, 0x01,
    0x81, 0x00,

    0x85, 0x3F,
    0x95, 0x15,
    0x09, 0x01,
    0x81, 0x00,

    0xC0,
};

static u8 _wudResetAuthCountCmd[] = {
    (0x00083630) & 0xFF, (0x00083630 >> 8) & 0xFF, (0x00083630 >> 16) & 0xFF, (0x00083630 >> 24) & 0xFF,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

WUDCB _wcb;
static WUDDiscResp sWudDiscoveryResponse;
static s8 sWudDiscoveryRssi;
static u8 sWudDiscoveryResultCount;
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

void __wudEnableStackHandler0(OSAlarm* pAlarm, OSContext* pContext);

static void __wudReverseAddr(BD_ADDR_PTR pDst, BD_ADDR src) {
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < BD_ADDR_LEN; i++) {
        pDst[i] = src[BD_ADDR_LEN - 1 - i];
    }

    OSRestoreInterrupts(enabled);
}

static int __wudSetPinCode(BD_ADDR addr) {
    BD_ADDR pin;
    BD_ADDR_PTR pAddr;

    pAddr = _wcb.syncType == WUD_SYNC_TYPE_STANDARD ? _wcb.hostAddr : addr;

    if (_work.sync_type == 4) {
        WUDiRemoveDevice(_work.devAddr);
        btm_remove_acl(_work.devAddr);
    }

    _work.pinCodeSubmitted = 1;

    __wudReverseAddr(pin, pAddr);
    BTA_DmPinReply(addr, TRUE, BD_ADDR_LEN, pin);

    return 0;
}

static void __wudEnableStack(void) {
    WUDCB* p = &_wcb;

    BTA_HhEnable(BTA_SEC_AUTHENTICATE, WUDHidHostCallback);

    p->stackState = WUD_STATE_STACK_GET_STORED_LINK_KEY;

    OSCreateAlarm(&p->alarm);
    OSSetPeriodicAlarm(&p->alarm, OSGetTime(), OSMillisecondsToTicks(10), __wudEnableStackHandler0);
}

static void __wudClearDiscoverResult(void) {
    sWudDiscoveryResultCount = 0;
    memset(&sWudDiscoveryResponse, 0, sizeof(WUDDiscResp));
}

static void __wudResetAuthFailCount(void) {
    BTM_VendorSpecificCommand(0xFC4C, sizeof(_wudResetAuthCountCmd), (u8*)_wudResetAuthCountCmd, NULL);
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

void WUDiRegisterDevice(BD_ADDR addr) {
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    tBTA_STATUS status;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    pInfo = WUDiGetDevInfo(addr);

    status = BTA_DmAddDevice(pInfo->devAddr, pInfo->linkKey, 0, FALSE);
    DEBUGPrint("BTA_DmAddDevice(): %d\n", status);

    if (WUD_DEV_NAME_IS_CNT(pInfo->conf.devName) || (WUD_DEV_NAME_IS(pInfo->conf.devName, "Nintendo RVL-WBC") && WUDIsLinkedWBC())) {
        tBTA_HH_DEV_DESCR desc;
        desc.dl_len = sizeof(_wudWiiRemoteDescriptor);
        desc.dsc_list = _wudWiiRemoteDescriptor;

        DEBUGPrint("BTA_HhAddDev()\n");
        BTA_HhAddDev(pInfo->devAddr, pInfo->hhAttrMask, pInfo->subclass, pInfo->appID, desc);
    }

    if (pInfo->sync_type == 0 || pInfo->sync_type == 4 || pInfo->sync_type == 2 || pInfo->sync_type == 5) {
        p->devNums++;
    } else {
        p->devSmpNums++;
    }

    OSRestoreInterrupts(enabled);
}

void WUDiRemoveDevice(BD_ADDR addr) {
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    tBTM_STATUS status;
    BOOL enabled = OSDisableInterrupts();

    DEBUGPrint("WUDiRemoveDevice : \n");

    pInfo = WUDiGetDevInfo(addr);

    if (pInfo != NULL) {
        DEBUGPrint(" handle : %d,  addr : %02x:%02x:%02x:%02x:%02x:%02x\n", pInfo->devHandle, pInfo->devAddr[0], pInfo->devAddr[1], pInfo->devAddr[2],
                   pInfo->devAddr[3], pInfo->devAddr[4], pInfo->devAddr[5]);

        DEBUGPrint("remove device info from database.\n");

        if (WUD_DEV_NAME_IS_CNT(pInfo->conf.devName) || (WUD_DEV_NAME_IS(pInfo->conf.devName, "Nintendo RVL-WBC") && WUDIsLinkedWBC())) {
            DEBUGPrint("BTA_HhRemoveDev()\n");
            DEBUGPrint(" handle : %d\n", pInfo->devHandle);
            BTA_HhRemoveDev(pInfo->devHandle);
        }

        status = BTA_DmRemoveDevice(pInfo->devAddr);
        DEBUGPrint("BTA_DmRemoveDevice(): %d\n", status);

        if (pInfo->sync_type == 0 || pInfo->sync_type == 2 || pInfo->sync_type == 4 || pInfo->sync_type == 5) {
            p->devNums--;
        } else {
            p->devSmpNums--;
        }

        memset(pInfo, 0, sizeof *pInfo);
    }

    OSRestoreInterrupts(enabled);
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

void __wudSecurityEventStackCallback(tBTA_DM_SEC_EVT event, tBTA_DM_SEC* pData) {
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    u8 i;

    tBTA_DM_ENABLE* pEnable;
    tBTA_DM_LINK_UP* pLinkUp;
    tBTA_DM_LINK_DOWN* pLinkDown;
    tBTA_DM_PIN_REQ* pPinReq;
    tBTA_DM_AUTH_CMPL* pAuthCmpl;

    DEBUGPrint("__wudCleanupStackCallback: ");

    switch (event) {
    case BTA_DM_ENABLE_EVT: {
        pEnable = &pData->enable;
        WUD_BDCPY(p->hostAddr, pEnable->bd_addr);

        DEBUGPrint("BTA_ENABLE_EVT\n");
        DEBUGPrint("host : %02x:%02x:%02x:%02x:%02x:%02x\n", p->hostAddr[0], p->hostAddr[1], p->hostAddr[2], p->hostAddr[3], p->hostAddr[4],
                   p->hostAddr[5]);

        __wudEnableStack();
        p->libStatus = WUD_LIB_STATUS_2;
        break;
    }

    case BTA_DM_DISABLE_EVT: {
        DEBUGPrint("BTA_DISABLE_EVT\n");

        BTA_CleanUp(__wudCleanupStackCallback);
        break;
    }

    case BTA_DM_PIN_REQ_EVT: {
        pPinReq = &pData->pin_req;

        DEBUGPrint("BTA_DM_PIN_REQ_EVT\n");

        __wudSetPinCode(pPinReq->bd_addr);
        break;
    }

    case BTA_DM_AUTH_CMPL_EVT: {
        pAuthCmpl = &pData->auth_cmpl;

        DEBUGPrint("BTA_DM_AUTH_CMPL_EVT\n");

        DEBUGPrint("  addr : %02x:%02x:%02x:%02x:%02x:%02x\n", pAuthCmpl->bd_addr[0], pAuthCmpl->bd_addr[1], pAuthCmpl->bd_addr[2],
                   pAuthCmpl->bd_addr[3], pAuthCmpl->bd_addr[4], pAuthCmpl->bd_addr[5]);

        DEBUGPrint("  key  : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
                   pAuthCmpl->key[0],  pAuthCmpl->key[1],  pAuthCmpl->key[2],  pAuthCmpl->key[3],
                   pAuthCmpl->key[4],  pAuthCmpl->key[5],  pAuthCmpl->key[6],  pAuthCmpl->key[7],
                   pAuthCmpl->key[8],  pAuthCmpl->key[9],  pAuthCmpl->key[10], pAuthCmpl->key[11],
                   pAuthCmpl->key[12], pAuthCmpl->key[13], pAuthCmpl->key[14], pAuthCmpl->key[15]);

        DEBUGPrint("  result = %d\n", pAuthCmpl->key_type);

        if (pAuthCmpl->key_type == HCI_LKEY_TYPE_COMBINATION) {
            break;
        }

        pInfo = WUDiGetDevInfo(pAuthCmpl->bd_addr);

        if (WUD_BDCMP(_work.devAddr, pAuthCmpl->bd_addr) == 0) {
            _work.status = 12;

            if (pInfo == NULL) {
                pInfo = &_work;
            }
        }

        memcpy(pInfo->linkKey, pAuthCmpl->key, LINK_KEY_LEN);
        break;
    }

    case BTA_DM_AUTHORIZE_EVT: {
        DEBUGPrint("BTA_DM_AUTHORIZE_EVT\n");
        break;
    }

    case BTA_DM_LINK_UP_EVT: {
        pLinkUp = &pData->link_up;

        DEBUGPrint("BTA_DM_LINK_UP_EVT\n");

        DEBUGPrint("   addr : %02x:%02x:%02x:%02x:%02x:%02x\n", pLinkUp->bd_addr[0], pLinkUp->bd_addr[1], pLinkUp->bd_addr[2], pLinkUp->bd_addr[3],
                   pLinkUp->bd_addr[4], pLinkUp->bd_addr[5]);

        pInfo = WUDiGetDevInfo(pLinkUp->bd_addr);
        if (pInfo == NULL && WUD_BDCMP(pLinkUp->bd_addr, _work.devAddr) == 0) {
            pInfo = &_work;
        }

        if (pInfo == NULL || _WUDGetLinkNumber() == WUD_MAX_CHANNELS) {
            btm_remove_acl(pLinkUp->bd_addr);

            DEBUGPrint("%s --> %02x:%02x:%02x:%02x:%02x:%02x\n", pInfo == NULL ? "not paired" : "4 links exist", pLinkUp->bd_addr[0],
                       pLinkUp->bd_addr[1], pLinkUp->bd_addr[2], pLinkUp->bd_addr[3], pLinkUp->bd_addr[4], pLinkUp->bd_addr[5]);
            break;
        }

        pInfo->status = pInfo->status == 2 ? 12 : 3;
        p->linkedNum++;
        break;
    }

    case BTA_DM_LINK_DOWN_EVT: {
        pLinkDown = &pData->link_down;

        DEBUGPrint("BTA_DM_LINK_DOWN_EVT\n");

        DEBUGPrint("   addr : %02x:%02x:%02x:%02x:%02x:%02x\n", pLinkDown->bd_addr[0], pLinkDown->bd_addr[1], pLinkDown->bd_addr[2],
                   pLinkDown->bd_addr[3], pLinkDown->bd_addr[4], pLinkDown->bd_addr[5]);

        DEBUGPrint("result: %d\n", pLinkDown->status);

        pInfo = WUDiGetDevInfo(pLinkDown->bd_addr);
        if (pInfo != NULL) {
            pInfo->status = 1;
            p->linkedNum--;

            if (WUD_BDCMP(_work.devAddr, pLinkDown->bd_addr) == 0) {
                p->syncState = WUD_STATE_SYNC_ERROR;
            }

            if (pLinkDown->status == HCI_ERR_PEER_POWER_OFF) {
                for (i = 0; i < WUD_MAX_CHANNELS; i++) {
                    if (WUD_BDCMP(_scArray.info[WUD_MAX_DEV_ENTRY_FOR_STD + i].bd_addr, pLinkDown->bd_addr) == 0) {
                        memset(&_scArray.info[WUD_MAX_DEV_ENTRY_FOR_STD + i], 0, sizeof(SCBtDeviceInfo));

                        sWudSettingsDirty = TRUE;
                    }
                }
            }
        } else if (WUD_BDCMP(_work.devAddr, pLinkDown->bd_addr) == 0) {
            p->syncState = WUD_STATE_SYNC_ERROR;
            p->linkedNum--;
        } else {
            DEBUGPrint("this device in not paired\n");
        }

        if (p->linkedNum <= 255 && p->linkedNum >= 250) {
            OSReport("WARNING: link num count is reset.\n");
            p->linkedNum = 0;
        }
        break;
    }

    case BTA_DM_SIG_STRENGTH_EVT: {
        DEBUGPrint("BTA_DM_SIG_STRENGTH_EVT\n");
        break;
    }

    case BTA_DM_BUSY_LEVEL_EVT: {
        DEBUGPrint("BTA_DM_BUSY_LEVEL_EVT\n");
        break;
    }
    }
}

void __wudSearchEventStackCallback(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH* pData) {
    s32 timeout;

    switch (event) {
    case BTA_DM_INQ_RES_EVT: {
        tBTA_DM_INQ_RES* pResp = &pData->inq_res;

        DEBUGPrint("INQUIRY RESULT: %02x:%02x:%02x:%02x:%02x:%02x   %02x%02x%02x   %d\n",
                   pResp->bd_addr[0], pResp->bd_addr[1], pResp->bd_addr[2],
                   pResp->bd_addr[3], pResp->bd_addr[4], pResp->bd_addr[5],
                   pResp->dev_class[0], pResp->dev_class[1], pResp->dev_class[2],
                   pResp->rssi);

        sWudDiscoveryRssi = pResp->rssi;

        if (_wcb.syncSkipChecks == TRUE || (_wcb.syncSkipChecks == FALSE && _WUDGetLinkNumber() < WUD_MAX_CHANNELS - 1)) {
            timeout = 6400;
        } else {
            timeout = 32768;
        }

        BTM_WritePageTimeout(timeout);
        break;
    }

    case BTA_DM_INQ_CMPL_EVT: {
        DEBUGPrint("INQUIRY_COMPLETED\n");
        break;
    }

    case BTA_DM_DISC_RES_EVT: {
        tBTA_DM_DISC_RES* dmDiscRes = &pData->disc_res;

        WUD_BDCPY(sWudDiscoveryResponse.devAddr, dmDiscRes->bd_addr);
        memcpy(sWudDiscoveryResponse.devName, dmDiscRes->bd_name, sizeof(sWudDiscoveryResponse.devName));

        sWudDiscoveryResponse.services = dmDiscRes->services;
        sWudDiscoveryResultCount++;

        DEBUGPrint("DISCOVER RESULT:  %02x:%02x:%02x:%02x:%02x:%02x   %s (%04x)\n",
                   sWudDiscoveryResponse.devAddr[0], sWudDiscoveryResponse.devAddr[1], sWudDiscoveryResponse.devAddr[2],
                   sWudDiscoveryResponse.devAddr[3], sWudDiscoveryResponse.devAddr[4], sWudDiscoveryResponse.devAddr[5],
                   sWudDiscoveryResponse.devName, sWudDiscoveryResponse.services);
        break;
    }

    case BTA_DM_DISC_CMPL_EVT: {
        DEBUGPrint("DISCOVER COMPLETED\n");
        _wcb.syncState = WUD_STATE_SYNC_CHECK_SEARCH_RESULT;
        break;
    }

    case BTA_DM_SEARCH_CANCEL_CMPL_EVT: {
        DEBUGPrint("SEARCH CANCEL\n\n");
        __wudResetAuthFailCount();
        __wudClearDiscoverResult();
        _wcb.syncState = WUD_STATE_SYNC_CHECK_SEARCH_RESULT;
        break;
    }

    default: {
        DEBUGPrint("Warning: Search Callback returns invalid event\n");
        break;
    }
    }
}

void __wudDeviceStatusEventStackCallback(tBTM_DEV_STATUS status) {
    DEBUGPrint("__wudDeviceStatusEventStackCallback\n");

    if (status == BTM_DEV_STATUS_CMD_TOUT) {
        OSReport("---- WARNING: USB FATAL ERROR! ----\n");
    }
}

void __wudPowerMangeEventStackCallback(BD_ADDR bd_addr, tBTM_PM_STATUS status, u16 value, u8 hci_status) {
    WUDCB* p_wcb = &_wcb;
    WUDDevInfo* p_info;

    WUD_DEBUGPrint("__wudPowerMangeEventStackCallback\n");
    WUD_DEBUGPrint("hci_status = %d\n", hci_status);

    p_info = WUDiGetDevInfo(bd_addr);
    if (p_info == NULL) {
        if (!memcmp(_work.devAddr, bd_addr, BD_ADDR_LEN)) {
            p_info = &_work;
        } else {
            WUD_DEBUGPrint("Unknown device is connected and changes the connection type!!!!\n");
            WUD_DEBUGPrint(" addr = %02x:%02x:%02x:%02x:%02x:%02x,  status = %d\n", bd_addr[0], bd_addr[1], bd_addr[2], bd_addr[3], bd_addr[4],
                           bd_addr[5], status);
            return;
        }
    }

    switch (status) {
    case BTM_PM_STS_ACTIVE:
        p_info->status = 8;
        break;
    case BTM_PM_STS_SNIFF:
        p_info->status = 9;
        break;
    default:

        break;
    }
    WUD_DEBUGPrint(" addr = %02x:%02x:%02x:%02x:%02x:%02x,  status = %d\n", p_info->devAddr[0], p_info->devAddr[1], p_info->devAddr[2],
                   p_info->devAddr[3], p_info->devAddr[4], p_info->devAddr[5], p_info->status);
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
