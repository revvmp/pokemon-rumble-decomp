#ifndef REVOLUTION_OS_OSALLOC_H
#define REVOLUTION_OS_OSALLOC_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int OSHeapHandle;

extern volatile OSHeapHandle __OSCurrHeap;

void* OSAllocFromHeap(OSHeapHandle heap, u32 size);
void OSFreeToHeap(OSHeapHandle heap, void* ptr);

#define OSAlloc(size) OSAllocFromHeap(__OSCurrHeap, (size))

#ifdef __cplusplus
}
#endif

#endif
