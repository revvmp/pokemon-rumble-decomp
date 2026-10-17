#include "nw4hbm/ut/ut_list.h"

namespace nw4hbm {
namespace ut {

void List_Init(List* pList, u16 offset) {
    pList->headObject = nullptr;
    pList->tailObject = nullptr;
    pList->numObjects = 0;
    pList->offset = offset;
}

static void SetFirstObject(List* pList, void* pObject) {
    Link* pLink = NW4R_UT_LIST_GET_LINK(*pList, pObject);
    pLink->nextObject = nullptr;
    pLink->prevObject = nullptr;

    pList->headObject = pObject;
    pList->tailObject = pObject;
    pList->numObjects++;
}

void List_Append(List* pList, void* pObject) {
    if (pList->headObject == nullptr) {
        SetFirstObject(pList, pObject);
        return;
    }

    Link* pLink = NW4R_UT_LIST_GET_LINK(*pList, pObject);
    pLink->prevObject = pList->tailObject;
    pLink->nextObject = nullptr;

    NW4R_UT_LIST_GET_LINK(*pList, pList->tailObject)->nextObject = pObject;
    pList->tailObject = pObject;
    pList->numObjects++;
}

void List_Prepend(List* pList, void* pObject) {
    if (pList->headObject == nullptr) {
        SetFirstObject(pList, pObject);
        return;
    }

    Link* pLink = NW4R_UT_LIST_GET_LINK(*pList, pObject);
    pLink->prevObject = nullptr;
    pLink->nextObject = pList->headObject;

    NW4R_UT_LIST_GET_LINK(*pList, pList->headObject)->prevObject = pObject;
    pList->headObject = pObject;
    pList->numObjects++;
}

void List_Insert(List* pList, void* pTarget, void* pObject) {
    if (pTarget == nullptr) {
        List_Append(pList, pObject);
        return;
    }

    if (pTarget == pList->headObject) {
        List_Prepend(pList, pObject);
        return;
    }

    Link* pLink = NW4R_UT_LIST_GET_LINK(*pList, pObject);
    void* pPrev = NW4R_UT_LIST_GET_LINK(*pList, pTarget)->prevObject;
    Link* pPrevLink = NW4R_UT_LIST_GET_LINK(*pList, pPrev);

    pLink->prevObject = pPrev;
    pLink->nextObject = pTarget;
    pPrevLink->nextObject = pObject;
    NW4R_UT_LIST_GET_LINK(*pList, pTarget)->prevObject = pObject;
    pList->numObjects++;
}

void List_Remove(List* pList, void* pObject) {
    Link* pLink = NW4R_UT_LIST_GET_LINK(*pList, pObject);

    if (pLink->prevObject == nullptr) {
        pList->headObject = pLink->nextObject;
    } else {
        NW4R_UT_LIST_GET_LINK(*pList, pLink->prevObject)->nextObject = pLink->nextObject;
    }

    if (pLink->nextObject == nullptr) {
        pList->tailObject = pLink->prevObject;
    } else {
        NW4R_UT_LIST_GET_LINK(*pList, pLink->nextObject)->prevObject = pLink->prevObject;
    }

    pLink->prevObject = nullptr;
    pLink->nextObject = nullptr;
    pList->numObjects--;
}

void* List_GetNext(const List* pList, const void* pObject) {
    if (pObject == nullptr) {
        return pList->headObject;
    }

    return NW4R_UT_LIST_GET_LINK(*pList, pObject)->nextObject;
}

void* List_GetPrev(const List* pList, const void* pObject) {
    if (pObject == nullptr) {
        return pList->tailObject;
    }

    return NW4R_UT_LIST_GET_LINK(*pList, pObject)->prevObject;
}

void* List_GetNth(const List* pList, u16 index) {
    int i;
    void* pObject;

    for (i = 0, pObject = nullptr; (pObject = List_GetNext(pList, pObject)) != nullptr; i++) {
        if (index == i) {
            return pObject;
        }
    }

    return nullptr;
}

}
}
