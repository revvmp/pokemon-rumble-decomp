#include <revolution/os/OSInterrupt.h>
#include "utils/gc/MWCriticalSection_gc.h"

void MWInitializeCriticalSection(unsigned int* section) {}

void MWEnterCriticalSection(unsigned int* section) {
    *section = OSDisableInterrupts();
}

void MWExitCriticalSection(unsigned int* section) {
    OSRestoreInterrupts(*section);
}
