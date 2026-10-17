#include "revolution/db.h"
#include "revolution/base/PPCArch.h"

DBInterface* __DBInterface = NULL;
BOOL DBVerbose;

void DBInit(void) {
    __DBInterface = (DBInterface*)OSPhysicalToCached(0x40);
    __DBInterface->ExceptionDestination = (void (*)(void))OSCachedToPhysical(__DBExceptionDestination);
    DBVerbose = TRUE;
}

void __DBExceptionDestinationAux(void) {
    u32* contextAddr = (u32*)0xC0;
    OSContext* context = (OSContext*)(0x80000000 + *contextAddr);

    OSReport("DBExceptionDestination\n");
    OSDumpContext(context);
    PPCHalt();
}

asm void __DBExceptionDestination(void) {
    nofralloc
    mfmsr r3
    ori r3, r3, 0x30
    mtmsr r3
    b __DBExceptionDestinationAux
}

BOOL __DBIsExceptionMarked(__OSException exception) {
    u32 mask = 1 << exception;
    return __DBInterface->exceptionMask & mask;
}

void DBPrintf(char* format, ...) {
}
