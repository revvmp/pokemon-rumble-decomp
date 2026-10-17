#include "revolution/mem/frameHeap.h"

#define MEMi_FRMHEAP_SIGNATURE 'FRMH'

static inline MEMiFrmHeapHead* GetFrmHeapHeadPtrFromHeapHead_(MEMiHeapHead* pHHead) {
    return (MEMiFrmHeapHead*)AddU32ToPtr(pHHead, sizeof(MEMiHeapHead));
}

static inline MEMiHeapHead* GetHeapHeadPtrFromFrmHeapHead_(MEMiFrmHeapHead* pFrmHeapHd) {
    return (MEMiHeapHead*)SubU32ToPtr(pFrmHeapHd, sizeof(MEMiHeapHead));
}

static inline MEMiFrmHeapHead* GetFrmHeapHeadPtrFromHandle_(MEMHeapHandle heap) {
    return GetFrmHeapHeadPtrFromHeapHead_(heap);
}

static inline MEMiHeapHead* InitFrameHeap_(void* startAddress, void* endAddress, u16 optFlag) {
    MEMiHeapHead* pHeapHd = (MEMiHeapHead*)startAddress;
    MEMiFrmHeapHead* pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead_(pHeapHd);

    MEMiInitHeapHead(pHeapHd, MEMi_FRMHEAP_SIGNATURE, AddU32ToPtr(pFrmHeapHd, sizeof(MEMiFrmHeapHead)), endAddress, optFlag);

    pFrmHeapHd->headAllocator = pHeapHd->heapStart;
    pFrmHeapHd->tailAllocator = pHeapHd->heapEnd;
    pFrmHeapHd->pState = NULL;

    return pHeapHd;
}

static inline void* AllocFromHead_(MEMiFrmHeapHead* pFrmHeapHd, u32 size, int alignment) {
    void* newBlock = RoundUpPtr(pFrmHeapHd->headAllocator, alignment);
    void* endAddress = AddU32ToPtr(newBlock, size);

    if (GetUIntPtr(endAddress) > GetUIntPtr(pFrmHeapHd->tailAllocator)) {
        return NULL;
    }

    FillAllocMemory(GetHeapHeadPtrFromFrmHeapHead_(pFrmHeapHd), pFrmHeapHd->headAllocator, GetOffsetFromPtr(pFrmHeapHd->headAllocator, endAddress));
    pFrmHeapHd->headAllocator = endAddress;

    return newBlock;
}

static inline void* AllocFromTail_(MEMiFrmHeapHead* pFrmHeapHd, u32 size, int alignment) {
    void* newBlock = RoundDownPtr(SubU32ToPtr(pFrmHeapHd->tailAllocator, size), alignment);

    if (GetUIntPtr(newBlock) < GetUIntPtr(pFrmHeapHd->headAllocator)) {
        return NULL;
    }

    FillAllocMemory(GetHeapHeadPtrFromFrmHeapHead_(pFrmHeapHd), newBlock, GetOffsetFromPtr(newBlock, pFrmHeapHd->tailAllocator));
    pFrmHeapHd->tailAllocator = newBlock;

    return newBlock;
}

static inline void FreeHead_(MEMiHeapHead* pHeapHd) {
    MEMiFrmHeapHead* pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead_(pHeapHd);

    pFrmHeapHd->headAllocator = pHeapHd->heapStart;
    pFrmHeapHd->pState = NULL;
}

static inline void FreeTail_(MEMiHeapHead* pHeapHd) {
    MEMiFrmHeapHead* pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead_(pHeapHd);
    MEMiFrmHeapState* pState;

    for (pState = pFrmHeapHd->pState; pState != NULL; pState = pState->pPrevState) {
        pState->tailAllocator = pHeapHd->heapEnd;
    }

    pFrmHeapHd->tailAllocator = pHeapHd->heapEnd;
}

MEMHeapHandle MEMCreateFrmHeapEx(void* startAddress, u32 size, u16 optFlag) {
    void* endAddress = RoundDownPtr(AddU32ToPtr(startAddress, size), 4);

    startAddress = RoundUpPtr(startAddress, 4);

    if (GetUIntPtr(startAddress) > GetUIntPtr(endAddress) || GetOffsetFromPtr(startAddress, endAddress) < sizeof(MEMiHeapHead) + sizeof(MEMiFrmHeapHead)) {
        return NULL;
    }

    return InitFrameHeap_(startAddress, endAddress, optFlag);
}

void* MEMDestroyFrmHeap(MEMHeapHandle heap) {
    MEMiFinalizeHeap(heap);
    return (void*)heap;
}

void* MEMAllocFromFrmHeapEx(MEMHeapHandle heap, u32 size, int alignment) {
    void* memory = NULL;
    MEMiFrmHeapHead* pFrmHeapHd = GetFrmHeapHeadPtrFromHandle_(heap);

    if (size == 0) {
        size = 1;
    }

    size = RoundUp(size, 4);

    LockHeap(heap);

    if (alignment >= 0) {
        memory = AllocFromHead_(pFrmHeapHd, size, alignment);
    } else {
        memory = AllocFromTail_(pFrmHeapHd, size, -alignment);
    }

    UnlockHeap(heap);

    return memory;
}

void MEMFreeToFrmHeap(MEMHeapHandle heap, int mode) {
    LockHeap(heap);

    if (mode & MEM_FRMHEAP_FREE_HEAD) {
        FreeHead_(heap);
    }

    if (mode & MEM_FRMHEAP_FREE_TAIL) {
        FreeTail_(heap);
    }

    UnlockHeap(heap);
}
