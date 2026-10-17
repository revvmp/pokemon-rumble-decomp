#include "nw4hbm/ut/ut_LinkList.h"

namespace nw4hbm {
namespace ut {
namespace detail {

LinkListImpl::~LinkListImpl() {
    Clear();
}

LinkListImpl::Iterator LinkListImpl::Erase(Iterator it) {
    Iterator next(it);
    return Erase(it, ++next);
}

void LinkListImpl::Clear() {
    Erase(GetBeginIter(), GetEndIter());
}

LinkListImpl::Iterator LinkListImpl::Insert(Iterator it, LinkListNode* pNode) {
    LinkListNode* pNext = it.mNode;
    LinkListNode* pPrev = pNext->mPrev;

    pNode->mNext = pNext;
    pNode->mPrev = pPrev;
    pNext->mPrev = pNode;
    pPrev->mNext = pNode;
    mSize++;

    return Iterator(pNode);
}

LinkListImpl::Iterator LinkListImpl::Erase(LinkListNode* pNode) {
    LinkListNode* pNext = pNode->mNext;
    LinkListNode* pPrev = pNode->mPrev;

    pNext->mPrev = pPrev;
    pPrev->mNext = pNext;
    mSize--;

    pNode->mNext = nullptr;
    pNode->mPrev = nullptr;

    return Iterator(pNext);
}

LinkListImpl::Iterator LinkListImpl::Erase(Iterator begin, Iterator end) {
    LinkListNode* pNode = begin.mNode;
    LinkListNode* pEnd = end.mNode;

    while (pNode != pEnd) {
        LinkListNode* pNext = pNode->mNext;
        Erase(pNode);
        pNode = pNext;
    }

    return Iterator(pEnd);
}

}
}
}
