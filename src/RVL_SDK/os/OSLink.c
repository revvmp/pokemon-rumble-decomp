#include "revolution/os/OSLink.h"
#include "revolution/os/OSAddress.h"

#define __OSModuleList (*(OSModuleQueue*)OSPhysicalToCached(0x30C8))
#define __OSStringTable (*(const char**)OSPhysicalToCached(0x30D0))

void __OSModuleInit(void) {
    __OSModuleList.head = __OSModuleList.tail = NULL;
    __OSStringTable = NULL;
}
