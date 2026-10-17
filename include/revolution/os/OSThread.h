#ifndef REVOLUTION_OS_OSTHREAD_H
#define REVOLUTION_OS_OSTHREAD_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OSThread OSThread;

typedef struct OSThreadQueue {
    OSThread* head;
    OSThread* tail;
} OSThreadQueue;

void OSInitThreadQueue(OSThreadQueue* queue);
void OSSleepThread(OSThreadQueue* queue);
void OSWakeupThread(OSThreadQueue* queue);
s32 OSDisableScheduler(void);
s32 OSEnableScheduler(void);
void __OSReschedule(void);

#ifdef __cplusplus
}
#endif

#endif
