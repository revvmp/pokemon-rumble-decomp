#ifndef GAME_CKDTREE_H
#define GAME_CKDTREE_H

#include <game/KDTreeZXNode.h>
#include <algorithm>
#include <math.h>
#include <memory>
#include <nw4r/math/math_geometry.h>

template <int Bits> struct KDTreeBitLength {
    static int Get(int value) {
        enum { half = Bits / 2 };
        if (value < (1 << half)) {
            return KDTreeBitLength<half>::Get(value);
        }
        return half + KDTreeBitLength<Bits - half>::Get(value >> half);
    }
};

template <> struct KDTreeBitLength<1> {
    static int Get(int value) { return 0; }
};

template <typename T> class CKDTree {
public:
    enum { DEPTH = 9, CELLS = 1 << DEPTH };

    CKDTree() {}
    ~CKDTree() {}

    void Insert(T* pObj) {
        if (!mRoot.mNode) {
            return;
        }
        const nw4r::math::AABB& rBounds = pObj->GetBounds();
        int zMin = Quantize(rBounds.min.z, mOrigin.z, mScale.z);
        int zMax = Quantize(rBounds.max.z, mOrigin.z, mScale.z);
        int xMin = Quantize(rBounds.min.x, mOrigin.x, mScale.x);
        int xMax = Quantize(rBounds.max.x, mOrigin.x, mScale.x);
        int zBits = GetBitLength(zMin ^ zMax);
        int xBits = GetBitLength(xMin ^ xMax);
        int zLevel = DEPTH - zBits;
        int xLevel = DEPTH - xBits;

        KDTreeZXNode<2, DEPTH>* pNode = mRoot.mNode.get();
        int mask = CELLS;
        for (int level = 0;; level++) {
            mask >>= 1;
            if (level == zLevel) {
                static_cast<BKDTreeLink*>(pObj)->LinkTo(&pNode->mLinks, pNode);
                return;
            }
            KDTreeZXNode<0, DEPTH>* pZNode = pNode->GetChild((zMin & mask) != 0);
            if (level == xLevel) {
                static_cast<BKDTreeLink*>(pObj)->LinkTo(&pZNode->mLinks, pZNode);
                return;
            }
            pNode = pZNode->GetChild((xMin & mask) != 0);
        }
    }

private:
    static int Quantize(f32 value, f32 origin, f32 scale) {
        int cell = static_cast<int>(static_cast<f32>(floor(scale * (value - origin))));
        int high = 511;
        int low = 0;
        return std::min(std::max(low, cell), high);
    }

    static int GetBitLength(int value) {
        return (value <= 0 ? -1 : KDTreeBitLength<DEPTH>::Get(value)) + 1;
    }

    struct Root {
        std::tr1::shared_ptr<KDTreeZXNode<2, DEPTH> > mNode;
    };
    Root mRoot;
    nw4r::math::AABB mBounds;
    nw4r::math::VEC3 mOrigin;
    nw4r::math::VEC3 mScale;
};

#endif
