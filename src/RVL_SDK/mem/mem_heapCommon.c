#include "revolution/mem/heapCommon.h"
#include "revolution/mem/list.h"
#include "revolution/os/OSMutex.h"

static MEMList sRootList;
static BOOL sRootListInitialized = FALSE;
static OSMutex sRootMutex;

static MEMiHeapHead* FindContainHeap_(MEMList* pList, const void* memBlock) {
    MEMiHeapHead* pHeapHd = NULL;

    while ((pHeapHd = (MEMiHeapHead*)MEMGetNextListObject(pList, pHeapHd)) != NULL) {
        if (GetUIntPtr(pHeapHd->heapStart) <= GetUIntPtr(memBlock) && GetUIntPtr(memBlock) < GetUIntPtr(pHeapHd->heapEnd)) {
            MEMiHeapHead* pChildHeapHd = FindContainHeap_(&pHeapHd->childList, memBlock);

            if (pChildHeapHd != NULL) {
                return pChildHeapHd;
            }

            return pHeapHd;
        }
    }

    return NULL;
}

static MEMList* FindListContainHeap_(MEMiHeapHead* pHeapHd) {
    MEMList* pList = &sRootList;
    MEMiHeapHead* pContainHeap = FindContainHeap_(&sRootList, pHeapHd);

    if (pContainHeap != NULL) {
        pList = &pContainHeap->childList;
    }

    return pList;
}

void MEMiInitHeapHead(MEMiHeapHead* pHeapHd, u32 signature, void* heapStart, void* heapEnd, u16 optFlag) {
    pHeapHd->signature = signature;
    pHeapHd->heapStart = heapStart;
    pHeapHd->heapEnd = heapEnd;
    pHeapHd->attribute.val = 0;

    SetOptForHeap(pHeapHd, optFlag);

    MEM_INIT_LIST(&pHeapHd->childList, MEMiHeapHead, link);

    if (!sRootListInitialized) {
        MEM_INIT_LIST(&sRootList, MEMiHeapHead, link);
        OSInitMutex(&sRootMutex);
        sRootListInitialized = TRUE;
    }

    OSInitMutex(&pHeapHd->mutex);

    OSLockMutex(&sRootMutex);
    MEMAppendListObject(FindListContainHeap_(pHeapHd), pHeapHd);
    OSUnlockMutex(&sRootMutex);
}

void MEMiFinalizeHeap(MEMiHeapHead* pHeapHd) {
    OSLockMutex(&sRootMutex);
    MEMRemoveListObject(FindListContainHeap_(pHeapHd), pHeapHd);
    OSUnlockMutex(&sRootMutex);

    pHeapHd->signature = 0;
}

MEMHeapHandle MEMFindContainHeap(const void* memBlock) {
    return FindContainHeap_(&sRootList, memBlock);
}
