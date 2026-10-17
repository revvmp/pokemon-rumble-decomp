#ifndef REVOLUTION_OS_OSMUTEX_H
#define REVOLUTION_OS_OSMUTEX_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OSThread OSThread;

typedef struct OSThreadQueue {
    OSThread* head;
    OSThread* tail;
} OSThreadQueue;

typedef struct OSMutex OSMutex;

typedef struct OSMutexLink {
    OSMutex* next;
    OSMutex* prev;
} OSMutexLink;

struct OSMutex {
    OSThreadQueue queue;
    OSThread* thread;
    s32 count;
    OSMutexLink link;
};

void OSInitMutex(OSMutex* mutex);
void OSLockMutex(OSMutex* mutex);
void OSUnlockMutex(OSMutex* mutex);

#ifdef __cplusplus
}
#endif

#endif
