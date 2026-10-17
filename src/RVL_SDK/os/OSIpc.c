#include "revolution/os/OSIpc.h"
#include "revolution/types.h"

static void* IpcBufferHi;
static void* IpcBufferLo = (void*)0xFFFFFFFF;

void* __OSGetIPCBufferHi(void) {
    return IpcBufferHi;
}

void* __OSGetIPCBufferLo(void) {
    return IpcBufferLo;
}

void __OSInitIPCBuffer(void) {
    IpcBufferLo = (void*)*(u32*)0x80003130;
    IpcBufferHi = (void*)*(u32*)0x80003134;
}
