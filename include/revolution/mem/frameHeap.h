#ifndef REVOLUTION_MEM_FRAMEHEAP_H
#define REVOLUTION_MEM_FRAMEHEAP_H

#include "revolution/types.h"
#include "revolution/mem/heapCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MEM_FRMHEAP_FREE_HEAD (1 << 0)
#define MEM_FRMHEAP_FREE_TAIL (1 << 1)
#define MEM_FRMHEAP_FREE_ALL (MEM_FRMHEAP_FREE_HEAD | MEM_FRMHEAP_FREE_TAIL)

typedef struct MEMiFrmHeapState MEMiFrmHeapState;

struct MEMiFrmHeapState {
    u32 tagName;
    void* headAllocator;
    void* tailAllocator;
    MEMiFrmHeapState* pPrevState;
};

typedef struct MEMiFrmHeapHead {
    void* headAllocator;
    void* tailAllocator;
    MEMiFrmHeapState* pState;
} MEMiFrmHeapHead;

MEMHeapHandle MEMCreateFrmHeapEx(void* startAddress, u32 size, u16 optFlag);
void* MEMDestroyFrmHeap(MEMHeapHandle heap);
void* MEMAllocFromFrmHeapEx(MEMHeapHandle heap, u32 size, int alignment);
void MEMFreeToFrmHeap(MEMHeapHandle heap, int mode);

static inline MEMHeapHandle MEMCreateFrmHeap(void* startAddress, u32 size) {
    return MEMCreateFrmHeapEx(startAddress, size, 0);
}

static inline void* MEMAllocFromFrmHeap(MEMHeapHandle heap, u32 size) {
    return MEMAllocFromFrmHeapEx(heap, size, 4);
}

#ifdef __cplusplus
}
#endif

#endif
