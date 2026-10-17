#include "revolution/mem/allocator.h"
#include "revolution/mem/expHeap.h"

static void* AllocatorAllocForExpHeap_(MEMAllocator* pAllocator, u32 size) {
    return MEMAllocFromExpHeapEx(pAllocator->pHeap, size, (int)pAllocator->heapParam1);
}

static void AllocatorFreeForExpHeap_(MEMAllocator* pAllocator, void* memBlock) {
    MEMFreeToExpHeap(pAllocator->pHeap, memBlock);
}

void* MEMAllocFromAllocator(MEMAllocator* pAllocator, u32 size) {
    return (*pAllocator->pFunc->pfAlloc)(pAllocator, size);
}

void MEMFreeToAllocator(MEMAllocator* pAllocator, void* memBlock) {
    (*pAllocator->pFunc->pfFree)(pAllocator, memBlock);
}

void MEMInitAllocatorForExpHeap(MEMAllocator* pAllocator, MEMHeapHandle heap, int alignment) {
    static const MEMAllocatorFunc sAllocatorFunc = {
        AllocatorAllocForExpHeap_,
        AllocatorFreeForExpHeap_,
    };

    pAllocator->pFunc = &sAllocatorFunc;
    pAllocator->pHeap = heap;
    pAllocator->heapParam1 = (u32)alignment;
    pAllocator->heapParam2 = 0;
}
