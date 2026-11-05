#ifndef GAME_CKDTREE_H
#define GAME_CKDTREE_H

#include <game/world/KDTreeZXNode.h>
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
    typedef KDTreeZXNode<0, DEPTH> ZNode;
    typedef KDTreeZXNode<2, DEPTH> XNode;

    CKDTree() {}
    ~CKDTree() {}

    void Insert(T* pObj) {
        if (!mRoot.mNode) {
            return;
        }
        const nw4r::math::AABB& rBounds = pObj->GetBounds();
        int zMin;
        int zMax;
        int xMin;
        int xMax;
        QuantizeRange(rBounds.min.z, rBounds.max.z, mOrigin.z, mScale.z, zMin, zMax);
        QuantizeRange(rBounds.min.x, rBounds.max.x, mOrigin.x, mScale.x, xMin, xMax);
        int zBits = GetBitLength(zMin ^ zMax);
        int xBits = GetBitLength(xMin ^ xMax);
        int zLevel = DEPTH - zBits;
        int xLevel = DEPTH - xBits;

        XNode* pNode = mRoot.mNode.get();
        int mask = CELLS;
        for (int level = 0;; level++) {
            mask >>= 1;
            if (level == zLevel) {
                static_cast<BKDTreeLink*>(pObj)->LinkTo(&pNode->mLinks, pNode);
                return;
            }
            std::tr1::shared_ptr<ZNode>& rZChild = pNode->mChildren[(zMin & mask) != 0];
            if (rZChild.get() == nullptr) {
                rZChild.reset(new ZNode(pNode));
            }
            ZNode* pZNode = rZChild.get();
            if (xLevel == level) {
                static_cast<BKDTreeLink*>(pObj)->LinkTo(&pZNode->mLinks, pZNode);
                return;
            }
            std::tr1::shared_ptr<XNode>& rXChild = pZNode->mChildren[(xMin & mask) != 0];
            if (rXChild.get() == nullptr) {
                rXChild.reset(new XNode(pZNode));
            }
            pNode = rXChild.get();
        }
    }

private:
    static void QuantizeRange(const f32& rMin, const f32& rMax, const f32& rOrigin, const f32& rScale, int& rLow, int& rHigh) {
        rLow = Quantize(rScale, rMin, rOrigin);
        rHigh = Quantize(rScale, rMax, rOrigin);
    }

    static int Quantize(f32 scale, f32 value, f32 origin) {
        int high;
        int low;
        int cell = static_cast<int>(static_cast<f32>(floor(scale * (value - origin))));
        high = 511;
        low = 0;
        return std::min(std::max(low, cell), high);
    }

    static int GetBitLength(int value) {
        return (value <= 0 ? -1 : KDTreeBitLength<DEPTH>::Get(value)) + 1;
    }

    struct Root {
        std::tr1::shared_ptr<XNode> mNode;
    };
    Root mRoot;
    nw4r::math::AABB mBounds;
    nw4r::math::VEC3 mOrigin;
    nw4r::math::VEC3 mScale;
};

#endif
