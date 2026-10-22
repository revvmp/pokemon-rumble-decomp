#ifndef GAME_KDTREEZXNODE_H
#define GAME_KDTREEZXNODE_H

#include <game/BKDTreeLink.h>
#include <memory>

template <int Axis, int Depth> class KDTreeZXNode : public IKDTreeLinkRemoved {
public:
    typedef KDTreeZXNode<(Axis == 2 ? 0 : 2), Depth> Other;

    explicit KDTreeZXNode(Other* pParent) : mParent(pParent) {}

    ~KDTreeZXNode() {
        while (mLinks.mNext != &mLinks) {
            mLinks.mPrev->Detach();
        }
    }

    virtual void OnLinkRemoved();

    void RemoveChild(Other* pChild);

    Other* GetChild(bool right) {
        if (mChildren[right].get() == nullptr) {
            mChildren[right].reset(new Other(this));
        }
        return mChildren[right].get();
    }

    bool IsEmpty() const { return mLinks.mNext == &mLinks; }

    Other* mParent;
    std::tr1::shared_ptr<Other> mChildren[2];
    BKDTreeLink mLinks;
};

template <int Axis, int Depth> void KDTreeZXNode<Axis, Depth>::OnLinkRemoved() {
    if (IsEmpty() && !mChildren[0] && !mChildren[1] && mParent != nullptr) {
        mParent->RemoveChild(this);
    }
}

template <int Axis, int Depth> void KDTreeZXNode<Axis, Depth>::RemoveChild(Other* pChild) {
    if (pChild == mChildren[0].get()) {
        if (IsEmpty() && !mChildren[1] && mParent != nullptr) {
            mParent->RemoveChild(this);
        } else {
            mChildren[0].reset();
        }
    } else {
        if (IsEmpty() && !mChildren[0] && mParent != nullptr) {
            mParent->RemoveChild(this);
        } else {
            mChildren[1].reset();
        }
    }
}

#endif
