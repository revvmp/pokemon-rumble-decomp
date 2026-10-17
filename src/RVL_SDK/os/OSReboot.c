#include "revolution/os/OSArena.h"
#include "revolution/os/OSExecParams.h"
#include "revolution/os/OSInterrupt.h"

extern void __OSBootDol(u32 dolOffset, u32 resetCode, const char** argv);

extern u32 __OSNextPartitionType;
vu32 __OSLaunchPartitionType : 0x80003194;
static void* SaveStart = NULL;
static void* SaveEnd = NULL;

void __OSReboot(u32 resetCode, u32 bootDol) {
    const char* argvToPass;
    OSDisableInterrupts();
    OSSetArenaLo((void*)0x81280000);
    OSSetArenaHi((void*)0x812F0000);
    argvToPass = NULL;
    __OSNextPartitionType = __OSLaunchPartitionType;
    __OSBootDol(bootDol, resetCode | 0x80000000, &argvToPass);
}

void OSGetSaveRegion(void** start, void** end) {
    *start = SaveStart;
    *end = SaveEnd;
}
