#ifndef REVOLUTION_OS_OSADDRESS_H
#define REVOLUTION_OS_OSADDRESS_H

#include "revolution/types.h"

#define OS_BASE_CACHED 0x80000000

#define OSCachedToPhysical(caddr) ((u32)((u8*)(caddr) - OS_BASE_CACHED))

#endif
