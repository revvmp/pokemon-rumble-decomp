#ifndef REVOLUTION_MEM_HEAPCOMMON_H
#define REVOLUTION_MEM_HEAPCOMMON_H

#include <string.h>
#include "revolution/types.h"
#include "revolution/mem/list.h"
#include "revolution/os/OSMutex.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MEM_HEAP_OPT_0_CLEAR (1 << 0)
#define MEM_HEAP_OPT_DEBUG_FILL (1 << 1)
#define MEM_HEAP_OPT_THREAD_SAFE (1 << 2)

typedef struct MEMiHeapHead MEMiHeapHead;

struct MEMiHeapHead {
    u32 signature;
    MEMLink link;
    MEMList childList;
    void* heapStart;
    void* heapEnd;
    OSMutex mutex;

    union {
        u32 val;

        struct {
            u32 reserved : 24;
            u32 optFlag : 8;
        } fields;
    } attribute;
};

typedef MEMiHeapHead* MEMHeapHandle;

typedef u32 UIntPtr;

static inline UIntPtr GetUIntPtr(const void* ptr) {
    return (UIntPtr)ptr;
}

static inline u32 GetOffsetFromPtr(const void* start, const void* end) {
    return GetUIntPtr(end) - GetUIntPtr(start);
}

static inline void* AddU32ToPtr(void* ptr, u32 val) {
    return (void*)(GetUIntPtr(ptr) + val);
}

static inline void* SubU32ToPtr(void* ptr, u32 val) {
    return (void*)(GetUIntPtr(ptr) - val);
}

static inline int ComparePtr(const void* a, const void* b) {
    const u8* wa = (const u8*)a;
    const u8* wb = (const u8*)b;

    return wa - wb;
}

#define RoundUp(value, alignment) (((value) + ((alignment) - 1)) & ~((alignment) - 1))
#define RoundUpPtr(ptr, alignment) ((void*)RoundUp(GetUIntPtr(ptr), (alignment)))
#define RoundDown(value, alignment) ((value) & ~((alignment) - 1))
#define RoundDownPtr(ptr, alignment) ((void*)RoundDown(GetUIntPtr(ptr), (alignment)))

static inline void SetOptForHeap(MEMiHeapHead* pHeapHd, u16 optFlag) {
    pHeapHd->attribute.fields.optFlag = (u8)optFlag;
}

static inline u16 GetOptForHeap(const MEMiHeapHead* pHeapHd) {
    return (u16)pHeapHd->attribute.fields.optFlag;
}

static inline void LockHeap(MEMiHeapHead* pHeapHd) {
    if (GetOptForHeap(pHeapHd) & MEM_HEAP_OPT_THREAD_SAFE) {
        OSLockMutex(&pHeapHd->mutex);
    }
}

static inline void UnlockHeap(MEMiHeapHead* pHeapHd) {
    if (GetOptForHeap(pHeapHd) & MEM_HEAP_OPT_THREAD_SAFE) {
        OSUnlockMutex(&pHeapHd->mutex);
    }
}

static inline void FillAllocMemory(MEMiHeapHead* pHeapHd, void* address, u32 size) {
    if (GetOptForHeap(pHeapHd) & MEM_HEAP_OPT_0_CLEAR) {
        (void)memset(address, 0, size);
    }
}

void MEMiInitHeapHead(MEMiHeapHead* pHeapHd, u32 signature, void* heapStart, void* heapEnd, u16 optFlag);
void MEMiFinalizeHeap(MEMiHeapHead* pHeapHd);
MEMHeapHandle MEMFindContainHeap(const void* memBlock);

#ifdef __cplusplus
}
#endif

#endif
