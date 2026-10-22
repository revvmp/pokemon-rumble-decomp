#ifndef GAME_BKDTREELINK_H
#define GAME_BKDTREELINK_H

#include <revolution/types.h>

class IKDTreeLinkRemoved {
public:
    virtual void OnLinkRemoved() = 0;
};

class BKDTreeLink {
public:
    BKDTreeLink() : mNext(this), mPrev(this), mOwner(nullptr) {}
    ~BKDTreeLink() { Unlink(); }

    bool IsLinked() const { return mNext != this; }

    void LinkTo(BKDTreeLink* pHead, IKDTreeLinkRemoved* pOwner) {
        mNext = pHead->mNext;
        mPrev = pHead;
        mOwner = pOwner;
        pHead->mNext->mPrev = this;
        pHead->mNext = this;
    }

    void Detach() {
        mNext->mPrev = mPrev;
        mPrev->mNext = mNext;
        mNext = this;
        mPrev = this;
    }

    void Unlink() {
        if (mNext != this) {
            IKDTreeLinkRemoved* pOwner = mOwner;
            mNext->mPrev = mPrev;
            mPrev->mNext = mNext;
            mNext = this;
            mPrev = this;
            pOwner->OnLinkRemoved();
        }
    }

    BKDTreeLink* mNext;
    BKDTreeLink* mPrev;
    IKDTreeLinkRemoved* mOwner;
};

#endif
