#include "revolution/usb.h"
#include "private/iosrestypes.h"
#include "private/iostypes.h"
#include "revolution/os.h"
#include "revolution/ipc.h"
#include "revolution/os/OSInterrupt.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define ROUNDUP(sz) (((u32)(sz) + 32 - 1) & ~(u32)(32 - 1))

static IOSHeapId sUsbHeapId = -1;
void *lo = NULL, *hi = NULL;

typedef struct {
    char* data;
    u16 wLength;
} IntBlkCtrlReq;

typedef struct iusb_Ctxt {
    USBCallbackFunc cb;
    USBIsoCallbackFunc icb;
    void* cbArg;
    void* xfer;
    void* spare;
    void* clean[8];
    u32 nclean;

    union {
        char path[ROUNDUP(64)];
        char des[ROUNDUP(sizeof(USB_DevDescr))];
        char reqBuf[ROUNDUP(sizeof(IntBlkCtrlReq))];
    } u ATTRIBUTE_ALIGN(32);

} iusb_ctxt;

static u8 s_usb_log = 0;
static u8 s_usb_err = 1;

void _usb_enable_log() {
    s_usb_log = 1;
}

void _usb_disable_log() {
    s_usb_log = 0;
}

void USB_LOG(char* str, ...) {
    va_list vlist;
    if (s_usb_log) {
        OSReport("USB: ");
        va_start(vlist, str);
        vprintf(str, vlist);
        va_end(vlist);
    } else {
        (void)str;
    }
}

void USB_ERR(char* str, ...) {
    va_list vlist;
    if (s_usb_err) {
        OSReport("USB ERR: ");
        va_start(vlist, str);
        vprintf(str, vlist);
        va_end(vlist);
    } else {
        (void)str;
    }
}

static void* IOSAlloc(u32 size) {
    void* rv;
    if ((rv = iosAllocAligned(sUsbHeapId, size, 32)) == NULL) {
        USB_ERR("iosAllocAligned(%d, %u) failed: %d\n", sUsbHeapId, size, rv);
    }

    return rv;
}

static void IOSFree(void* addr) {
    IOSError rv;

    if (addr) {
        if ((rv = iosFree(sUsbHeapId, addr)) < 0) {
            USB_ERR("iosFree(%d, 0x%x) failed: %d\n", sUsbHeapId, addr, rv);
        }
    }
    return;
}

IOSError IUSB_OpenLib(void) {
    IOSError rc = IOS_ERROR_OK;
    u32 mask = (u32)OSDisableInterrupts();

    if (-1 != sUsbHeapId) {
        USB_LOG("Library is already initialized. Heap Id = %d\n", sUsbHeapId);
        goto out;
    }

    if (NULL == lo) {
        lo = IPCGetBufferLo();
        hi = IPCGetBufferHi();
        USB_LOG("iusb size: %d lo: %x hi: %x\n", sizeof(iusb_ctxt), lo, hi);

        if ((u32)lo + ROUNDUP(16384) > (u32)hi) {
            USB_ERR("Not enough IPC arena\n");
            rc = IOS_ERROR_FAIL_ALLOC;
            goto out;
        }

        IPCSetBufferLo((void*)((u32)lo + ROUNDUP(16384)));
    }

    sUsbHeapId = iosCreateHeap(lo, 16384);

    if (sUsbHeapId < 0) {
        USB_ERR("Not enough heaps\n");
        rc = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

out:
    OSRestoreInterrupts((BOOL)mask);
    return rc;
}

IOSError IUSB_CloseLib(void) {
    return IOS_ERROR_OK;
}

IOSError _intrBlkCtrlIsoCb(IOSError ret, void* ctxt) {
    u32 i;
    IOSError rv = ret;
    iusb_ctxt* req = (iusb_ctxt*)ctxt;

    USB_LOG("_intrBlkCtrlIsoCb returned: %d\n", rv);
    USB_LOG("_intrBlkCtrlIsoCb: nclean = %d\n", req->nclean);

    if ((req->nclean != 7) && (req->nclean != 3) && (req->nclean != 0) && (req->nclean != 4) && (req->nclean != 2)) {
        USB_ERR("__intrBlkCtrlIsoCb: got invalid nclean\n");
    } else {
        for (i = 0; i < req->nclean; ++i) {
            USB_LOG("Freeing clean[%d] = %x\n", i, req->clean[i]);
            IOSFree(req->clean[i]);
        }
        req->nclean = 0;
    }

    USB_LOG("cb = %x cbArg = %x\n", req->cb, req->cbArg);

    if (req->cb) {
        req->cb(ret, req->cbArg);
    } else if (req->icb) {
        USB_LOG("calling iso callback\n");
        req->icb(ret, req->xfer, req->cbArg);
    }

    IOSFree(req);
    return rv;
}

IOSError IUSB_OpenDeviceIds(const char* did, u16 vid, u16 pid, IOSFd* fd) {
    IOSError rv = IOS_ERROR_OK;
    iusb_ctxt* req = 0;

    if (fd == 0) {
        rv = IOS_ERROR_INVALID;
        goto out;
    }

    req = IOSAlloc(ROUNDUP(sizeof(*req)));
    if (req == 0) {
        USB_ERR("OpenDeviceIds: Not enough memory\n");
        rv = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

    memset(req, 0, sizeof(iusb_ctxt));
    snprintf(req->u.path, sizeof(req->u.path), "/dev/usb/%s/%x/%x", did, vid, pid);
    USB_LOG("OpenDevice - %s\n", req->u.path);
    rv = IOS_Open(req->u.path, 0);
    USB_LOG("OpenDevice returned: %d\n", rv);
    *fd = rv;

out:
    IOSFree(req);
    return rv;
}

IOSError IUSB_OpenDeviceIdsAsync(const char* did, u16 vid, u16 pid, USBCallbackFunc cb, void* cbArg) {
    IOSError rv = IOS_ERROR_OK;
    iusb_ctxt* req;

    USB_LOG("OpenDevice\n");
    req = IOSAlloc(ROUNDUP(sizeof(*req)));
    if (req == 0) {
        USB_ERR("OpenDeviceIdsAsync: Not enough memory\n");
        rv = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

    memset(req, 0, sizeof(iusb_ctxt));

    req->cb = cb;
    req->cbArg = cbArg;
    req->nclean = 0;

    snprintf(req->u.path, 64, "/dev/usb/%s/%x/%x", did, vid, pid);
    USB_LOG("OpenDevice - %s\n", req->u.path);
    rv = IOS_OpenAsync(req->u.path, 0, _intrBlkCtrlIsoCb, req);
    USB_LOG("OpenDevice returned: %d\n", rv);
    if (rv < 0) {
        IOSFree(req);
        goto out;
    }

out:
    return rv;
}

IOSError IUSB_CloseDevice(IOSFd fd) {
    IOSError rv = IOS_ERROR_OK;

    USB_LOG("CloseDevice\n");
    rv = IOS_Close(fd);
    USB_LOG("CloseDevice returned: %d\n", rv);

out:
    return rv;
}

IOSError IUSB_CloseDeviceAsync(IOSFd fd, USBCallbackFunc cb, void* cbArg) {
    IOSError rv = IOS_ERROR_OK;
    iusb_ctxt* req;

    USB_LOG("CloseDevice\n");
    req = IOSAlloc(ROUNDUP(sizeof(*req)));
    if (req == 0) {
        USB_ERR("CloseDeviceAsync: Not enough memory\n");
        rv = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

    memset(req, 0, sizeof(iusb_ctxt));

    req->cb = cb;
    req->cbArg = cbArg;
    req->nclean = 0;

    rv = IOS_CloseAsync(fd, _intrBlkCtrlIsoCb, req);
    USB_LOG("CloseDevice returned: %d\n", rv);
    if (rv < 0) {
        IOSFree(req);
        goto out;
    }

out:
    return rv;
}

static IOSFd __openDevice(const char* path) {
    IOSFd rv;
    char* pathBuf;
    iusb_ctxt* ctx = IOSAlloc(ROUNDUP(sizeof(iusb_ctxt)));

    if (ctx == 0) {
        USB_ERR("openDevice: Not enough memory\n");
        rv = IOS_ERROR_FAIL_ALLOC;
        goto done;
    }

    pathBuf = ctx->u.path;
    strncpy(pathBuf, path, 64);
    rv = IOS_Open(pathBuf, 0);
    IOSFree(ctx);

done:
    return rv;
}

IOSError IUSB_GetDeviceList(const char* path, USBDeviceInfo* deviceList, u8 maxDev, u8 deviceClass, u8* numDev) {
    IOSError rv = IOS_ERROR_OK;
    IOSIoVector* vector;
    u8* smaxDev;
    u8* sclass;
    u8* snumDev;
    IOSFd fd;

    if ((u32)deviceList & 0x1f) {
        rv = IOS_ERROR_INVALID;
        goto out;
    }
    if ((fd = __openDevice(path)) < 0) {
        rv = fd;
        goto out;
    }

    vector = IOSAlloc(ROUNDUP(sizeof(IOSIoVector)) * 4);
    smaxDev = IOSAlloc(ROUNDUP(sizeof(u8)));
    sclass = IOSAlloc(ROUNDUP(sizeof(u8)));
    snumDev = IOSAlloc(ROUNDUP(sizeof(u8)));
    if (NULL == smaxDev || NULL == sclass || NULL == snumDev || NULL == vector) {
        USB_ERR("getDeviceList: Not enough memory\n");
        rv = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

    *smaxDev = maxDev;
    *sclass = deviceClass;
    *snumDev = 0;

    vector[0].base = smaxDev;
    vector[0].length = sizeof(u8);
    vector[1].base = sclass;
    vector[1].length = sizeof(u8);
    vector[2].base = (u8*)snumDev;
    vector[2].length = sizeof(u8);
    vector[3].base = (u8*)deviceList;
    vector[3].length = sizeof(USBDeviceInfo) * maxDev;

    DCInvalidateRange(deviceList, sizeof(USBDeviceInfo) * maxDev);
    DCInvalidateRange(snumDev, ROUNDUP(sizeof(u8)));
    DCFlushRange(sclass, ROUNDUP(sizeof(u8)));
    DCFlushRange(smaxDev, ROUNDUP(sizeof(u8)));
    DCFlushRange(vector, ROUNDUP(sizeof(IOSIoVector)) * 4);

    rv = IOS_Ioctlv(fd, 12, 2, 2, vector);
    *numDev = *snumDev;
    IOS_Close(fd);

out:
    IOSFree(snumDev);
    IOSFree(smaxDev);
    IOSFree(sclass);
    IOSFree(vector);
    return rv;
}

static IOSError __LongBlkMsgInt(s32 fd, u32 endpoint, u32 length, void* buffer,
                                 USBCallbackFunc callback, void* callbackArg,
                                 u8 async) {
    IOSError result;
    IOSIoVector* vectors;
    u8* endpointWork;
    u32* lengthWork;
    iusb_ctxt* block;

    vectors = (IOSIoVector*)IOSAlloc(0x60);
    endpointWork = (u8*)IOSAlloc(32);
    lengthWork = (u32*)IOSAlloc(32);

    if (vectors == NULL || endpointWork == NULL || lengthWork == NULL) {
        USB_ERR("__LongBlkMsgInt: Not enough memory\n");
        result = IOS_ERROR_FAIL_ALLOC;
        goto end;
    }

    *endpointWork = (u8)endpoint;
    *lengthWork = (u32)length;

    vectors[0].base = (u8*)endpointWork;
    vectors[0].length = sizeof(u8);

    vectors[1].base = (u8*)lengthWork;
    vectors[1].length = sizeof(u32);

    vectors[2].base = (u8*)buffer;
    vectors[2].length = length;

    DCFlushRange(endpointWork, 32);
    DCFlushRange(lengthWork, 32);
    DCFlushRange(vectors, 0x60);

    if (!async) {
        result = IOS_Ioctlv(fd, 10, 2, 1, vectors);
        USB_LOG("Long bulk ioctl returned: %d\n", result);
        goto end;
    }

    block = IOSAlloc(sizeof(iusb_ctxt));
    if (block == NULL) {
        USB_ERR("LongBlkMsgInt (async): Not enough memory\n");
        result = IOS_ERROR_FAIL_ALLOC;
        goto end;
    }
    memset(block, 0, 0x80);

    block->cb = callback;
    block->cbArg = callbackArg;
    USB_LOG("longblkmsg: cb = 0x%x cbArg = 0x%x\n", block->cb,
            block->cbArg);

    block->nclean = 3;
    block->clean[0] = endpointWork;
    block->clean[1] = lengthWork;
    block->clean[2] = vectors;

    ((IntBlkCtrlReq*)block->u.reqBuf)->data = buffer;
    ((IntBlkCtrlReq*)block->u.reqBuf)->wLength = length;

    result =
        IOS_IoctlvAsync(fd, 10, 2, 1, vectors, _intrBlkCtrlIsoCb, block);
    if (result >= IOS_ERROR_OK) {
        goto end_async;
    }
    if (result == IOS_ERROR_FAIL_ALLOC) {
        OSReport("%s: IoctlvAsync returned error %d\n", __FUNCTION__, result);
    }

    IOSFree(block);

end:
    IOSFree(endpointWork);
    IOSFree(lengthWork);
    IOSFree(vectors);

end_async:
    return result;
}

static IOSError __IntrBlkMsgInt(s32 fd, u32 endpoint, u32 length, void* buffer,
                                 u8 ioctl, USBCallbackFunc callback,
                                 void* callbackArg, u8 async) {
    IOSError result;
    IOSIoVector* vectors;
    u8* endpointWork;
    u16* lengthWork;
    iusb_ctxt* block;

    vectors = (IOSIoVector*)IOSAlloc(0x60);
    endpointWork = (u8*)IOSAlloc(32);
    lengthWork = (u16*)IOSAlloc(32);

    if (vectors == NULL || endpointWork == NULL || lengthWork == NULL) {
        USB_ERR("__IntrBlkMsgInt: Not enough memory\n");
        result = IOS_ERROR_FAIL_ALLOC;
        goto end;
    }

    *endpointWork = (u8)endpoint;
    *lengthWork = (u16)length;

    vectors[0].base = (u8*)endpointWork;
    vectors[0].length = sizeof(u8);

    vectors[1].base = (u8*)lengthWork;
    vectors[1].length = sizeof(u16);

    vectors[2].base = (u8*)buffer;
    vectors[2].length = length;

    DCFlushRange(endpointWork, 32);
    DCFlushRange(lengthWork, 32);
    DCFlushRange(vectors, 0x60);

    if (!async) {
        result = IOS_Ioctlv(fd, ioctl, 2, 1, vectors);
        USB_LOG("intr/blk ioctl returned: %d\n", result);
        goto end;
    }

    block = IOSAlloc(sizeof(iusb_ctxt));
    if (block == NULL) {
        USB_ERR("IntBlkMsgInt (async): Not enough memory\n");
        result = IOS_ERROR_FAIL_ALLOC;
        goto end;
    }
    memset(block, 0, 0x80);

    block->cb = callback;
    block->cbArg = callbackArg;
    USB_LOG("intrblkmsg: cb = 0x%x cbArg = 0x%x\n", block->cb,
            block->cbArg);

    block->nclean = 3;
    block->clean[0] = endpointWork;
    block->clean[1] = lengthWork;
    block->clean[2] = vectors;

    ((IntBlkCtrlReq*)block->u.reqBuf)->data = buffer;
    ((IntBlkCtrlReq*)block->u.reqBuf)->wLength = length;

    result = IOS_IoctlvAsync(fd, ioctl, 2, 1, vectors, _intrBlkCtrlIsoCb, block);
    if (result >= IOS_ERROR_OK) {
        goto end_async;
    }
    if (result == IOS_ERROR_FAIL_ALLOC) {
        OSReport("%s: IoctlvAsync returned error %d\n", __FUNCTION__,
                 result);
    }

    IOSFree(block);

end:
    IOSFree(endpointWork);
    IOSFree(lengthWork);
    IOSFree(vectors);

end_async:
    return result;
}

const char* const gUsbStrippedTransferMessages[] = {
    "calling short blk transfer fn: buflen = %u limit = %u\n",
    "calling long blk transfer fn: buflen = %u limit = %u\n"
};

IOSError IUSB_ReadIntrMsg(IOSFd fd, u32 ep, u32 buflen, char* buf) {
    IOSError rv;

    DCInvalidateRange(buf, buflen);
    rv = __IntrBlkMsgInt(fd, ep, buflen, buf, 2, NULL, NULL, 0);
    if (rv < 0)
        goto out;

out:
    return rv;
}

IOSError IUSB_ReadIntrMsgAsync(IOSFd fd, u32 ep, u32 buflen, char* buf, USBCallbackFunc cb, void* cbArg) {
    DCInvalidateRange(buf, buflen);
    return __IntrBlkMsgInt(fd, ep, buflen, buf, 2, cb, cbArg, 1);
}

IOSError IUSB_WriteIntrMsg(IOSFd fd, u32 ep, u32 buflen, char* buf) {
    DCFlushRange((void*)buf, buflen);
    return __IntrBlkMsgInt(fd, ep, buflen, buf, 2, NULL, NULL, 0);
}

IOSError IUSB_WriteIntrMsgAsync(IOSFd fd, u32 ep, u32 buflen, char* buf, USBCallbackFunc cb, void* cbArg) {
    DCFlushRange((void*)buf, buflen);
    return __IntrBlkMsgInt(fd, ep, buflen, buf, 2, cb, cbArg, 1);
}

IOSError IUSB_ReadBlkMsgAsync(s32 fd, u32 endpoint, u32 length, char* buffer,
                               USBCallbackFunc callback, void* callbackArg) {
    DCInvalidateRange(buffer, length);
    if (length <= 0xffff)
        return __IntrBlkMsgInt(fd, endpoint, length, (void*)buffer,
                               1, callback, callbackArg, TRUE);
    return __LongBlkMsgInt(fd, endpoint, length, (void*)buffer, callback,
                           callbackArg, TRUE);
}

IOSError IUSB_WriteBlkMsgAsync(s32 fd, u32 endpoint, u32 length,
                                char* buffer, USBCallbackFunc callback,
                                void* callbackArg) {
    DCFlushRange(buffer, length);
    if(length <= 0xffff)
        return __IntrBlkMsgInt(fd, endpoint, length, (void*)buffer,
                               1, callback, callbackArg, TRUE);
    return __LongBlkMsgInt(fd, endpoint, length, (void*)buffer, callback,
                           callbackArg, TRUE);
}

static IOSError __CtrlMsgInt(s32 fd, u8 requestType, u8 request, u16 value,
                              u16 index, u16 length, void* buffer,
                              USBCallbackFunc callback, void* callbackArg,
                              u8 async) {
    IOSError result;
    IOSIoVector* vectors;
    u8* requestTypeWork;
    u8* requestWork;
    u8* reservedByteWork;
    u16* valueWork;
    u16* indexWork;
    u16* lengthWork;
    iusb_ctxt* block;

    if ((buffer == NULL && length != 0) || (u32)buffer % 32 != 0) {
        result = IOS_ERROR_INVALID;
        USB_ERR("ctrlmsg: bad data buffer\n");
        goto end_async;
    }

    vectors = (IOSIoVector*)IOSAlloc(0xE0);
    requestTypeWork = (u8*)IOSAlloc(32);
    requestWork = (u8*)IOSAlloc(32);
    reservedByteWork = (u8*)IOSAlloc(32);
    valueWork = (u16*)IOSAlloc(32);
    indexWork = (u16*)IOSAlloc(32);
    lengthWork = (u16*)IOSAlloc(32);

    if (requestTypeWork == NULL || requestWork == NULL || reservedByteWork == NULL ||
        valueWork == NULL || indexWork == NULL || lengthWork == NULL ||
        vectors == NULL) {
        USB_ERR("Ctrl Msg: Not enough memory\n");
        result = IOS_ERROR_FAIL_ALLOC;
        goto end;
    }

    *requestTypeWork = requestType;
    *requestWork = request;
    *valueWork = (value & 0xFF) << 8 | value >> 8 & 0xFF;
    *indexWork = (index & 0xFF) << 8 | index >> 8 & 0xFF;
    *lengthWork = (length & 0xFF) << 8 | length >> 8 & 0xFF;
    *reservedByteWork = 0;

    vectors[0].base = (u8*)requestTypeWork;
    vectors[0].length = sizeof(u8);

    vectors[1].base = (u8*)requestWork;
    vectors[1].length = sizeof(u8);

    vectors[2].base = (u8*)valueWork;
    vectors[2].length = sizeof(u16);

    vectors[3].base = (u8*)indexWork;
    vectors[3].length = sizeof(u16);

    vectors[4].base = (u8*)lengthWork;
    vectors[4].length = sizeof(u16);

    vectors[5].base = (u8*)reservedByteWork;
    vectors[5].length = sizeof(u8);

    vectors[6].base = (u8*)buffer;
    vectors[6].length = length;

    DCFlushRange(requestTypeWork, 32);
    DCFlushRange(requestWork, 32);
    DCFlushRange(reservedByteWork, 32);
    DCFlushRange(valueWork, 32);
    DCFlushRange(indexWork, 32);
    DCFlushRange(lengthWork, 32);
    DCFlushRange(vectors, 0xE0);

    if (!async) {
        result = IOS_Ioctlv(fd, 0, 6, 1, vectors);
        goto end;
    }

    block = IOSAlloc(sizeof(iusb_ctxt));
    if (block == NULL) {
        USB_ERR("CtrlMsgInt (async): Not enough memory\n");
        result = IOS_ERROR_FAIL_ALLOC;
        goto end;
    }
    memset(block, 0, sizeof(iusb_ctxt));

    block->cb = callback;
    block->cbArg = callbackArg;
    USB_LOG("ctrlmsgint: cb = 0x%x cbArg = 0x%x\n", block->cb,
            block->cbArg);

    block->nclean = 7;
    block->clean[0] = requestTypeWork;
    block->clean[1] = requestWork;
    block->clean[2] = valueWork;
    block->clean[3] = indexWork;
    block->clean[4] = lengthWork;
    block->clean[5] = reservedByteWork;
    block->clean[6] = vectors;

    ((IntBlkCtrlReq*)block->u.reqBuf)->data = buffer;
    ((IntBlkCtrlReq*)block->u.reqBuf)->wLength = length;

    result = IOS_IoctlvAsync(fd, 0, 6, 1, vectors,
                             _intrBlkCtrlIsoCb, block);
    USB_LOG("Ctrl Msg async returned: %d\n", result);

    if (result >= IOS_ERROR_OK) {
        goto end_async;
    }

    IOSFree(block);

end:
    IOSFree(requestTypeWork);
    IOSFree(requestWork);
    IOSFree(valueWork);
    IOSFree(indexWork);
    IOSFree(lengthWork);
    IOSFree(reservedByteWork);
    IOSFree(vectors);

end_async:
    return result;
}

IOSError IUSB_ReadCtrlMsg(IOSFd fd, u8 reqType, u8 request, u16 value, u16 index, u16 buflen, char* buf) {
    DCInvalidateRange(buf, buflen);
    return __CtrlMsgInt(fd, reqType, request, value, index, buflen, buf, NULL, NULL, 0);
}

IOSError IUSB_ReadCtrlMsgAsync(IOSFd fd, u8 reqType, u8 request, u16 value, u16 index, u16 buflen, char* buf, USBCallbackFunc cb, void* cbArg) {
    DCInvalidateRange(buf, buflen);
    return __CtrlMsgInt(fd, reqType, request, value, index, buflen, buf, cb, cbArg, 1);
}

IOSError IUSB_WriteCtrlMsg(IOSFd fd, u8 reqType, u8 request, u16 value, u16 index, u16 buflen, char* buf) {
    DCFlushRange((void*)buf, buflen);
    return __CtrlMsgInt(fd, reqType, request, value, index, buflen, buf, NULL, NULL, 0);
}

IOSError IUSB_WriteCtrlMsgAsync(IOSFd fd, u8 reqType, u8 request, u16 value, u16 index, u16 buflen, char* buf, USBCallbackFunc cb, void* cbArg) {
    DCFlushRange((void*)buf, buflen);
    return __CtrlMsgInt(fd, reqType, request, value, index, buflen, buf, cb, cbArg, 1);
}

static s8 unicode2ascii(char* tbuf, int buflen) {
    char buf[128];
    s8 di, si;

    if (tbuf[1] != 0x03) {
        di = -1;
        goto out;
    }

    for (di = 0, si = 2; si < tbuf[0] && si < buflen; si += 2) {
        if (di >= (sizeof(buf) - 1))
            break;
        if (tbuf[si + 1])
            buf[di++] = '?';
        else
            buf[di++] = tbuf[si];
    }

    buf[di] = 0;
    memcpy(tbuf, buf, (u32)di);

out:
    return di;
}

static void _GetStrCb(IOSError ret, void* ctxt) {
    IOSError rv = ret;
    iusb_ctxt* req = (iusb_ctxt*)ctxt;
    char* buf;
    u16 buflen;
    s8 len;

    USB_LOG("GetStrCb returned: %d\n", rv);
    if (rv <= 0)
        goto out;

    buf = (char*)((IntBlkCtrlReq*)req->u.reqBuf)->data;
    buflen = ((IntBlkCtrlReq*)req->u.reqBuf)->wLength;
    USB_LOG("GetStrCb: buf = 0x%x buflen = %u\n", buf, buflen);
    if ((len = unicode2ascii(buf, buflen)) < 0)
        USB_ERR("Failed to convert buffer from unicode 2 ascii\n");
    else
        buf[len] = '\0';

out:
    if (req->cb) {
        USB_LOG("calling cb 0x%x with arg 0x%x\n", req->cb, req->cbArg);
        req->cb(ret, req->cbArg);
    }
    IOSFree(req);
    return;
}

IOSError IUSB_GetAsciiStr(IOSFd fd, u8 ep, u16 index, u16 langId, char* buf, u16 buflen) {
    IOSError rv = IOS_ERROR_OK;
    s8 len;

    USB_LOG("GetStr\n");

    DCInvalidateRange(buf, (u32)buflen);
    rv = __CtrlMsgInt(fd, 0x80, 0x06, (u16)((0x03 << 8) + index), langId, buflen, buf, NULL, NULL, 0);
    if (rv <= 0) {
        USB_ERR("Failed __CtrlMsg: %d", rv);
        goto out;
    }
    if ((len = unicode2ascii(buf, buflen)) < 0)
        USB_ERR("Failed to convert unicode 2 ascii\n");
    else
        buf[len] = '\0';

out:
    return rv;
}

IOSError IUSB_GetAsciiStrAsync(IOSFd fd, u8 ep, u16 index, u16 langId, char* buf, u16 buflen, USBCallbackFunc cb, void* cbArg) {
    IOSError rv = IOS_ERROR_OK;
    iusb_ctxt* req;

    USB_LOG("GetStr - _GetStrCb\n");
    req = IOSAlloc(ROUNDUP(sizeof(*req)));
    if (req == 0) {
        USB_ERR(" GetAsciiStrAsync: Not enough memory\n");
        rv = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

    memset(req, 0, sizeof(iusb_ctxt));

    req->cb = cb;
    req->cbArg = cbArg;
    req->nclean = 0;
    ((IntBlkCtrlReq*)req->u.reqBuf)->data = buf;
    ((IntBlkCtrlReq*)req->u.reqBuf)->wLength = buflen;
    DCInvalidateRange(buf, buflen);
    rv = __CtrlMsgInt(fd, 0x80, 0x06, (u16)((0x03 << 8) + index), langId, buflen, buf, _GetStrCb, req, 1);
    if (rv < 0) {
        USB_ERR("__CtrlMsgInt failed %d\n", rv);
        IOSFree(req);
        goto out;
    }

out:
    return rv;
}

static void _GetDescrCb(IOSError ret, void* ctxt) {
    IOSError rv = ret;
    iusb_ctxt* req = (iusb_ctxt*)ctxt;

    USB_LOG("GetDescrCb returned: %d\n", rv);
    if (rv <= 0)
        goto out;

    *(USB_DevDescr*)(req->spare) = *(USB_DevDescr*)req->u.des;

out:
    if (req->cb)
        req->cb(ret, req->cbArg);
    IOSFree(req);
    return;
}

IOSError IUSB_GetDevDescr(IOSFd fd, USB_DevDescr* des) {
    IOSError rv = IOS_ERROR_OK;
    iusb_ctxt* req = 0;

    USB_LOG("GetDevDescr\n");
    req = IOSAlloc(ROUNDUP(sizeof(*req)));
    if (req == 0) {
        USB_ERR("GetDevDescr: Not enough memory\n");
        rv = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

    memset(req, 0, sizeof(iusb_ctxt));
    DCInvalidateRange(req->u.des, (u32)sizeof(*des));
    rv = __CtrlMsgInt(fd, 0x80, 0x06, SWAP16(0x01), 0, sizeof(*des), req->u.des, NULL, NULL, 0);
    if (rv < 0) {
        USB_ERR("Failed __CtrlMsg: %d", rv);
        goto out;
    }
    USB_LOG("GetDevDescr: %d\n", rv);
    *des = *((USB_DevDescr*)req->u.des);

out:
    IOSFree(req);
    return rv;
}

IOSError IUSB_GetDevDescrAsync(IOSFd fd, USB_DevDescr* des, USBCallbackFunc cb, void* cbArg) {
    IOSError rv = IOS_ERROR_OK;
    iusb_ctxt* req;

    USB_LOG("GetDevDescr - _GetDescrCb\n");
    req = IOSAlloc(ROUNDUP(sizeof(*req)));
    if (req == 0) {
        USB_ERR("GetDevDescrAsync: Not enough memory\n");
        rv = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

    memset(req, 0, sizeof(iusb_ctxt));

    req->cb = cb;
    req->cbArg = cbArg;
    req->nclean = 0;
    req->spare = des;

    rv = IUSB_ReadCtrlMsgAsync(fd, 0x80, 0x06, SWAP16(0x01), 0, sizeof(*des), (char*)req->u.des, _GetDescrCb, req);
    if (rv < 0) {
        IOSFree(req);
        goto out;
    }

out:
    return rv;
}

IOSError IUSB_SuspendDevice(IOSFd fd) {
    return IOS_Ioctl(fd, 5, NULL, 0, 0, 0);
}

IOSError IUSB_ResumeDevice(IOSFd fd) {
    return IOS_Ioctl(fd, 6, NULL, 0, 0, 0);
}

IOSError IUSB_ResetDevice(IOSFd fd) {
    return IOS_Ioctl(fd, 29, NULL, 0, 0, 0);
}

IOSError IUSB_GetRhDesca(IOSFd fd, u32* desca) {
    IOSError rv = IOS_ERROR_OK;
    u32* tmp;

    if (NULL == desca) {
        rv = IOS_ERROR_INVALID;
        goto done;
    }

    tmp = IOSAlloc(ROUNDUP(sizeof(u32)));
    if (NULL == tmp) {
        rv = IOS_ERROR_FAIL_ALLOC;
        goto done;
    }
    rv = IOS_Ioctl(fd, 15, 0, 0, tmp, sizeof(u32));
    *desca = *tmp;
    IOSFree(tmp);

done:
    return rv;
}

static IOSError __getsetRhPortStat(IOSFd fd, s32 cmd, u8* v0, u32 s0, u8* v1, u32 s1, u32 rd, u32 wr) {
    IOSError rv;
    IOSIoVector* vector;

    if (NULL == v0 || NULL == v1) {
        rv = IOS_ERROR_INVALID;
        goto done;
    }

    vector = IOSAlloc(ROUNDUP(sizeof(IOSIoVector)) * 2);
    if (NULL == vector) {
        rv = IOS_ERROR_FAIL_ALLOC;
        goto done;
    }

    vector[0].base = v0;
    vector[0].length = s0;
    vector[1].base = v1;
    vector[1].length = s1;

    rv = IOS_Ioctlv(fd, cmd, rd, wr, vector);
    IOSFree(vector);

done:
    return rv;
}

IOSError IUSB_GetRhPortStatus(IOSFd fd, u8 port, u32* stat) {
    IOSError rv;
    u8* p;
    u32* s;

    p = IOSAlloc(ROUNDUP(sizeof(u8)));
    s = IOSAlloc(ROUNDUP(sizeof(u32)));
    if (NULL == p || NULL == s) {
        rv = IOS_ERROR_FAIL_ALLOC;
        goto done;
    }

    *p = port;
    rv = __getsetRhPortStat(fd, 20, p, sizeof(u8), (u8*)s, sizeof(u32), 1, 1);
    *stat = *s;

done:
    IOSFree(p);
    IOSFree(s);
    return rv;
}

IOSError IUSB_SetRhPortStatus(IOSFd fd, u8 port, u32 stat) {
    IOSError rv;
    u8* p;
    u32* s;

    p = IOSAlloc(ROUNDUP(sizeof(u8)));
    s = IOSAlloc(ROUNDUP(sizeof(u32)));
    if (NULL == p || NULL == s) {
        rv = IOS_ERROR_FAIL_ALLOC;
        goto done;
    }

    *p = port;
    *s = stat;
    rv = __getsetRhPortStat(fd, 25, p, sizeof(u8), (u8*)s, sizeof(u32), 2, 0);

done:
    IOSFree(p);
    IOSFree(s);
    return rv;
}

IOSError IUSB_DeviceRemovalNotifyAsync(IOSFd fd, IOSIpcCb cb, void* cbArg) {
    IOSError rv;

    USB_LOG("DeviceRemovalNotifyAsync\n");
    rv = IOS_IoctlAsync(fd, 26, 0, 0, 0, 0, cb, cbArg);
    return rv;
}

static IOSError __checkIsoArgs(IsoTransfer* xfer, u16* buflen) {
    IOSError rv = IOS_ERROR_INVALID;

    if (NULL == xfer || NULL == xfer->buf || 0 == xfer->numPackets || xfer->numPackets > 8 || NULL == xfer->packets) {
        goto out;
    }
    {
        u32 i;
        *buflen = 0;
        for (i = 0; i < xfer->numPackets; ++i) {
            if (xfer->packets[i] > 1023) {
                USB_ERR("packet %u too big: %u\n", i, xfer->packets[i]);
                goto out;
            }
            *buflen += xfer->packets[i];
        }
    }
    rv = IOS_ERROR_OK;

out:
    return rv;
}

IOSError IUSB_IsoMsgAsync(IOSFd fd, u8 ep, IsoTransfer* xfer, USBIsoCallbackFunc cb, void* cbArg) {
    IOSError rv = IOS_ERROR_OK;
    u8 *sep, *snumPackets;
    u16* sbuflen;
    IOSIoVector* vector;
    iusb_ctxt* req;
    u16 buflen;

    if (0 == ep || NULL == cb || (__checkIsoArgs(xfer, &buflen) < 0)) {
        USB_ERR("Invalid parameters for ISO transfer request\n");
        rv = IOS_ERROR_INVALID;
        goto out;
    }

    vector = IOSAlloc(ROUNDUP(sizeof(IOSIoVector)) * 5);
    sep = IOSAlloc(ROUNDUP(sizeof(u8)));
    sbuflen = IOSAlloc(ROUNDUP(sizeof(u16)));
    snumPackets = IOSAlloc(ROUNDUP(sizeof(u8)));
    req = IOSAlloc(ROUNDUP(sizeof(*req)));
    if (NULL == vector || NULL == sep || NULL == sbuflen || NULL == req) {
        USB_ERR("IUSB_IsoMsgAsync: Not enough memory\n");
        rv = IOS_ERROR_FAIL_ALLOC;
        IOSFree(req);
        goto clean;
    }

    memset(req, 0, sizeof(iusb_ctxt));
    *sep = ep;
    *sbuflen = buflen;
    *snumPackets = xfer->numPackets;

    vector[0].base = (u8*)sep;
    vector[0].length = sizeof(u8);
    vector[1].base = (u8*)sbuflen;
    vector[1].length = sizeof(u16);
    vector[2].base = snumPackets;
    vector[2].length = sizeof(u8);
    vector[3].base = (u8*)xfer->packets;
    vector[3].length = sizeof(u16) * xfer->numPackets;
    vector[4].base = xfer->buf;
    vector[4].length = buflen;

    DCFlushRange(sep, ROUNDUP(sizeof(u8)));
    DCFlushRange(sbuflen, ROUNDUP(sizeof(u16)));
    DCFlushRange(snumPackets, ROUNDUP(sizeof(u8)));
    DCFlushRange(vector, ROUNDUP(sizeof(IOSIoVector)) * 5);

    req->icb = cb;
    req->xfer = xfer;
    req->cbArg = cbArg;
    req->nclean = 4;
    req->clean[0] = sep;
    req->clean[1] = sbuflen;
    req->clean[2] = snumPackets;
    req->clean[3] = vector;

    rv = IOS_IoctlvAsync(fd, 9, 3, 2, vector, _intrBlkCtrlIsoCb, req);
    if (rv < 0) {
        IOSFree(req);
        goto clean;
    }
    goto out;

clean:
    IOSFree(sep);
    IOSFree(sbuflen);
    IOSFree(snumPackets);
    IOSFree(vector);

out:
    return rv;
}

IOSError IUSB_DeviceInsertionNotifyAsync(const char* path, u16 vid, u16 pid, USBCallbackFunc cb, void* cbArg) {
    IOSError rv;
    IOSFd fd;
    IOSIoVector* vector;
    u16 *svid, *spid;
    iusb_ctxt* req;

    if (NULL == path || 0 == vid || 0 == pid) {
        rv = IOS_ERROR_INVALID;
        goto done;
    }
    if ((fd = IOS_Open(path, 0)) < 0) {
        rv = fd;
        USB_ERR("Open(%s) failed\n", path);
        goto done;
    }

    vector = IOSAlloc(ROUNDUP(sizeof(IOSIoVector)) * 2);
    spid = IOSAlloc(ROUNDUP(sizeof(u16)));
    svid = IOSAlloc(ROUNDUP(sizeof(u16)));
    req = IOSAlloc(ROUNDUP(sizeof(*req)));
    if (NULL == vector || NULL == spid || NULL == svid || NULL == req) {
        USB_ERR("getDeviceList: Not enough memory\n");
        rv = IOS_ERROR_FAIL_ALLOC;
        goto clean;
    }

    memset(req, 0, sizeof(iusb_ctxt));
    memset(req, 0, sizeof(iusb_ctxt));
    *svid = vid;
    *spid = pid;

    vector[0].base = (u8*)svid;
    vector[0].length = sizeof(u16);
    vector[1].base = (u8*)spid;
    vector[1].length = sizeof(u16);
    DCFlushRange(svid, ROUNDUP(sizeof(u16)));
    DCFlushRange(spid, ROUNDUP(sizeof(u16)));
    DCFlushRange(vector, ROUNDUP(sizeof(IOSIoVector)) * 2);
    req->cb = cb;
    req->cbArg = cbArg;
    req->nclean = 3;
    req->clean[0] = svid;
    req->clean[1] = spid;
    req->clean[2] = vector;

    rv = IOS_IoctlvAsync(fd, 27, 2, 0, vector, _intrBlkCtrlIsoCb, req);
    if (rv < 0) {
        goto clean;
    }
    IOS_Close(fd);
    goto done;

clean:
    IOSFree(svid);
    IOSFree(spid);
    IOSFree(vector);
    IOSFree(req);

done:
    return rv;
}

IOSError IUSB_DeviceClassInsertionNotifyAsync(const char* path, u8 devClass, USBCallbackFunc cb, void* cbArg) {
    IOSError rv;
    IOSFd fd;
    IOSIoVector* vector;
    u8* sdevClass;
    iusb_ctxt* req;

    if (NULL == path || 0 == devClass) {
        OSReport("Invalid path or devClass in insertion notification call\n");
        rv = IOS_ERROR_INVALID;
        goto done;
    }
    if ((fd = IOS_Open(path, 0)) < 0) {
        rv = fd;
        OSReport("Open(%s) failed\n", path);
        goto done;
    }

    vector = IOSAlloc(ROUNDUP(sizeof(IOSIoVector)) * 2);
    sdevClass = IOSAlloc(ROUNDUP(sizeof(u8)));
    req = IOSAlloc(ROUNDUP(sizeof(*req)));
    *sdevClass = devClass;

    vector[0].base = (u8*)sdevClass;
    vector[0].length = sizeof(u8);
    DCFlushRange(sdevClass, ROUNDUP(sizeof(u8)));
    DCFlushRange(vector, ROUNDUP(sizeof(IOSIoVector)) * 2);
    req->cb = cb;
    req->cbArg = cbArg;
    req->nclean = 2;
    req->clean[0] = sdevClass;
    req->clean[1] = vector;

    rv = IOS_IoctlvAsync(fd, 28, 1, 0, vector, _intrBlkCtrlIsoCb, req);
    IOS_Close(fd);

done:
    return rv;
}

const char* const gUsbStrippedInsertionMessages[] = {
    "IUSB_RegisterInsertionNotifyWithIdAsync",
    "%s: Not enough memory\n",
    "Failed to open %s: %d\n"
};
