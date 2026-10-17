#include <revolution/base/PPCArch.h>
#include <revolution/os/OSThread.h>
#include <revolution/os/OSCache.h>
#include <revolution/os/OSMutex.h>
#include "revolution/os.h"
#include <string.h>

static void __OSSystemCallVectorStart(void);
static void __OSSystemCallVectorEnd(void);
void __OSInitSystemCall(void);

static asm void SystemCallVector(void) {
    nofralloc

entry __OSSystemCallVectorStart
    mfspr r9, hid0
    ori r10, r9, 8
    mtspr hid0, r10
    isync
    sync
    mtspr hid0, r9
    rfi

entry __OSSystemCallVectorEnd
    nop
}

void __OSInitSystemCall(void) {
    void* addr;
    addr = (void*)OSPhysicalToCached(0xC00);
    memcpy(addr, __OSSystemCallVectorStart, (u32) __OSSystemCallVectorEnd - (u32) __OSSystemCallVectorStart);
    DCFlushRangeNoSync(addr, 0x100);
    __sync();
    ICInvalidateRange(addr, 0x100);
}
