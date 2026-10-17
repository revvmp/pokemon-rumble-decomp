#ifndef REVOLUTION_OS_OSEXCEPTION_H
#define REVOLUTION_OS_OSEXCEPTION_H

#include "revolution/os/OSContext.h"
#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef u8 __OSException;
typedef void (*__OSExceptionHandler)(__OSException exception, OSContext* context);

#define __OS_EXCEPTION_EXTERNAL_INTERRUPT 4
#define __OS_EXCEPTION_MEMORY_PROTECTION 15

__OSExceptionHandler __OSSetExceptionHandler(__OSException exception, __OSExceptionHandler handler);

#ifdef __cplusplus
}
#endif

#endif
