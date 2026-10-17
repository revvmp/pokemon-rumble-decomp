#ifndef REVOLUTION_OS_OSTIME_H
#define REVOLUTION_OS_OSTIME_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef s64 OSTime;

OSTime OSGetTime(void);

#ifdef __cplusplus
}
#endif

#endif
