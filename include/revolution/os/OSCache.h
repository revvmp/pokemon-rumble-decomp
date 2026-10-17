#ifndef REVOLUTION_OS_OSCACHE_H
#define REVOLUTION_OS_OSCACHE_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

void DCInvalidateRange(void* addr, u32 nBytes);

#ifdef __cplusplus
}
#endif

#endif
