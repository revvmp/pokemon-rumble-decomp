#ifndef REVOLUTION_OS_OSRTC_H
#define REVOLUTION_OS_OSRTC_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

BOOL __OSReadROM(void* buffer, s32 length, s32 offset);

#ifdef __cplusplus
}
#endif

#endif
