#ifndef REVOLUTION_OS_OSRESET_H
#define REVOLUTION_OS_OSRESET_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef BOOL (*OSShutdownFunction)(BOOL final, u32 event);

typedef struct OSShutdownFunctionInfo OSShutdownFunctionInfo;

struct OSShutdownFunctionInfo {
    OSShutdownFunction func;
    u32 priority;
    OSShutdownFunctionInfo* next;
    OSShutdownFunctionInfo* prev;
};

void OSRegisterShutdownFunction(OSShutdownFunctionInfo* info);

#ifdef __cplusplus
}
#endif

#endif
