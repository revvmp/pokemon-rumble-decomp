#ifndef REVOLUTION_OS_OSLINK_H
#define REVOLUTION_OS_OSLINK_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OSModuleInfo OSModuleInfo;

typedef struct OSModuleQueue {
    OSModuleInfo* head;
    OSModuleInfo* tail;
} OSModuleQueue;

void __OSModuleInit(void);

#ifdef __cplusplus
}
#endif

#endif
