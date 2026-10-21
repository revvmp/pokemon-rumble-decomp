#ifndef GAME_CKDTREE_H
#define GAME_CKDTREE_H

#include <memory>
#include <nw4r/math/math_geometry.h>

template <int Axis, int Depth> class KDTreeZXNode;

template <typename T> class CKDTree {
public:
    CKDTree() {}
    ~CKDTree() {}

private:
    struct Root {
        std::tr1::shared_ptr<KDTreeZXNode<2, 9> > mNode;
    };
    Root mRoot;
    nw4r::math::AABB mBounds;
    nw4r::math::VEC3 mOrigin;
    nw4r::math::VEC3 mScale;
};

#endif