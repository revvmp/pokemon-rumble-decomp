#ifndef REVOLUTION_OS_OSERROR_H
#define REVOLUTION_OS_OSERROR_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

void OSReport(const char* msg, ...);

#ifdef __cplusplus
}
#endif

#endif
