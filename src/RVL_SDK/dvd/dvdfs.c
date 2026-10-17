#include "revolution/dvd.h"
#include "revolution/os.h"
#include "revolution/os/OSBootInfo.h"
#include "revolution/os/OSThread.h"

extern u32  __DVDLayoutFormat;

typedef struct FSTEntry FSTEntry;

struct FSTEntry {
    u32 isDirAndStringOff;
    u32 parentOrPosition;
    u32 nextEntryOrLength;
};

static OSBootInfo* BootInfo;
static FSTEntry* FstStart;
static char* FstStringStart;
static u32 MaxEntryNum;

OSThreadQueue __DVDThreadQueue;
u32 __DVDLongFileNameFlag = 1;

#define entryIsDir(i)   (((FstStart[i].isDirAndStringOff & 0xFF000000) == 0) ? FALSE : TRUE)
#define filePosition(i) (FstStart[i].parentOrPosition)
#define fileLength(i)   (FstStart[i].nextEntryOrLength)

void __DVDFSInit(void) {
    BootInfo = (OSBootInfo*)OSPhysicalToCached(0);
    FstStart = (FSTEntry*)BootInfo->FSTLocation;

    if (FstStart != NULL) {
        MaxEntryNum = FstStart[0].nextEntryOrLength;
        FstStringStart = (char*)&FstStart[MaxEntryNum];
    }
}

BOOL DVDFastOpen(s32 entrynum, DVDFileInfo* fileInfo) {
    if ((entrynum < 0) || (entrynum >= MaxEntryNum) || entryIsDir(entrynum)) {
        return FALSE;
    }

    fileInfo->startAddr = filePosition(entrynum) >> __DVDLayoutFormat;
    fileInfo->length = fileLength(entrynum);
    fileInfo->callback = (DVDCallback)NULL;
    fileInfo->cb.state = 0;
    return TRUE;
}

BOOL DVDClose(DVDFileInfo* fileInfo) {
    DVDCancel(&fileInfo->cb);
    return TRUE;
}

static void cbForReadSync(s32 result, DVDCommandBlock* block);

s32 DVDReadPrio(DVDFileInfo* fileInfo, void* addr, s32 length, s32 offset, s32 prio) {
    BOOL result, enabled;
    DVDCommandBlock* block;
    s32 state, retVal;

    if (!((0 <= offset) && (offset <= fileInfo->length))) {
        OSPanic(__FILE__, 0x37D, "DVDRead(): specified area is out of the file  ");
    }

    if (!((0 <= offset + length) && (offset + length < fileInfo->length + 32))) {
         OSPanic(__FILE__, 0x383, "DVDRead(): specified area is out of the file  ");
    }

    block = &fileInfo->cb;
    result = DVDReadAbsAsyncPrio(block, addr, length, (u32)(fileInfo->startAddr + (offset >> 2)), cbForReadSync, prio);
    
    if (result == FALSE) {
        return -1;
    }

    enabled = OSDisableInterrupts();

    for (;;) {
        state = ((DVDCommandBlock*)block)->state;

        if (state == 0) {
            retVal = (s32)block->transferredSize;
            break;
        }

        if (state == -1) {
            retVal = -1;
            break;
        }

        if (state == 10) {
            retVal = -3;
            break;
        }

        OSSleepThread(&__DVDThreadQueue);
    }

    OSRestoreInterrupts(enabled);
    return retVal;
}

static void cbForReadSync(s32 result, DVDCommandBlock* block) {
    OSWakeupThread(&__DVDThreadQueue);
}

void* DVDGetFSTLocation(void) {
    return BootInfo->FSTLocation;
}
