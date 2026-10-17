#ifndef REVOLUTION_OS_H
#define REVOLUTION_OS_H

#include "revolution/types.h"
#include "revolution/os/OSContext.h"
#include "revolution/os/OSError.h"
#include "revolution/os/OSInterrupt.h"
#include "revolution/os/OSThread.h"
#include "revolution/os/OSTime.h"

#define OS_BASE_CACHED 0x80000000
#define OS_BASE_UNCACHED 0xC0000000

#define OSPhysicalToCached(paddr) ((void*)((u32)(paddr) + OS_BASE_CACHED))
#define OSPhysicalToUncached(paddr) ((void*)((u32)(paddr) + OS_BASE_UNCACHED))
#define OSCachedToPhysical(caddr) ((u32)((u32)(caddr) - OS_BASE_CACHED))
#define OSUncachedToPhysical(ucaddr) ((u32)((u32)(ucaddr) & ~OS_BASE_UNCACHED))

#endif
