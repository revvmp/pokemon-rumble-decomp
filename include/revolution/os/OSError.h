#ifndef REVOLUTION_OS_OSERROR_H
#define REVOLUTION_OS_OSERROR_H

#include "revolution/types.h"
#include "revolution/os/OSContext.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef u16 OSError;
typedef void (*OSErrorHandler)(OSError error, OSContext* pContext, ...);

#define OS_ERROR_PROTECTION 15

extern OSErrorHandler __OSErrorTable[];

OSErrorHandler OSSetErrorHandler(OSError error, OSErrorHandler handler);
void OSReport(const char* msg, ...);
void __OSUnhandledException(u8 exception, OSContext* context, u32 dsisr, u32 dar);
void OSPanic(const char* file, int line, const char* msg, ...);

#ifdef __cplusplus
}
#endif

#endif
