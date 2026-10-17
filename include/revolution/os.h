#ifndef REVOLUTION_OS_H
#define REVOLUTION_OS_H

#include "revolution/types.h"
#include "revolution/os/OSCache.h"
#include "revolution/os/OSContext.h"
#include "revolution/os/OSError.h"
#include "revolution/os/OSInterrupt.h"
#include "revolution/os/OSThread.h"
#include "revolution/os/OSTime.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OS_BASE_CACHED 0x80000000
#define OSPhysicalToCached(paddr) ((void*)((u32)(paddr) + OS_BASE_CACHED))

typedef BOOL (*OSShutdownFunction)(BOOL final, u32 event);

typedef struct OSShutdownFunctionInfo OSShutdownFunctionInfo;

struct OSShutdownFunctionInfo {
    OSShutdownFunction func;
    u32 priority;
    OSShutdownFunctionInfo* next;
    OSShutdownFunctionInfo* prev;
};

#define OS_SHUTDOWN_EVENT_FATAL 0
#define OS_SHUTDOWN_EVENT_1 1
#define OS_SHUTDOWN_EVENT_SHUTDOWN 2
#define OS_SHUTDOWN_EVENT_3 3
#define OS_SHUTDOWN_EVENT_RESTART 4
#define OS_SHUTDOWN_EVENT_RETURN_TO_MENU 5
#define OS_SHUTDOWN_EVENT_LAUNCH_APP 6

extern BOOL __OSIsReturnToIdle;

void OSRegisterShutdownFunction(OSShutdownFunctionInfo* info);
void OSRegisterVersion(const char* id);
const char* OSGetAppGamename(void);
u8 OSGetAppType(void);

#ifdef __cplusplus
}
#endif

#endif
