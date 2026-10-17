#ifndef REVOLUTION_OS_OSTHREAD_H
#define REVOLUTION_OS_OSTHREAD_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OSThread OSThread;
typedef struct OSThreadQueue OSThreadQueue;

struct OSThreadQueue {
    OSThread* head;
    OSThread* tail;
};

void OSInitThreadQueue(OSThreadQueue* queue);
void OSSleepThread(OSThreadQueue* queue);
void OSWakeupThread(OSThreadQueue* queue);

#ifdef __cplusplus
}
#endif

#endif
