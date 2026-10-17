#include "revolution/wud.h"
#include <string.h>
#include "private/wud/WUDWiiFit.h"

#define WUD_DEV_NAME_IS_WBC(NAME) WUD_DEV_NAME_IS(NAME, "Nintendo RVL-WBC")

#define DEBUGPrint WUD_DEBUGPrint
#define WUD_WII_FIT_FILE_PATH "/title/00010004/52464e4a/data/RPHealth.dat"

enum {
    WUD_INIT_FLUSH_COMPLETE = 5,
    WUD_INIT_READY = 6,
    WUD_STATE_WII_FIT_OPEN = 100,
    WUD_STATE_WII_FIT_SEEK = 101,
    WUD_STATE_WII_FIT_UPDATE = 102,
    WUD_STATE_WII_FIT_CLOSE = 103,
    WUD_WII_FIT_FILE_OFFSET = 0x4AF18,
    WUD_WII_FIT_WRITE_SIZE = 132,
    WUD_WII_FIT_REGION_JAPAN = 0
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

static WUDWiiFitPairingInfo sWudWiiFitPairingInfo ATTRIBUTE_ALIGN(32);
static u32 sWudWiiFitChecksumSeed;
static u8 sWudNandLocked;

void WUDiMoveTopOfUnusedStdDevice(WUDDevInfo* pInfo);

static NANDFileInfo sWudWiiFitFileInfo;
static NANDCommandBlock sWudWiiFitCommandBlock;
static u8 sWudHandlerStack[0x1000] ATTRIBUTE_ALIGN(32);

void __wudSearchEventStackCallback(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH* pData);
void __wudLinkKeyEventStackCallback(void* pData);
static void __wudSyncFlushCallback(u32 status);
static u8 __wudSyncVirginStandard(void);
static void __wudOpenWiiFitCallback(s32 result, NANDCommandBlock* pBlock);
static void __wudSeekWiiFitCallback(s32 result, NANDCommandBlock* pBlock);
static void __wudUpdateWiiFitCallback(s32 result, NANDCommandBlock* pBlock);
static void __wudCloseWiiFitCallback(s32 result, NANDCommandBlock* pBlock);

static SCBtCmpDevInfoArray _spArray;
static void __wudDeleteFlushCallback(u32 status);
static void __wudShutdownFlushCallback(u32 result);
void __wudAppendRuntimePatch(void);
void __wudInitSub(void);

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

static WUDDevInfo* __wudGetDevInfoByIndex(int idx) {
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    BOOL enabled;

    enabled = OSDisableInterrupts();

    if ((u32)idx <= WUD_MAX_DEV_ENTRY_FOR_STD - 1) {
        pInfo = &p->stdDevs[idx];
    } else {
        pInfo = &p->smpDevs[idx - WUD_MAX_DEV_ENTRY_FOR_STD];
    }

    OSRestoreInterrupts(enabled);
    return pInfo;
}

static u8 __wudGetConnectionNumber(void) {
    WUDCB* p = &_wcb;
    BOOL enabled = OSDisableInterrupts();

    u8 num = p->connectedNum;

    OSRestoreInterrupts(enabled);
    return num;
}

static WUDDevInfo* __wudGetWbcDevice(void) {
    WUDCB* p_wcb = &_wcb;
    WUDDevInfo* p_info = NULL;
    WUDDevInfoList* ptr;
    BOOL enable;

    enable = OSDisableInterrupts();
    ptr = p_wcb->stdListHead;
    while (ptr) {
        if (!memcmp(ptr->devInfo->conf.devName, "Nintendo RVL-WBC", 16)) {
            p_info = ptr->devInfo;
        }
        ptr = ptr->next;
    }
    OSRestoreInterrupts(enable);

    return p_info;
}

static u8 __wudGetStdDevNumber(void) {
    WUDCB* p = &_wcb;
    BOOL enabled = OSDisableInterrupts();

    u8 num = p->devNums;

    OSRestoreInterrupts(enabled);
    return num;
}

static WUDDevInfo* __wudGetOldestStdDevice(void) {
    return _wcb.stdListTail->devInfo;
}

static inline WUDDevInfo* __wudGetNewStdDevInfo(void) {
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo = NULL;
    int i;
    for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++) {
        if (p->stdDevs[i].status == 0) {
            DEBUGPrint("new entry index is %d\n", i);
            pInfo = &p->stdDevs[i];
            break;
        }
    }
    return pInfo;
}

static inline WUDDevInfo* __wudGetNewDevInfo(void) {
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    BOOL enabled = OSDisableInterrupts();
    if (p->syncType == WUD_SYNC_TYPE_STANDARD) {
        pInfo = __wudGetNewStdDevInfo();
    } else {
        pInfo = p->smpListTail->devInfo;
    }
    OSRestoreInterrupts(enabled);
    return pInfo;
}

static u32 __wudCalcWiiFitCrc2(void *data_, u32 length_, u32 crc)
{
	u16 *data = data_;
	u32 crcHi = (u16)(crc >> 16);
	u16 crcLo = (u16)crc;

	u32 i;
	u32 length = length_ / sizeof *data;
	for (i = 0; i < length; ++i, ++data)
	{
		crcHi += *data;
		crcLo += ~*data;
	}

	return (u16)crcHi << 16 | crcLo;
}

static void __wudPrepareWBCDevInfo(BD_ADDR devAddr, LINK_KEY linkKey, void* pProfile) {
    u32 checksum;

    if (devAddr) {
        memcpy(sWudWiiFitPairingInfo.devAddr, devAddr, BD_ADDR_LEN);
    }
    if (linkKey) {
        memcpy(sWudWiiFitPairingInfo.linkKey, linkKey, LINK_KEY_LEN);
    }
    if (pProfile) {
        memcpy(&sWudWiiFitPairingInfo.devInfo, pProfile, sizeof(SCDevInfo));
    }

    checksum = __wudCalcWiiFitCrc2(&sWudWiiFitPairingInfo, 128, sWudWiiFitChecksumSeed);
    memcpy(&sWudWiiFitPairingInfo.checksum, &checksum, sizeof(checksum));
}

static BOOL __wudIsBusyScCmd(void) {
    return SCCheckStatus() == SC_STATUS_BUSY ? TRUE : FALSE;
}

static u8 __wudGetSmpDevNumber(void) {
    WUDCB* p = &_wcb;
    BOOL enabled = OSDisableInterrupts();

    u8 num = p->devSmpNums;

    OSRestoreInterrupts(enabled);
    return num;
}

static WUDDevInfo* __wudGetOldestSmpDevice(void) {
    return _wcb.smpListTail->devInfo;
}

static BOOL __wudIsBusyLinkKeyCmd(void)
{
	return _wcb.linkKeyState != WUD_STATE_LINK_KEY_START ? 1 : 0;
}

static u8 __wudSyncWaitForStartSearch(void) {
    WUDCB* p_wcb = &_wcb;

    if (p_wcb->syncLoopNum == 0) {
        return 14;
    }

    if ((_WUDGetLinkNumber() == 4) && (__wudGetConnectionNumber() == 4)) {
        return 14;
    }

    if (--p_wcb->delayCnt < 0) {
        return 2;
    }

    return 29;
}

static u8 __wudSyncStartSearch(void) {
    WUDCB* p = &_wcb;
    tBTA_DM_INQ dm_inq;

    dm_inq.mode = BTM_LIMITED_INQUIRY;
    dm_inq.max_resps = 1;
    dm_inq.report_dup = FALSE;

    if (p->syncSkipChecks) {
        dm_inq.duration = 5 - 2;
    } else if (p->syncType == WUD_SYNC_TYPE_STANDARD) {
        dm_inq.duration = _WUDGetLinkNumber() == WUD_MAX_CHANNELS - 1 ? 10 : 5;
        dm_inq.duration -= p->syncLoopNum;
    } else {
        dm_inq.duration = _WUDGetLinkNumber() == WUD_MAX_CHANNELS - 1 ? 10 - 2 : 5 - 2;
    }

    __wudClearDiscoverResult();
    BTA_DmSearch(&dm_inq, 0, &__wudSearchEventStackCallback);

    return WUD_STATE_SYNC_WAIT_FOR_SEARCH_RESULT;
}

static u8 __wudSyncWaitForSearchResult(void) {
    if (_WUDGetLinkNumber() == WUD_MAX_CHANNELS && __wudGetConnectionNumber() == WUD_MAX_CHANNELS) {
        DEBUGPrint("Cancel searching because 4 connections exist.\n");
        BTA_DmSearchCancel();

        return WUD_STATE_SYNC_CANCEL_SEARCH;
    }

    return WUD_STATE_SYNC_WAIT_FOR_SEARCH_RESULT;
}

static u8 __wudSyncWaitForIncoming(void) {
    u8 next = 24;

    if ((_WUDGetLinkNumber() == 4) && (__wudGetConnectionNumber() == 4)) {
        next = 14;
    }

    if (--_wcb.incomeCnt < 0) {
        next = 1;
    }

    return next;
}

static u8 __wudSyncIsExistedDevice(void) {
    WUDDevInfo* pInfo;
    WUDSyncState nextState = WUD_STATE_SYNC_PREPARE_FOR_UNKNOWN_DEVICE;

    pInfo = WUDiGetDevInfo(sWudDiscoveryResponse.devAddr);

    if (pInfo) {
        memcpy(&_work, pInfo, sizeof(WUDDevInfo));
        nextState = WUD_STATE_SYNC_PREPARE_FOR_EXISTED_DEVICE;
    }

    return nextState;
}

static u8 __wudSyncPrepareForExistedDevice(void) {
    WUDDevInfo* pWork = &_work;

    pWork->status = 1;

    switch (pWork->sync_type) {
    case 0:
    case 2:
    case 4:
    case 5: {
        if (_wcb.syncType == WUD_SYNC_TYPE_STANDARD) {
            WUDiMoveBottomStdDevInfoPtr(pWork);
            WUDiRemoveDevice(pWork->devAddr);
            return WUD_STATE_SYNC_PREPARE_FOR_UNKNOWN_DEVICE;
        } else {
            pWork->sync_type = 4;
        }

        break;
    }

    case 1:
    case 3: {
        WUDiMoveBottomSmpDevInfoPtr(pWork);
        WUDiRemoveDevice(_wcb.smpListTail->devInfo->devAddr);
        return WUD_STATE_SYNC_PREPARE_FOR_UNKNOWN_DEVICE;
    }
    }

    return WUD_STATE_SYNC_TRY_CONNECT;
}

static u8 __wudSyncPrepareForUnknownDevice(void) {
    WUDDevInfo* pWork = &_work;

    pWork->status = 1;
    pWork->sync_type = _wcb.syncType == WUD_SYNC_TYPE_STANDARD ? 0 : 1;

    WUD_BDCPY(pWork->devAddr, sWudDiscoveryResponse.devAddr);
    memcpy(&pWork->conf, sWudDiscoveryResponse.devName, sizeof(SCDevInfo));
    memset(pWork->linkKey, 0, LINK_KEY_LEN);

    return WUD_STATE_SYNC_TRY_CONNECT;
}

static u8 __wudSyncRegisterDevice(void) {
    WUDSyncState nextState;

    switch (_work.sync_type) {
    case 1: {
        nextState = WUD_STATE_SYNC_VIRGIN_SIMPLE;
        break;
    }

    case 0: {
        nextState = WUD_STATE_SYNC_VIRGIN_STANDARD;
        break;
    }

    case 5: {
        nextState = WUD_STATE_SYNC_CHANGE_SIMPLE_TO_STANDARD;
        break;
    }

    case 3: {
        nextState = WUD_STATE_SYNC_COMPLETE;
        break;
    }

    case 4: {
        nextState = WUD_STATE_SYNC_STORED_LINK_KEY_TO_EEPROM;
        break;
    }

    case 2: {
        nextState = WUD_STATE_SYNC_STORED_LINK_KEY_TO_EEPROM;
        break;
    }

    default: {
        break;
    }
    }

    return nextState;
}

static u8 __wudSyncVirginSimple(void) {
    WUDDevInfo* pInfo;

    if (__wudGetSmpDevNumber() == WUD_MAX_DEV_ENTRY_FOR_SMP) {
        pInfo = __wudGetOldestSmpDevice();
        WUDiRemoveDevice(pInfo->devAddr);
    }

    pInfo = __wudGetNewDevInfo();
    if (!pInfo) {
        return WUD_STATE_SYNC_ERROR;
    }
    if (pInfo->status != 0) {
        return WUD_STATE_SYNC_ERROR;
    }
    memcpy(pInfo, &_work, sizeof(WUDDevInfo));

    WUDiRegisterDevice(pInfo->devAddr);
    WUDiMoveTopSmpDevInfoPtr(pInfo);

    WUDiSetDevAddrForHandle(pInfo->devHandle, pInfo->devAddr);

    return WUD_STATE_SYNC_COMPLETE;
}

static u8 __wudSyncChangeSmp2Std(void) {
    WUDiMoveBottomSmpDevInfoPtr(&_work);
    WUDiRemoveDevice(_wcb.smpListTail->devInfo->devAddr);
    return __wudSyncVirginStandard();
}

static u8 __wudSyncStoredLinkKeyToE2prom(void) {
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;

    if (__wudIsBusyLinkKeyCmd()) {
        return WUD_STATE_SYNC_STORED_LINK_KEY_TO_EEPROM;
    }

    if (WUDIsLinkedWBC() && WUD_DEV_NAME_IS_WBC(_work.conf.devName)) {
        return WUD_STATE_SYNC_STORED_DEV_INFO_TO_NAND;
    }

    pInfo = WUDiGetDevInfo(_work.devAddr);

    DEBUGPrint("write stored link key\n");

    DEBUGPrint("addr : %02x:%02x:%02x:%02x:%02x:%02x\n",
               pInfo->devAddr[0], pInfo->devAddr[1], pInfo->devAddr[2],
               pInfo->devAddr[3], pInfo->devAddr[4], pInfo->devAddr[5]);

    DEBUGPrint("key  : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
               pInfo->linkKey[0],  pInfo->linkKey[1],  pInfo->linkKey[2],  pInfo->linkKey[3],
               pInfo->linkKey[4],  pInfo->linkKey[5],  pInfo->linkKey[6],  pInfo->linkKey[7],
               pInfo->linkKey[8],  pInfo->linkKey[9],  pInfo->linkKey[10], pInfo->linkKey[11],
               pInfo->linkKey[12], pInfo->linkKey[13], pInfo->linkKey[14], pInfo->linkKey[15]);

    p->linkKeyState = WUD_STATE_LINK_KEY_WRITING;

    BTM_WriteStoredLinkKey(1, &pInfo->devAddr, &pInfo->linkKey, __wudLinkKeyEventStackCallback);

    return WUD_STATE_SYNC_WAIT_FOR_STORING;
}

static u8 __wudSyncWaitForStoring(void) {
    return !__wudIsBusyLinkKeyCmd() ? WUD_STATE_SYNC_STORED_DEV_INFO_TO_NAND : WUD_STATE_SYNC_WAIT_FOR_STORING;
}

static u8 __wudSyncWaitForReading(void) {
    WUDSyncState nextState = WUD_STATE_SYNC_WAIT_FOR_READING;

    if (!__wudIsBusyLinkKeyCmd()) {
        nextState = WUD_STATE_SYNC_VIRGIN_STANDARD;
        WUDiRemoveDevice(_wcb.pairAddr);
    }

    return nextState;
}

static void __wudSyncScFlush(void) {
    WUDCB* p = &_wcb;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    p->syncState = WUD_STATE_SYNC_13;
    OSRestoreInterrupts(enabled);

    SCFlushAsync(__wudSyncFlushCallback);
}

static u8 __wudSyncComplete(void) {
    _wcb.syncedNum++;

    memset(&_work, 0, sizeof(WUDDevInfo));

    return _wcb.syncType == WUD_SYNC_TYPE_STANDARD ? WUD_STATE_SYNC_DONE : WUD_STATE_SYNC_PREPARE_SEARCH;
}

static u8 __wudSyncError(void) {
    memset(&_work, 0, sizeof(WUDDevInfo));

    return _wcb.syncType == WUD_SYNC_TYPE_STANDARD ? WUD_STATE_SYNC_DONE : WUD_STATE_SYNC_PREPARE_SEARCH;
}

static u8 __wudSyncCheckSearchResult(void)
{
	u8 nextState = WUD_STATE_SYNC_PREPARE_SEARCH;

	if (sWudDiscoveryResultCount > 0)
	{
		if (WUD_DEV_NAME_IS_CNT(sWudDiscoveryResponse.devName))
			nextState = WUD_STATE_SYNC_IS_EXISTED_DEVICE;

		if (WUDIsLinkedWBC() && WUD_DEV_NAME_IS_WBC(sWudDiscoveryResponse.devName))
			nextState = WUD_STATE_SYNC_IS_EXISTED_DEVICE;

		if (sWudDiscoveryRssi < _wcb.syncRssi)
			nextState = WUD_STATE_SYNC_PREPARE_SEARCH;
	}

	if (_wcb.syncType == WUD_SYNC_TYPE_STANDARD
	    && nextState == WUD_STATE_SYNC_PREPARE_SEARCH)
	{
		nextState = WUD_STATE_SYNC_WAIT_FOR_INCOMING;

		_wcb.incomeCnt = _WUDGetLinkNumber() == 3 ? 200 : 100;

		WUDSetVisibility(FALSE, TRUE);
	}

	return nextState;
}

static void __wudOpenWiiFitFile(void)
{
	const char *rpHealthFilePath = WUD_WII_FIT_FILE_PATH;

	if (sWudNandLocked)
		return;

	DEBUGPrint("WbcSaveData: %s\n", rpHealthFilePath);
	sWudNandLocked = TRUE;
	NANDOpenAsync(rpHealthFilePath, &sWudWiiFitFileInfo, NAND_ACCESS_WRITE,
	              &__wudOpenWiiFitCallback, &sWudWiiFitCommandBlock);
}

static void __wudSeekWiiFitFile(void)
{
	if (sWudNandLocked)
		return;

	sWudNandLocked = TRUE;
	NANDSeekAsync(&sWudWiiFitFileInfo, WUD_WII_FIT_FILE_OFFSET, NAND_SEEK_SET,
	              &__wudSeekWiiFitCallback, &sWudWiiFitCommandBlock);
}

static void __wudUpdateWiiFitFile(void)
{
	if (sWudNandLocked)
		return;

	DEBUGPrint("WBC to WiiFit\n");
    DEBUGPrint(" %02x %02x %02x %02x %02x %02x\n", sWudWiiFitPairingInfo.devAddr[0], sWudWiiFitPairingInfo.devAddr[1], sWudWiiFitPairingInfo.devAddr[2], sWudWiiFitPairingInfo.devAddr[3], sWudWiiFitPairingInfo.devAddr[4], sWudWiiFitPairingInfo.devAddr[5]);
    DEBUGPrint(" %02x %02x %02x %02x %02x %02x %02x %02x\n", sWudWiiFitPairingInfo.linkKey[0], sWudWiiFitPairingInfo.linkKey[1], sWudWiiFitPairingInfo.linkKey[2], sWudWiiFitPairingInfo.linkKey[3], sWudWiiFitPairingInfo.linkKey[4], sWudWiiFitPairingInfo.linkKey[5], sWudWiiFitPairingInfo.linkKey[6], sWudWiiFitPairingInfo.linkKey[7]);
    DEBUGPrint(" %02x %02x %02x %02x %02x %02x %02x %02x\n", sWudWiiFitPairingInfo.linkKey[8], sWudWiiFitPairingInfo.linkKey[9], sWudWiiFitPairingInfo.linkKey[10], sWudWiiFitPairingInfo.linkKey[11], sWudWiiFitPairingInfo.linkKey[12], sWudWiiFitPairingInfo.linkKey[13], sWudWiiFitPairingInfo.linkKey[14], sWudWiiFitPairingInfo.linkKey[15]);
    DEBUGPrint(" %s\n", sWudWiiFitPairingInfo.devInfo.devName);
    DEBUGPrint(" %02x%02x%02x%02x\n", ((u8*)&sWudWiiFitPairingInfo.checksum)[0], ((u8*)&sWudWiiFitPairingInfo.checksum)[1], ((u8*)&sWudWiiFitPairingInfo.checksum)[2], ((u8*)&sWudWiiFitPairingInfo.checksum)[3]);
    sWudNandLocked = TRUE;
	NANDWriteAsync(&sWudWiiFitFileInfo, &sWudWiiFitPairingInfo,
	               WUD_WII_FIT_WRITE_SIZE, &__wudUpdateWiiFitCallback,
	               &sWudWiiFitCommandBlock);
}

static void __wudCloseWiiFit(void)
{
	if (sWudNandLocked)
		return;

	sWudNandLocked = TRUE;
	NANDCloseAsync(&sWudWiiFitFileInfo, &__wudCloseWiiFitCallback,
	               &sWudWiiFitCommandBlock);
}

static inline u8 WUDGetConnectable(void) {
    WUDCB* p = &_wcb;
    u8 connectable;
    BOOL enabled = OSDisableInterrupts();

    connectable = p->connectable;

    OSRestoreInterrupts(enabled);
    return connectable;
}

static void __wudCheckFirmwareVersion(void) {
    tBTM_VERSION_INFO version;
    BTM_ReadLocalVersion(&version);

    DEBUGPrint("hci_version   : %02x\n", version.hci_version);
    DEBUGPrint("hci_revision  : %04x\n", version.hci_revision);

    DEBUGPrint("lmp_version   : %02x\n", version.lmp_version);
    DEBUGPrint("lmp_subversion: %04x\n", version.lmp_subversion);

    DEBUGPrint("manufacturer  : %04x\n", version.manufacturer);

    DEBUGPrint(" ==> 2045 firmware ver.002.003.014.%d\n", version.hci_revision & 0xFFF);

    if ((version.hci_revision & 0xFFF) == 167) {
        __wudAppendRuntimePatch();
    } else {
        __wudInitSub();
    }
}

static WUDDeleteState __wudDeleteDisallowIncoming(void) {
    BTA_DmSetVisibility(FALSE, FALSE);
    return WUD_STATE_DELETE_DISCONNECT_ALL;
}

static void __wudDeleteCleanupSetting(void) {
    WUDCB* p_wcb = &_wcb;

    BOOL result = FALSE;

    if (SCCheckStatus() == SC_STATUS_BUSY) {
        return;
    }

    memset(&_scArray, 0, sizeof(_scArray));
    memset(&_spArray, 0, sizeof(_spArray));

    result |= SCSetBtDeviceInfoArray(&_scArray);
    result |= SCSetBtCmpDevInfoArray(&_spArray);

    if (result) {
        p_wcb->deleteState = 6;
        SCFlushAsync(__wudDeleteFlushCallback);
    } else {
        p_wcb->deleteState = 8;
    }
    return;
}

static WUDDeleteState __wudDeleteDone(void) {
    WUDCB* p = &_wcb;

    WUDSetVisibility(FALSE, WUDGetConnectable());

    OSCancelAlarm(&p->alarm);
    p->deleteState = WUD_STATE_DELETE_START;

    if (p->clearDevCB) {
        p->clearDevCB(WUD_RESULT_DELETE_COMPLETE);
    }

    return WUD_STATE_DELETE_START;
}

static void __wudRemoveDevInfo(BD_ADDR addr) {
    WUDDevInfo* pInfo;
    BOOL enabled;

    pInfo = WUDiGetDevInfo(addr);
    if (!pInfo) {
        return;
    }

    enabled = OSDisableInterrupts();
    memset(pInfo, 0, sizeof(WUDDevInfo));
    OSRestoreInterrupts(enabled);
}

static WUDStackState __wudStackGetStoredLinkKey(void) {
    WUDCB* p = &_wcb;

    if (p->hhFlags == 1) {
        p->linkKeyState = WUD_STATE_LINK_KEY_READING;
        BTM_ReadStoredLinkKey(NULL, __wudLinkKeyEventStackCallback);
    }

    return WUD_STATE_STACK_CHECK_DEVICE_INFO;
}

static WUDStackState __wudStackDone(void) {
    WUDCB* p = &_wcb;

    if (p->linkKeyState == WUD_STATE_LINK_KEY_START) {
        OSCancelAlarm(&p->alarm);
        __wudCheckFirmwareVersion();
        return WUD_STATE_STACK_INITIALIZED;
    }

    return WUD_STATE_STACK_CHECK_DEVICE_INFO;
}

static void __wudShutdownStoreSetting(void) {
    BOOL result = sWudReadNand;

    if (SCCheckStatus() != SC_STATUS_BUSY) {
        result &= SCSetBtDeviceInfoArray(&_scArray);
        result &= SCSetBtCmpDevInfoArray(&_spArray);
        if (result) {
            _wcb.shutdownState = 2;
            SCFlushAsync(__wudShutdownFlushCallback);
        } else {
            _wcb.shutdownState = 3;
        }
    }
}

static void __wudShutdownDone(void) {
    OSCancelAlarm(&_wcb.alarm);
    BTA_DisableBluetooth();
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

static u8 __wudSyncPrepareSearch(void) {
    WUDCB* p = &_wcb;
    int i;
    WUDDevInfo* pInfo;

    WUDSetVisibility(FALSE, FALSE);

    if (p->syncLoopNum == 0) {
        return WUD_STATE_SYNC_DONE;
    }

    if (_WUDGetLinkNumber() == WUD_MAX_CHANNELS && __wudGetConnectionNumber() == WUD_MAX_CHANNELS) {
        return WUD_STATE_SYNC_DONE;
    }

    if (p->syncSkipChecks) {
        for (i = 0; i < WUD_MAX_DEV_ENTRY; i++) {
            pInfo = __wudGetDevInfoByIndex(i);

            if (pInfo->status == 9) {
                WUDSetSniffMode(pInfo->devAddr, 0);
                return WUD_STATE_SYNC_PREPARE_SEARCH;
            }
        }
    }

    if (p->syncLoopNum > 0) {
        p->syncLoopNum--;
    }

    p->delayCnt = 50;
    return WUD_STATE_SYNC_WAIT_FOR_START_SEARCH;
}

static u8 __wudSyncTryConnect(void) {
    WUDSyncState nextState = WUD_STATE_SYNC_ERROR;

    if (WUD_DEV_NAME_IS_CNT(sWudDiscoveryResponse.devName)) {
        _work.status = 2;
        BTA_HhOpen(_work.devAddr, BTA_HH_PROTO_RPT_MODE, BTA_SEC_AUTHENTICATE);
        nextState = WUD_STATE_SYNC_6;
    }

    if (WUDIsLinkedWBC() && WUD_DEV_NAME_IS(sWudDiscoveryResponse.devName, "Nintendo RVL-WBC")) {
        WUDDevInfo* pInfo = __wudGetWbcDevice();

        if (pInfo != NULL) {
            DEBUGPrint("Found the registered WBC in database\n");

            if (pInfo->status > 1) {
                return nextState;
            }

            if (WUD_BDCMP(sWudDiscoveryResponse.devAddr, pInfo->devAddr) != 0) {
                DEBUGPrint("Removed WBC data\n");
                WUDiMoveBottomStdDevInfoPtr(pInfo);
                WUDiRemoveDevice(pInfo->devAddr);
            }
        }

        _work.status = 2;
        BTA_HhOpen(_work.devAddr, BTA_HH_PROTO_RPT_MODE, BTA_SEC_AUTHENTICATE);
        nextState = WUD_STATE_SYNC_6;
    }

    return nextState;
}

static u8 __wudSyncVirginStandard(void)
{
	WUDCB *p_wcb = &_wcb;
	WUDDevInfo *p_info;

	if (WUDIsLinkedWBC() && WUD_DEV_NAME_IS_CNT(_work.conf.devName))
	{
		p_info = __wudGetWbcDevice();

		if (p_info && __wudGetStdDevNumber() == WUD_MAX_DEV_ENTRY_FOR_STD)
		{
			p_info = p_wcb->stdListTail->prev->devInfo;
			WUDiMoveBottomStdDevInfoPtr(p_info);
			WUDiRemoveDevice(p_info->devAddr);
		}
	}

	if (__wudGetStdDevNumber() == WUD_MAX_DEV_ENTRY_FOR_STD)
	{
		p_info = __wudGetOldestStdDevice();
		WUDiRemoveDevice(p_info->devAddr);
	}

	p_info = __wudGetNewDevInfo();

	if (!p_info)
		return WUD_STATE_SYNC_ERROR;

	if (p_info->status != 0)
		return WUD_STATE_SYNC_ERROR;

	memcpy(p_info, &_work, sizeof *p_info);
	WUDiRegisterDevice(p_info->devAddr);
	WUDiMoveTopStdDevInfoPtr(p_info);

	if (WUDIsLinkedWBC() && WUD_DEV_NAME_IS_WBC(p_info->conf.devName))
	{
		WUDiMoveTopOfUnusedStdDevice(p_info);

		memset(&_scArray.info[14], 0, sizeof _scArray.info[14]);
		WUD_BDCPY(&_scArray.info[14].bd_addr, p_info->devAddr);
		memcpy(&_scArray.info[14].bd_name, p_info, sizeof _scArray.info[14].bd_name);

		memcpy(&((WUDWiiFitConfigProfile*)_scArray.info[14].bd_name)->linkKey, p_info->linkKey, LINK_KEY_LEN);
		__wudPrepareWBCDevInfo(p_info->devAddr, p_info->linkKey, p_info);
	}

	WUDiSetDevAddrForHandle(p_info->devHandle, p_info->devAddr);

	return WUD_STATE_SYNC_STORED_LINK_KEY_TO_EEPROM;
}

static u8 __wudSyncStoredDevInfoToNand(void) {
    WUDSyncState nextState = WUD_STATE_SYNC_COMPLETE;
    u8 i;
    WUDDevInfoList* pIt;

    if (__wudIsBusyScCmd()) {
        return WUD_STATE_SYNC_STORED_DEV_INFO_TO_NAND;
    }

    memset(&_scArray.info, 0, sizeof(_scArray.info[0]) * WUD_MAX_DEV_ENTRY_FOR_STD);

    _scArray.num = __wudGetStdDevNumber();

    for (pIt = _wcb.stdListHead, i = 0; pIt; i++, pIt = pIt->next) {
        WUD_BDCPY(_scArray.info[i].bd_addr, pIt->devInfo->devAddr);

        memcpy(&_scArray.info[i].bd_name, &pIt->devInfo->conf, sizeof(SCDevInfo));
    }

    DEBUGPrint("%d devices is stored into SC.\n", i);

    if (!SCSetBtDeviceInfoArray(&_scArray)) {
        return WUD_STATE_SYNC_STORED_DEV_INFO_TO_NAND;
    }

    nextState = WUD_STATE_SYNC_SC_FLUSH;

    if (WUDIsLinkedWBC() && WUD_DEV_NAME_IS_WBC(_work.conf.devName) &&
        SCGetProductGameRegion() == WUD_WII_FIT_REGION_JAPAN) {
        nextState = WUD_STATE_WII_FIT_OPEN;
    }

    return nextState;
}

static void __wudOpenWiiFitCallback(s32 result, NANDCommandBlock* pBlock) {
	if (_wcb.syncState != WUD_STATE_SYNC_START)
	{
		_wcb.syncState = result == NAND_RESULT_OK
		                          ? WUD_STATE_WII_FIT_SEEK
		                          : WUD_STATE_SYNC_SC_FLUSH;
	}

	if (_wcb.deleteState != WUD_STATE_DELETE_START)
	{
		_wcb.deleteState = result == NAND_RESULT_OK
		                            ? WUD_STATE_WII_FIT_SEEK
		                            : WUD_STATE_DELETE_CLEANUP_SETTING;
	}

	sWudNandLocked = FALSE;
	DEBUGPrint("NANDOpen. [%d]\n", result);
}

static void __wudSeekWiiFitCallback(s32 result, NANDCommandBlock* pBlock) {
	if (_wcb.syncState != WUD_STATE_SYNC_START)
	{
		_wcb.syncState = result == WUD_WII_FIT_FILE_OFFSET
		                          ? WUD_STATE_WII_FIT_UPDATE
		                          : WUD_STATE_SYNC_SC_FLUSH;
	}

	if (_wcb.deleteState != WUD_STATE_DELETE_START)
	{
		_wcb.deleteState = result == WUD_WII_FIT_FILE_OFFSET
		                            ? WUD_STATE_WII_FIT_UPDATE
		                            : WUD_STATE_DELETE_CLEANUP_SETTING;
	}

	sWudNandLocked = FALSE;
	DEBUGPrint("NANDSeek. [%d]\n", result);
}

static void __wudUpdateWiiFitCallback(s32 result, NANDCommandBlock* pBlock) {
	if (_wcb.syncState != WUD_STATE_SYNC_START)
	{
		_wcb.syncState = result == WUD_WII_FIT_WRITE_SIZE
		                          ? WUD_STATE_WII_FIT_CLOSE
		                          : WUD_STATE_SYNC_SC_FLUSH;
	}

	if (_wcb.deleteState != WUD_STATE_DELETE_START)
	{
		_wcb.deleteState = result == WUD_WII_FIT_WRITE_SIZE
		                            ? WUD_STATE_WII_FIT_CLOSE
		                            : WUD_STATE_DELETE_CLEANUP_SETTING;
	}

	sWudNandLocked = FALSE;
	DEBUGPrint("NANDWrite. [%d]\n", result);
}

static void __wudCloseWiiFitCallback(s32 result, NANDCommandBlock* pBlock) {
	if (_wcb.syncState != WUD_STATE_SYNC_START)
		_wcb.syncState = WUD_STATE_SYNC_SC_FLUSH;

	if (_wcb.deleteState != WUD_STATE_DELETE_START)
		_wcb.deleteState = WUD_STATE_DELETE_CLEANUP_SETTING;

	sWudNandLocked = FALSE;
	DEBUGPrint("NANDClose. [%d]\n", result);
}

static u8 __wudSyncDone(void) {
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    int i;
    WUDSyncDeviceCallback pSyncCallback;

    if (p->syncSkipChecks) {
        for (i = 0; i < WUD_MAX_DEV_ENTRY; i++) {
            pInfo = __wudGetDevInfoByIndex(i);

            if (pInfo->status == 8) {
                WUDSetSniffMode(pInfo->devAddr, 8);
                return WUD_STATE_SYNC_DONE;
            }
        }
    }

    OSCancelAlarm(&p->alarm);

    if (!sWudAbortSync) {
        WUDSetVisibility(FALSE, TRUE);
    }

    pSyncCallback = p->syncType == WUD_SYNC_TYPE_STANDARD ? p->syncStdCB : p->syncSmpCB;

    if (pSyncCallback) {
        pSyncCallback(WUD_RESULT_SYNC_DONE, p->syncedNum);
    }

    DEBUGPrint("Pairing Done\n");
    return WUD_STATE_SYNC_START;
}

static void __wudSyncHandler(void) {
    WUDCB* p = &_wcb;

    switch (p->syncState) {
    case WUD_STATE_SYNC_PREPARE_SEARCH: {
        p->syncState = __wudSyncPrepareSearch();
        break;
    }

    case WUD_STATE_SYNC_WAIT_FOR_START_SEARCH: {
        p->syncState = __wudSyncWaitForStartSearch();
        break;
    }

    case WUD_STATE_SYNC_START_SEARCH: {
        p->syncState = __wudSyncStartSearch();
        break;
    }

    case WUD_STATE_SYNC_CHECK_SEARCH_RESULT: {
        p->syncState = __wudSyncCheckSearchResult();
        break;
    }

    case WUD_STATE_SYNC_IS_EXISTED_DEVICE: {
        p->syncState = __wudSyncIsExistedDevice();
        break;
    }

    case WUD_STATE_SYNC_PREPARE_FOR_EXISTED_DEVICE: {
        p->syncState = __wudSyncPrepareForExistedDevice();
        break;
    }

    case WUD_STATE_SYNC_PREPARE_FOR_UNKNOWN_DEVICE: {
        p->syncState = __wudSyncPrepareForUnknownDevice();
        break;
    }

    case WUD_STATE_SYNC_TRY_CONNECT: {
        p->syncState = __wudSyncTryConnect();
        break;
    }

    case WUD_STATE_SYNC_REGISTER_DEVICE: {
        p->syncState = __wudSyncRegisterDevice();
        break;
    }

    case WUD_STATE_SYNC_VIRGIN_SIMPLE: {
        p->syncState = __wudSyncVirginSimple();
        break;
    }

    case WUD_STATE_SYNC_VIRGIN_STANDARD: {
        p->syncState = __wudSyncVirginStandard();
        break;
    }

    case WUD_STATE_SYNC_CHANGE_SIMPLE_TO_STANDARD: {
        p->syncState = __wudSyncChangeSmp2Std();
        break;
    }

    case WUD_STATE_SYNC_STORED_LINK_KEY_TO_EEPROM: {
        p->syncState = __wudSyncStoredLinkKeyToE2prom();
        break;
    }

    case WUD_STATE_SYNC_WAIT_FOR_STORING: {
        p->syncState = __wudSyncWaitForStoring();
        break;
    }

    case WUD_STATE_SYNC_STORED_DEV_INFO_TO_NAND: {
        p->syncState = __wudSyncStoredDevInfoToNand();
        break;
    }

    case WUD_STATE_SYNC_SC_FLUSH: {
        __wudSyncScFlush();
        break;
    }

    case WUD_STATE_SYNC_WAIT_FOR_READING: {
        p->syncState = __wudSyncWaitForReading();
        break;
    }

    case WUD_STATE_SYNC_COMPLETE: {
        p->syncState = __wudSyncComplete();
        break;
    }

    case WUD_STATE_SYNC_ERROR: {
        p->syncState = __wudSyncError();
        break;
    }

    case WUD_STATE_SYNC_DONE: {
        p->syncState = __wudSyncDone();
        break;
    }

    case WUD_STATE_SYNC_WAIT_FOR_SEARCH_RESULT: {
        p->syncState = __wudSyncWaitForSearchResult();
        break;
    }

    case WUD_STATE_SYNC_WAIT_FOR_INCOMING: {
        p->syncState = __wudSyncWaitForIncoming();
        break;
    }

    case WUD_STATE_WII_FIT_OPEN:
        __wudOpenWiiFitFile();
        break;
    case WUD_STATE_WII_FIT_SEEK:
        __wudSeekWiiFitFile();
        break;
    case WUD_STATE_WII_FIT_UPDATE:
        __wudUpdateWiiFitFile();
        break;
    case WUD_STATE_WII_FIT_CLOSE:
        __wudCloseWiiFit();
        break;
    case WUD_STATE_SYNC_6:
    case WUD_STATE_SYNC_13:
    case WUD_STATE_SYNC_CANCEL_SEARCH: {
        break;
    }

    case WUD_STATE_SYNC_11:
    case WUD_STATE_SYNC_12:
    default: {
        DEBUGPrint("WARNING: Illigal status\n");
        break;
    }
    }
}

void __wudSyncHandler0(OSAlarm* pAlarm, OSContext* pContext) {
    OSSwitchFiberEx((u32)pAlarm, (u32)pContext, 0, 0, (u32)__wudSyncHandler, (u32)sWudHandlerStack + sizeof(sWudHandlerStack));
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

static u8 __wudDeleteCleanupDatabase(void)
{
	int i;

	WUDCB *p_wcb = &_wcb;
	WUDDevInfo *p_info;

	if (_WUDGetLinkNumber() != 0)
		return WUD_STATE_DELETE_CLEANUP_DATABASE;

	for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_STD; i++)
	{
		p_info = &p_wcb->stdDevs[i];
		if (p_info->status == 1)
			WUDiRemoveDevice(p_info->devAddr);
	}

	for (i = 0; i < WUD_MAX_DEV_ENTRY_FOR_SMP; i++)
	{
		p_info = &p_wcb->smpDevs[i];
		if (p_info->status == 1)
			WUDiRemoveDevice(p_info->devAddr);
	}

	if (WUDIsLinkedWBC())
	{
		if (SCGetProductGameRegion() == WUD_WII_FIT_REGION_JAPAN)
		{
			LINK_KEY link_key = {0};
			__wudPrepareWBCDevInfo(NULL, link_key, NULL);

			return WUD_STATE_WII_FIT_OPEN;
		}
		else
		{
			return WUD_STATE_DELETE_CLEANUP_SETTING;
		}
	}
	else
	{
		return WUD_STATE_DELETE_CLEANUP_SETTING;
	}
}

static void __wudDeleteHandler(void)
{
	WUDCB *p_wcb = &_wcb;

	p_wcb->syncState = WUD_STATE_SYNC_START;

	switch (p_wcb->deleteState)
	{
	case WUD_STATE_DELETE_DISALLOW_INCOMING:
		p_wcb->deleteState = __wudDeleteDisallowIncoming();
		break;

	case WUD_STATE_DELETE_DISCONNECT_ALL:
		p_wcb->deleteState = WUDiTerminateDevice();
		break;

	case WUD_STATE_DELETE_CLEANUP_DATABASE:
		p_wcb->deleteState = __wudDeleteCleanupDatabase();
		break;

	case WUD_STATE_DELETE_CLEANUP_SETTING:
		__wudDeleteCleanupSetting();
		break;

	case WUD_STATE_DELETE_DONE:
		p_wcb->deleteState = __wudDeleteDone();
		break;

	case WUD_STATE_WII_FIT_OPEN:
		__wudOpenWiiFitFile();
		break;

	case WUD_STATE_WII_FIT_SEEK:
		__wudSeekWiiFitFile();
		break;

	case WUD_STATE_WII_FIT_UPDATE:
		__wudUpdateWiiFitFile();
		break;

	case WUD_STATE_WII_FIT_CLOSE:
		__wudCloseWiiFit();
		break;

	case WUD_STATE_DELETE_7:
		break;
	}
}

static void __wudDeleteHandler0(OSAlarm* pAlarm, OSContext* pContext) {
    OSSwitchFiberEx((u32)pAlarm, (u32)pContext, 0, 0, (u32)__wudDeleteHandler, (u32)sWudHandlerStack + sizeof(sWudHandlerStack));
}

static WUDStackState __wudStackCheckDeviceInfo(void) {
    WUDCB* p = &_wcb;
    WUDDevInfo* pInfo;
    int i;

    if (p->linkKeyState == WUD_STATE_LINK_KEY_START) {
        for (i = 0; i < WUD_MAX_DEV_ENTRY; i++) {
            pInfo = __wudGetDevInfoByIndex(i);
            if (pInfo->status == 0) {
                continue;
            }

            if (pInfo->linkKeyStorage == 1) {
                p->linkKeyState = WUD_STATE_LINK_KEY_DELETING;
                BTM_DeleteStoredLinkKey(pInfo->devAddr, __wudLinkKeyEventStackCallback);

                pInfo->linkKeyStorage = 0;
                return WUD_STATE_STACK_CHECK_DEVICE_INFO;
            }

            if (pInfo->linkKeyStorage != 3) {
                __wudRemoveDevInfo(pInfo->devAddr);
            }
        }

        return WUD_STATE_STACK_DONE;
    }

    return WUD_STATE_STACK_CHECK_DEVICE_INFO;
}

static void __wudStackHandler(void) {
    WUDCB* p = &_wcb;

    switch (p->stackState) {
    case WUD_STATE_STACK_GET_STORED_LINK_KEY: {
        p->stackState = __wudStackGetStoredLinkKey();
        break;
    }

    case WUD_STATE_STACK_CHECK_DEVICE_INFO: {
        p->stackState = __wudStackCheckDeviceInfo();
        break;
    }

    case WUD_STATE_STACK_DONE: {
        p->stackState = __wudStackDone();
        break;
    }
    }
}

void __wudEnableStackHandler0(OSAlarm* pAlarm, OSContext* pContext) {
    OSSwitchFiberEx((u32)pAlarm, (u32)pContext, 0, 0, (u32)__wudStackHandler, (u32)sWudHandlerStack + sizeof(sWudHandlerStack));
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

static void __wudShutdownHandler(void) {
    WUDCB* p = &_wcb;

    switch (p->shutdownState) {
    case WUD_STATE_SHUTDOWN_STORE_SETTINGS: {
        __wudShutdownStoreSetting();
        break;
    }

    case WUD_STATE_SHUTDOWN_DONE: {
        __wudShutdownDone();
        break;
    }

    case WUD_STATE_SHUTDOWN_FLUSH_SETTINGS: {
        break;
    }
    }
}

static void __wudShutdownHandler0(OSAlarm* pAlarm, OSContext* pContext) {
    OSSwitchFiberEx((u32)pAlarm, (u32)pContext, 0, 0, (u32)__wudShutdownHandler, (u32)sWudHandlerStack + sizeof(sWudHandlerStack));
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
