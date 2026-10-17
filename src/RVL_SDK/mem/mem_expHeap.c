#include "revolution/mem/expHeap.h"
#include "revolution/os/OSInterrupt.h"

#define MBLOCK_FREE_SIGNATURE 'FR'
#define MBLOCK_USED_SIGNATURE 'UD'
#define MEMi_EXPHEAP_SIGNATURE 'EXPH'
#define MIN_FREE_BLOCK_SIZE 4

typedef struct MemRegion {
    void* start;
    void* end;
} MemRegion;

static inline MEMiExpHeapHead* GetExpHeapHeadPtrFromHeapHead_(MEMiHeapHead* pHHead) {
    return (MEMiExpHeapHead*)AddU32ToPtr(pHHead, sizeof(MEMiHeapHead));
}

static inline MEMiHeapHead* GetHeapHeadPtrFromExpHeapHead_(MEMiExpHeapHead* pEHHead) {
    return (MEMiHeapHead*)SubU32ToPtr(pEHHead, sizeof(MEMiHeapHead));
}

static inline MEMiExpHeapHead* GetExpHeapHeadPtrFromHandle_(MEMHeapHandle heap) {
    return GetExpHeapHeadPtrFromHeapHead_(heap);
}

static inline void* GetMemPtrForMBlock_(MEMiExpHeapMBlockHead* pMBlkHd) {
    return AddU32ToPtr(pMBlkHd, sizeof(MEMiExpHeapMBlockHead));
}

static inline MEMiExpHeapMBlockHead* GetMBlockHeadPtr_(void* memBlock) {
    return (MEMiExpHeapMBlockHead*)SubU32ToPtr(memBlock, sizeof(MEMiExpHeapMBlockHead));
}

static inline void* GetMBlockEndAddr_(MEMiExpHeapMBlockHead* pMBHead) {
    return AddU32ToPtr(GetMemPtrForMBlock_(pMBHead), pMBHead->blockSize);
}

static inline u16 GetAllocMode_(MEMiExpHeapHead* pEHHead) {
    return pEHHead->feature.fields.allocMode;
}

static inline void SetAllocMode_(MEMiExpHeapHead* pEHHead, u16 mode) {
    pEHHead->feature.fields.allocMode = mode;
}

static inline BOOL IsUseMarginOfAlign_(MEMiExpHeapHead* pEHHead) {
    return pEHHead->feature.fields.useMarginOfAlign;
}

static inline void SetAllocDirForMBlock_(MEMiExpHeapMBlockHead* pMBHead, u16 mode) {
    pMBHead->attribute.fields.allocDir = mode;
}

static inline u16 GetAlignmentForMBlock_(const MEMiExpHeapMBlockHead* pMBlkHd) {
    return pMBlkHd->attribute.fields.alignment;
}

static inline void SetAlignmentForMBlock_(MEMiExpHeapMBlockHead* pMBlkHd, u16 alignment) {
    pMBlkHd->attribute.fields.alignment = alignment;
}

static inline void SetGroupIDForMBlock_(MEMiExpHeapMBlockHead* pMBHead, u16 id) {
    pMBHead->attribute.fields.groupID = (u8)id;
}

static inline void GetRegionOfMBlock_(MemRegion* region, MEMiExpHeapMBlockHead* block) {
    region->start = SubU32ToPtr(block, GetAlignmentForMBlock_(block));
    region->end = GetMBlockEndAddr_(block);
}

static inline MEMiExpHeapMBlockHead* RemoveMBlock_(MEMiExpMBlockList* list, MEMiExpHeapMBlockHead* block) {
    MEMiExpHeapMBlockHead* const prev = block->pMBHeadPrev;
    MEMiExpHeapMBlockHead* const next = block->pMBHeadNext;

    if (prev != NULL) {
        prev->pMBHeadNext = next;
    } else {
        list->head = next;
    }

    if (next != NULL) {
        next->pMBHeadPrev = prev;
    } else {
        list->tail = prev;
    }

    return prev;
}

static inline MEMiExpHeapMBlockHead* InsertMBlock_(MEMiExpMBlockList* list, MEMiExpHeapMBlockHead* target, MEMiExpHeapMBlockHead* prev) {
    MEMiExpHeapMBlockHead* next;

    target->pMBHeadPrev = prev;

    if (prev != NULL) {
        next = prev->pMBHeadNext;
        prev->pMBHeadNext = target;
    } else {
        next = list->head;
        list->head = target;
    }

    target->pMBHeadNext = next;

    if (next != NULL) {
        next->pMBHeadPrev = target;
    } else {
        list->tail = target;
    }

    return target;
}

static inline void AppendMBlock_(MEMiExpMBlockList* list, MEMiExpHeapMBlockHead* block) {
    (void)InsertMBlock_(list, block, list->tail);
}

static inline MEMiExpHeapMBlockHead* InitMBlock_(MemRegion* pRegion, u16 signature) {
    MEMiExpHeapMBlockHead* block = (MEMiExpHeapMBlockHead*)pRegion->start;

    block->signature = signature;
    block->attribute.val = 0;
    block->blockSize = GetOffsetFromPtr(GetMemPtrForMBlock_(block), pRegion->end);
    block->pMBHeadPrev = NULL;
    block->pMBHeadNext = NULL;

    return block;
}

static inline MEMiExpHeapMBlockHead* InitFreeMBlock_(MemRegion* pRegion) {
    return InitMBlock_(pRegion, MBLOCK_FREE_SIGNATURE);
}

static inline MEMiHeapHead* InitExpHeap_(void* startAddress, void* endAddress, u16 optFlag) {
    MEMiHeapHead* pHeapHd = (MEMiHeapHead*)startAddress;
    MEMiExpHeapHead* pExpHeapHd = GetExpHeapHeadPtrFromHeapHead_(pHeapHd);

    MEMiInitHeapHead(pHeapHd, MEMi_EXPHEAP_SIGNATURE, AddU32ToPtr(pExpHeapHd, sizeof(MEMiExpHeapHead)), endAddress, optFlag);

    pExpHeapHd->groupID = 0;
    pExpHeapHd->feature.val = 0;
    SetAllocMode_(pExpHeapHd, MEM_EXPHEAP_ALLOC_MODE_FIRST);

    {
        MEMiExpHeapMBlockHead* pMBHead;
        MemRegion region;

        region.start = pHeapHd->heapStart;
        region.end = pHeapHd->heapEnd;
        pMBHead = InitFreeMBlock_(&region);

        pExpHeapHd->mbFreeList.head = pMBHead;
        pExpHeapHd->mbFreeList.tail = pMBHead;
        pExpHeapHd->mbUsedList.head = NULL;
        pExpHeapHd->mbUsedList.tail = NULL;

        return pHeapHd;
    }
}

static void* AllocUsedBlockFromFreeBlock_(MEMiExpHeapHead* pEHHead, MEMiExpHeapMBlockHead* pMBHeadFree, void* mblock, u32 size, u16 direction) {
    MemRegion freeRgnT;
    MemRegion freeRgnB;
    MEMiExpHeapMBlockHead* pMBHeadFreePrev;

    GetRegionOfMBlock_(&freeRgnT, pMBHeadFree);
    freeRgnB.end = freeRgnT.end;
    freeRgnB.start = AddU32ToPtr(mblock, size);
    freeRgnT.end = SubU32ToPtr(mblock, sizeof(MEMiExpHeapMBlockHead));

    pMBHeadFreePrev = RemoveMBlock_(&pEHHead->mbFreeList, pMBHeadFree);

    if (GetOffsetFromPtr(freeRgnT.start, freeRgnT.end) < sizeof(MEMiExpHeapMBlockHead) + MIN_FREE_BLOCK_SIZE ||
        (direction == MEM_EXPHEAP_ALLOC_DIR_FRONT && !IsUseMarginOfAlign_(pEHHead))) {
        freeRgnT.end = freeRgnT.start;
    } else {
        pMBHeadFreePrev = InsertMBlock_(&pEHHead->mbFreeList, InitFreeMBlock_(&freeRgnT), pMBHeadFreePrev);
    }

    if (GetOffsetFromPtr(freeRgnB.start, freeRgnB.end) < sizeof(MEMiExpHeapMBlockHead) + MIN_FREE_BLOCK_SIZE ||
        (direction == MEM_EXPHEAP_ALLOC_DIR_REAR && !IsUseMarginOfAlign_(pEHHead))) {
        freeRgnB.start = freeRgnB.end;
    } else {
        (void)InsertMBlock_(&pEHHead->mbFreeList, InitFreeMBlock_(&freeRgnB), pMBHeadFreePrev);
    }

    FillAllocMemory(GetHeapHeadPtrFromExpHeapHead_(pEHHead), freeRgnT.end, GetOffsetFromPtr(freeRgnT.end, freeRgnB.start));

    {
        MEMiExpHeapMBlockHead* pMBHeadNewUsed;
        MemRegion region;

        region.start = SubU32ToPtr(mblock, sizeof(MEMiExpHeapMBlockHead));
        region.end = freeRgnB.start;

        pMBHeadNewUsed = InitMBlock_(&region, MBLOCK_USED_SIGNATURE);
        SetAllocDirForMBlock_(pMBHeadNewUsed, direction);
        SetAlignmentForMBlock_(pMBHeadNewUsed, (u16)GetOffsetFromPtr(freeRgnT.end, pMBHeadNewUsed));
        SetGroupIDForMBlock_(pMBHeadNewUsed, pEHHead->groupID);
        AppendMBlock_(&pEHHead->mbUsedList, pMBHeadNewUsed);
    }

    return mblock;
}

static void* AllocFromHead_(MEMiHeapHead* pHeapHd, u32 size, int alignment) {
    MEMiExpHeapHead* pExpHeapHd = GetExpHeapHeadPtrFromHeapHead_(pHeapHd);
    const BOOL bAllocFirst = GetAllocMode_(pExpHeapHd) == MEM_EXPHEAP_ALLOC_MODE_FIRST;
    MEMiExpHeapMBlockHead* pMBlkHd = NULL;
    MEMiExpHeapMBlockHead* pMBlkHdFound = NULL;
    u32 foundSize = 0xFFFFFFFF;
    void* foundMBlock = NULL;

    for (pMBlkHd = pExpHeapHd->mbFreeList.head; pMBlkHd != NULL; pMBlkHd = pMBlkHd->pMBHeadNext) {
        void* const mblock = GetMemPtrForMBlock_(pMBlkHd);
        void* const reqMBlock = RoundUpPtr(mblock, alignment);
        const u32 offset = GetOffsetFromPtr(mblock, reqMBlock);

        if (pMBlkHd->blockSize >= size + offset && foundSize > pMBlkHd->blockSize) {
            pMBlkHdFound = pMBlkHd;
            foundSize = pMBlkHd->blockSize;
            foundMBlock = reqMBlock;

            if (bAllocFirst || foundSize == size) {
                break;
            }
        }
    }

    if (pMBlkHdFound != NULL) {
        return AllocUsedBlockFromFreeBlock_(pExpHeapHd, pMBlkHdFound, foundMBlock, size, MEM_EXPHEAP_ALLOC_DIR_FRONT);
    }

    return NULL;
}

static void* AllocFromTail_(MEMiHeapHead* pHeapHd, u32 size, int alignment) {
    MEMiExpHeapHead* pExpHeapHd = GetExpHeapHeadPtrFromHeapHead_(pHeapHd);
    const BOOL bAllocFirst = GetAllocMode_(pExpHeapHd) == MEM_EXPHEAP_ALLOC_MODE_FIRST;
    MEMiExpHeapMBlockHead* pMBlkHd = NULL;
    MEMiExpHeapMBlockHead* pMBlkHdFound = NULL;
    u32 foundSize = 0xFFFFFFFF;
    void* foundMBlock = NULL;

    for (pMBlkHd = pExpHeapHd->mbFreeList.tail; pMBlkHd != NULL; pMBlkHd = pMBlkHd->pMBHeadPrev) {
        void* const mblock = GetMemPtrForMBlock_(pMBlkHd);
        void* const mblockEnd = AddU32ToPtr(mblock, pMBlkHd->blockSize);
        void* const reqMBlock = RoundDownPtr(SubU32ToPtr(mblockEnd, size), alignment);

        if (ComparePtr(reqMBlock, mblock) >= 0 && foundSize > pMBlkHd->blockSize) {
            pMBlkHdFound = pMBlkHd;
            foundSize = pMBlkHd->blockSize;
            foundMBlock = reqMBlock;

            if (bAllocFirst || foundSize == size) {
                break;
            }
        }
    }

    if (pMBlkHdFound != NULL) {
        return AllocUsedBlockFromFreeBlock_(pExpHeapHd, pMBlkHdFound, foundMBlock, size, MEM_EXPHEAP_ALLOC_DIR_REAR);
    }

    return NULL;
}

static BOOL RecycleRegion_(MEMiExpHeapHead* pEHHead, const MemRegion* pRegion) {
    MEMiExpHeapMBlockHead* pBlkPrFree = NULL;
    MemRegion freeRgn = *pRegion;

    {
        MEMiExpHeapMBlockHead* pBlk;

        for (pBlk = pEHHead->mbFreeList.head; pBlk != NULL; pBlk = pBlk->pMBHeadNext) {
            if (pBlk < pRegion->start) {
                pBlkPrFree = pBlk;
                continue;
            }

            if (pBlk == pRegion->end) {
                freeRgn.end = GetMBlockEndAddr_(pBlk);
                (void)RemoveMBlock_(&pEHHead->mbFreeList, pBlk);
            }

            break;
        }
    }

    if (pBlkPrFree != NULL && GetMBlockEndAddr_(pBlkPrFree) == pRegion->start) {
        freeRgn.start = pBlkPrFree;
        pBlkPrFree = RemoveMBlock_(&pEHHead->mbFreeList, pBlkPrFree);
    }

    if (GetOffsetFromPtr(freeRgn.start, freeRgn.end) < sizeof(MEMiExpHeapMBlockHead)) {
        return FALSE;
    }

    (void)InsertMBlock_(&pEHHead->mbFreeList, InitFreeMBlock_(&freeRgn), pBlkPrFree);

    return TRUE;
}

MEMHeapHandle MEMCreateExpHeapEx(void* startAddress, u32 size, u16 optFlag) {
    void* endAddress = RoundDownPtr(AddU32ToPtr(startAddress, size), 4);

    startAddress = RoundUpPtr(startAddress, 4);

    if (GetUIntPtr(startAddress) > GetUIntPtr(endAddress) ||
        GetOffsetFromPtr(startAddress, endAddress) < sizeof(MEMiHeapHead) + sizeof(MEMiExpHeapHead) + sizeof(MEMiExpHeapMBlockHead) + MIN_FREE_BLOCK_SIZE) {
        return NULL;
    }

    return InitExpHeap_(startAddress, endAddress, optFlag);
}

void* MEMDestroyExpHeap(MEMHeapHandle heap) {
    MEMiFinalizeHeap(heap);
    return heap;
}

void* MEMAllocFromExpHeapEx(MEMHeapHandle heap, u32 size, int alignment) {
    void* memory = NULL;

    if (size == 0) {
        size = 1;
    }

    size = RoundUp(size, 4);

    LockHeap(heap);

    if (alignment >= 0) {
        memory = AllocFromHead_(heap, size, alignment);
    } else {
        memory = AllocFromTail_(heap, size, -alignment);
    }

    UnlockHeap(heap);

    return memory;
}

void MEMFreeToExpHeap(MEMHeapHandle heap, void* memBlock) {
    MEMiExpHeapHead* pExpHeapHd;
    MEMiExpHeapMBlockHead* pMBHead;
    MemRegion region;

    if (memBlock == NULL) {
        return;
    }

    LockHeap(heap);

    pExpHeapHd = GetExpHeapHeadPtrFromHandle_(heap);
    pMBHead = GetMBlockHeadPtr_(memBlock);

    GetRegionOfMBlock_(&region, pMBHead);
    (void)RemoveMBlock_(&pExpHeapHd->mbUsedList, pMBHead);
    (void)RecycleRegion_(pExpHeapHd, &region);
    UnlockHeap(heap);
}

u32 MEMGetAllocatableSizeForExpHeapEx(MEMHeapHandle heap, int alignment) {
    alignment = __abs(alignment);

    LockHeap(heap);

    {
        MEMiExpHeapHead* pEHHead = GetExpHeapHeadPtrFromHandle_(heap);
        MEMiExpHeapMBlockHead* pMBlkHd;
        u32 maxSize = 0;
        u32 offsetMin = 0xFFFFFFFF;

        for (pMBlkHd = pEHHead->mbFreeList.head; pMBlkHd != NULL; pMBlkHd = pMBlkHd->pMBHeadNext) {
            void* baseAddress = RoundUpPtr(GetMemPtrForMBlock_(pMBlkHd), alignment);

            if (GetUIntPtr(baseAddress) < GetUIntPtr(GetMBlockEndAddr_(pMBlkHd))) {
                const u32 blockSize = GetOffsetFromPtr(baseAddress, GetMBlockEndAddr_(pMBlkHd));
                const u32 offset = GetOffsetFromPtr(GetMemPtrForMBlock_(pMBlkHd), baseAddress);

                if (maxSize < blockSize || (maxSize == blockSize && offsetMin > offset)) {
                    maxSize = blockSize;
                    offsetMin = offset;
                }
            }
        }

        UnlockHeap(heap);

        return maxSize;
    }
}

u16 MEMSetGroupIDForExpHeap(MEMHeapHandle heap, u16 groupID) {
    MEMiExpHeapHead* pEHHead = GetExpHeapHeadPtrFromHandle_(heap);
    u16 oldGroupID;
    BOOL enabled = OSDisableInterrupts();

    oldGroupID = pEHHead->groupID;
    pEHHead->groupID = groupID;

    OSRestoreInterrupts(enabled);

    return oldGroupID;
}
