#include "revolution/mem/list.h"

#define GetLink(pList, pObject) ((MEMLink*)((u32)(pObject) + (pList)->offset))

void MEMInitList(MEMList* pList, u16 offset) {
    pList->headObject = NULL;
    pList->tailObject = NULL;
    pList->numObjects = 0;
    pList->offset = offset;
}

static void SetFirstObject_(MEMList* pList, void* pObject) {
    MEMLink* pLink = GetLink(pList, pObject);

    pLink->nextObject = NULL;
    pLink->prevObject = NULL;
    pList->headObject = pObject;
    pList->tailObject = pObject;
    pList->numObjects++;
}

void MEMAppendListObject(MEMList* pList, void* pObject) {
    if (pList->headObject == NULL) {
        SetFirstObject_(pList, pObject);
    } else {
        MEMLink* pLink = GetLink(pList, pObject);

        pLink->prevObject = pList->tailObject;
        pLink->nextObject = NULL;
        GetLink(pList, pList->tailObject)->nextObject = pObject;
        pList->tailObject = pObject;
        pList->numObjects++;
    }
}

void MEMRemoveListObject(MEMList* pList, void* pObject) {
    MEMLink* pLink = GetLink(pList, pObject);

    if (pLink->prevObject == NULL) {
        pList->headObject = pLink->nextObject;
    } else {
        GetLink(pList, pLink->prevObject)->nextObject = pLink->nextObject;
    }

    if (pLink->nextObject == NULL) {
        pList->tailObject = pLink->prevObject;
    } else {
        GetLink(pList, pLink->nextObject)->prevObject = pLink->prevObject;
    }

    pLink->prevObject = NULL;
    pLink->nextObject = NULL;
    pList->numObjects--;
}

void* MEMGetNextListObject(MEMList* pList, void* pObject) {
    if (pObject == NULL) {
        return pList->headObject;
    }

    return GetLink(pList, pObject)->nextObject;
}
