#ifndef REVOLUTION_MEM_EXPHEAP_H
#define REVOLUTION_MEM_EXPHEAP_H

#include "revolution/types.h"
#include "revolution/mem/heapCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MEM_EXPHEAP_ALLOC_MODE_FIRST 0
#define MEM_EXPHEAP_ALLOC_MODE_NEAR 1

#define MEM_EXPHEAP_ALLOC_DIR_FRONT 0
#define MEM_EXPHEAP_ALLOC_DIR_REAR 1

typedef struct MEMiExpHeapMBlockHead MEMiExpHeapMBlockHead;

struct MEMiExpHeapMBlockHead {
    u16 signature;

    union {
        u16 val;

        struct {
            u16 allocDir : 1;
            u16 alignment : 7;
            u16 groupID : 8;
        } fields;
    } attribute;

    u32 blockSize;
    MEMiExpHeapMBlockHead* pMBHeadPrev;
    MEMiExpHeapMBlockHead* pMBHeadNext;
};

typedef struct MEMiExpMBlockList {
    MEMiExpHeapMBlockHead* head;
    MEMiExpHeapMBlockHead* tail;
} MEMiExpMBlockList;

typedef struct MEMiExpHeapHead {
    MEMiExpMBlockList mbFreeList;
    MEMiExpMBlockList mbUsedList;
    u16 groupID;

    union {
        u16 val;

        struct {
            u16 reserved : 14;
            u16 useMarginOfAlign : 1;
            u16 allocMode : 1;
        } fields;
    } feature;
} MEMiExpHeapHead;

MEMHeapHandle MEMCreateExpHeapEx(void* startAddress, u32 size, u16 optFlag);
void* MEMDestroyExpHeap(MEMHeapHandle heap);
void* MEMAllocFromExpHeapEx(MEMHeapHandle heap, u32 size, int alignment);
void MEMFreeToExpHeap(MEMHeapHandle heap, void* memBlock);
u32 MEMGetAllocatableSizeForExpHeapEx(MEMHeapHandle heap, int alignment);
u16 MEMSetGroupIDForExpHeap(MEMHeapHandle heap, u16 groupID);

static inline MEMHeapHandle MEMCreateExpHeap(void* startAddress, u32 size) {
    return MEMCreateExpHeapEx(startAddress, size, 0);
}

#ifdef __cplusplus
}
#endif

#endif
