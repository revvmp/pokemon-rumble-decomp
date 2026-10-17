#include "revolution/os.h"
#include "revolution/wpad.h"
#include <string.h>

#define WPAD_MEM_BLOCK_SIZE 16
#define WPAD_MEM_GAME_DATA 0x9A

static u16 _wpadGameTitle[17];

static void __wpadResultCallback(s32 chan, s32 result);
static void __wpadWriteCallback(s32 chan, s32 result);
static s32 __wpadSendWriteMemory(s32 chan, u16 len);
static void __wpadWriteGameDataSub(s32 chan, s32 result);
static void __wpadWriteCheck1(s32 chan, s32 result);
static void __wpadWriteCheck2(s32 chan, s32 result);

void WPADiClearMemBlock(s32 chan) {
    WPADControlBlock* p_wpd = _wpdcb[chan];

    p_wpd->memBlock.used = FALSE;
    p_wpd->memBlock.p_buf = NULL;
    p_wpd->memBlock.len = 0;
    p_wpd->memBlock.addr = 0;
    p_wpd->memBlock.callback = NULL;
}

static void __wpadResultCallback(s32 chan, s32 result) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    WPADCallback callback;

    p_wpd->memBlock.used = FALSE;
    callback = p_wpd->memBlock.callback;
    p_wpd->memBlock.callback = NULL;

    if (callback) {
        callback(chan, result);
    }
}

static void __wpadWriteCallback(s32 chan, s32 result) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    u16 len;

    if (p_wpd->memBlock.len == 0) {
        __wpadResultCallback(chan, result);
        return;
    }

    len = (u16)(p_wpd->memBlock.len > WPAD_MEM_BLOCK_SIZE ? WPAD_MEM_BLOCK_SIZE : p_wpd->memBlock.len);

    if (result == WPAD_ERR_NONE) {
        p_wpd->memBlock.len -= len;
        p_wpd->memBlock.addr += WPAD_MEM_BLOCK_SIZE;
        p_wpd->memBlock.p_buf += WPAD_MEM_BLOCK_SIZE;
        __wpadSendWriteMemory(chan, len);
    } else if (result == WPAD_ERR_BUSY) {
        __wpadSendWriteMemory(chan, len);
    } else {
        __wpadResultCallback(chan, result);
    }
}

static s32 __wpadSendWriteMemory(s32 chan, u16 len) {
    BOOL enabled;
    s32 status;
    WPADControlBlock* p_wpd = _wpdcb[chan];

    enabled = OSDisableInterrupts();
    status = WPADWriteMemoryAsync(chan, p_wpd->memBlock.p_buf, len, p_wpd->memBlock.addr, __wpadWriteCallback);
    OSRestoreInterrupts(enabled);

    return status;
}

static void __wpadWriteGameDataSub(s32 chan, s32 result) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    u16 len;

    if (p_wpd->gameInfoErr[1] == WPAD_ERR_NONE) {
        p_wpd->gameInfoErr[1] = result == WPAD_ERR_NONE ? WPAD_ERR_NONE : WPAD_ERR_TRANSFER;
    }

    if (result == WPAD_ERR_NONE) {
        if (p_wpd->gameInfoErr[0] == WPAD_ERR_NONE || p_wpd->gameInfoErr[1] == WPAD_ERR_NONE) {
            len = (u16)(p_wpd->memBlock.len > WPAD_MEM_BLOCK_SIZE ? WPAD_MEM_BLOCK_SIZE : p_wpd->memBlock.len);
            p_wpd->memBlock.len -= len;
            __wpadSendWriteMemory(chan, len);
            return;
        }

        result = WPAD_ERR_TRANSFER;
    }

    if (p_wpd->memBlock.callback) {
        p_wpd->memBlock.used = FALSE;
        p_wpd->memBlock.callback(chan, result);
        p_wpd->memBlock.callback = NULL;
    }
}

static void __wpadWriteCheck1(s32 chan, s32 result) {
    WPADControlBlock* p_wpd = _wpdcb[chan];

    if (p_wpd->gameInfoErr[0] == WPAD_ERR_NONE) {
        p_wpd->gameInfoErr[0] = result == WPAD_ERR_NONE ? WPAD_ERR_NONE : WPAD_ERR_TRANSFER;
    }
}

static void __wpadWriteCheck2(s32 chan, s32 result) {
    WPADControlBlock* p_wpd = _wpdcb[chan];

    if (p_wpd->gameInfoErr[1] == WPAD_ERR_NONE) {
        p_wpd->gameInfoErr[1] = result == WPAD_ERR_NONE ? WPAD_ERR_NONE : WPAD_ERR_TRANSFER;
    }
}

s32 WPADWriteGameData(s32 chan, void* p_buf, u16 len, u16 addr, WPADCallback callback) {
    BOOL enabled;
    WPADControlBlock* p_wpd = _wpdcb[chan];
    s32 status;
    BOOL setup;
    u8 i;

    enabled = OSDisableInterrupts();

    status = p_wpd->status;
    setup = p_wpd->setup;

    if (status == WPAD_ERR_NO_CONTROLLER) {
        goto end;
    }

    if (!setup) {
        status = WPAD_ERR_BUSY;
        goto end;
    }

    if (!p_wpd->memBlock.used) {
        if (!WPADiIsAvailableCmdQueue(&p_wpd->cmdq, 9)) {
            status = WPAD_ERR_BUSY;
            goto end;
        }

        p_wpd->memBlock.used = TRUE;
        p_wpd->memBlock.callback = callback;
        p_wpd->memBlock.len = len;
        p_wpd->memBlock.addr = (u32)(addr + WPAD_MEM_GAME_DATA);
        p_wpd->memBlock.p_buf = (u8*)p_buf;

        memcpy(p_wpd->gameInfo.gameId, WPADiGetGameCode(), sizeof(p_wpd->gameInfo.gameId));
        memcpy(p_wpd->gameInfo.gameTitle, _wpadGameTitle, sizeof(p_wpd->gameInfo.gameTitle));
        p_wpd->gameInfo.calendarTimeTick = OSGetTime();
        p_wpd->gameInfo.gameType = WPADiGetGameType();

        p_wpd->gameInfo.checkSum = 0;
        for (i = 0; i < 47; i++) {
            p_wpd->gameInfo.checkSum += ((u8*)&p_wpd->gameInfo)[i];
        }
        p_wpd->gameInfo.checkSum += 0x55;

        p_wpd->gameInfoErr[1] = p_wpd->gameInfoErr[0] = WPAD_ERR_NONE;

        WPADWriteMemoryAsync(chan, &p_wpd->gameInfo, 16, 0x2A, __wpadWriteCheck1);
        WPADWriteMemoryAsync(chan, (u8*)&p_wpd->gameInfo + 0x10, 16, 0x3A, __wpadWriteCheck1);
        WPADWriteMemoryAsync(chan, (u8*)&p_wpd->gameInfo + 0x20, 16, 0x4A, __wpadWriteCheck1);
        WPADWriteMemoryAsync(chan, (u8*)&p_wpd->gameInfo + 0x30, 8, 0x5A, __wpadWriteCheck1);
        WPADWriteMemoryAsync(chan, &p_wpd->gameInfo, 16, 0x62, __wpadWriteCheck2);
        WPADWriteMemoryAsync(chan, (u8*)&p_wpd->gameInfo + 0x10, 16, 0x72, __wpadWriteCheck2);
        WPADWriteMemoryAsync(chan, (u8*)&p_wpd->gameInfo + 0x20, 16, 0x82, __wpadWriteCheck2);
        WPADWriteMemoryAsync(chan, (u8*)&p_wpd->gameInfo + 0x30, 8, 0x92, __wpadWriteGameDataSub);

        OSRestoreInterrupts(enabled);
        return WPAD_ERR_NONE;
    } else {
        status = WPAD_ERR_BUSY;
    }

end:
    OSRestoreInterrupts(enabled);

    if (callback) {
        callback(chan, status);
    }

    return status;
}

s32 WPADReadGameData(s32 chan, void* p_buf, u16 len, u16 addr, WPADCallback callback) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    s32 status;
    BOOL enabled;
    BOOL setup;

    enabled = OSDisableInterrupts();

    status = p_wpd->status;
    setup = p_wpd->setup;

    if (status == WPAD_ERR_NO_CONTROLLER) {
        goto end;
    }

    if (!setup) {
        status = WPAD_ERR_BUSY;
        goto end;
    }

    if (!p_wpd->memBlock.used) {
        status = (p_wpd->gameInfoErr[0] == WPAD_ERR_NONE || p_wpd->gameInfoErr[1] == WPAD_ERR_NONE) ? WPAD_ERR_NONE
                                                                                                     : WPAD_ERR_BROKEN;
        if (status == WPAD_ERR_NONE) {
            if (memcmp(p_wpd->gameInfo.gameId, WPADiGetGameCode(), sizeof(p_wpd->gameInfo.gameId)) == 0) {
                p_wpd->memBlock.used = TRUE;
                p_wpd->memBlock.callback = callback;
                OSRestoreInterrupts(enabled);

                return WPADReadMemoryAsync(chan, p_buf, len, (u32)(addr + WPAD_MEM_GAME_DATA), __wpadResultCallback);
            } else {
                status = WPAD_ERR_NOPERM;
            }
        }
    } else {
        status = WPAD_ERR_BUSY;
    }

end:
    OSRestoreInterrupts(enabled);

    if (callback) {
        callback(chan, status);
    }

    return status;
}

s32 WPADReadMemoryAsync(s32 chan, void* p_buf, u16 len, u32 addr, WPADCallback callback) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL success;
    BOOL enabled;
    BOOL setup;
    s32 status = WPAD_ERR_NONE;

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

    success = WPADiSendReadData(&p_wpd->cmdq, p_buf, len, addr, callback);
    status = success ? WPAD_ERR_NONE : WPAD_ERR_BUSY;

end:
    if (status != WPAD_ERR_NONE && callback) {
        callback(chan, status);
    }

    return status;
}

s32 WPADWriteMemoryAsync(s32 chan, void* p_buf, u16 len, u32 addr, WPADCallback callback) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL success;
    BOOL enabled;
    BOOL setup;
    s32 status = WPAD_ERR_NONE;

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

    success = WPADiSendWriteData(&p_wpd->cmdq, p_buf, len, addr, callback);
    status = success ? WPAD_ERR_NONE : WPAD_ERR_BUSY;

end:
    if (status != WPAD_ERR_NONE && callback) {
        callback(chan, status);
    }

    return status;
}

s32 WPADGetGameTitleUtf16(s32 chan, u16** title) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL enabled;
    s32 status;

    enabled = OSDisableInterrupts();

    status = (p_wpd->gameInfoErr[0] == WPAD_ERR_NONE || p_wpd->gameInfoErr[1] == WPAD_ERR_NONE) ? WPAD_ERR_NONE
                                                                                                 : WPAD_ERR_INVALID;
    if (status == WPAD_ERR_NONE) {
        *title = p_wpd->gameInfo.gameTitle;
    } else {
        *title = NULL;
    }

    OSRestoreInterrupts(enabled);
    return status;
}

void WPADSetGameTitleUtf16(const u16* title) {
    int i;

    for (i = 0; i < 16; i++) {
        _wpadGameTitle[i] = title[i];
        if (title[i] == 0) {
            break;
        }
    }

    _wpadGameTitle[16] = 0;
}

s32 WPADWriteExtReg(s32 chan, void* p_buf, u16 len, u16 addr, WPADCallback callback) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL success;
    BOOL enabled;
    BOOL setup;
    s32 status = WPAD_ERR_NONE;

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

    success = WPADiSendWriteData(&p_wpd->cmdq, p_buf, len, addr | 0x04A40000, callback);
    status = success ? WPAD_ERR_NONE : WPAD_ERR_BUSY;

end:
    if (status != WPAD_ERR_NONE && callback) {
        callback(chan, status);
    }

    return status;
}
