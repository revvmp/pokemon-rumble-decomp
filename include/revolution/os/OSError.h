#ifndef REVOLUTION_OS_OSERROR_H
#define REVOLUTION_OS_OSERROR_H

#include "revolution/types.h"
#include "revolution/os/OSContext.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef u16 OSError;
typedef void (*OSErrorHandler)(OSError error, OSContext* pContext, ...);

OSErrorHandler OSSetErrorHandler(OSError error, OSErrorHandler handler);
void OSReport(const char* msg, ...);
void OSPanic(const char* file, int line, const char* msg, ...);

#ifdef __cplusplus
}
#endif

#endif
