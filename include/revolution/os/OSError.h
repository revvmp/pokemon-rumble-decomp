#ifndef REVOLUTION_OS_OSERROR_H
#define REVOLUTION_OS_OSERROR_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct OSContext;

typedef u16 OSError;
typedef void (*OSErrorHandler)(OSError error, struct OSContext* context, ...);

#define OS_ERROR_PROTECTION 15

extern OSErrorHandler __OSErrorTable[];

void OSReport(const char* msg, ...);
void __OSUnhandledException(u8 exception, struct OSContext* context, u32 dsisr, u32 dar);
void OSPanic(const char* file, int line, const char* msg, ...);

#ifdef __cplusplus
}
#endif

#endif
