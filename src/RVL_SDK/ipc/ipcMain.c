#include "RVL_SDK/ipc/ipcMain.h"

#define TRUE 1
#define FALSE 0
volatile u32 __IPCRegs[4] : 0xCD000000;

extern void* __OSGetIPCBufferHi(void);
extern void* __OSGetIPCBufferLo(void);

static void* IPCBufferHi;
static void* IPCBufferLo;

static void* IPCCurrentBufferHi;
static void* IPCCurrentBufferLo;

static u8 Initialized;

void IPCInit(void) {
    if (Initialized) {
        return;
    }

    IPCBufferHi = __OSGetIPCBufferHi();
    IPCBufferLo = __OSGetIPCBufferLo();
    IPCCurrentBufferHi = IPCBufferHi;
    IPCCurrentBufferLo = IPCBufferLo;

    Initialized = TRUE;
}

void IPCReInit(void) {
    Initialized = FALSE;
    IPCInit();
}

u32 IPCReadReg(u32 regIdx) {
    u32 reg = __IPCRegs[regIdx];
    return reg;
}

void IPCWriteReg(u32 regIdx, u32 data) {
    __IPCRegs[regIdx] = data;
}

void* IPCGetBufferHi(void) {
    return IPCCurrentBufferHi;
}

void* IPCGetBufferLo(void) {
    return IPCCurrentBufferLo;
}

void IPCSetBufferLo(void* lo) {
    IPCCurrentBufferLo = lo;
}
