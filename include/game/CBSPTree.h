#ifndef GAME_CBSPTREE_H
#define GAME_CBSPTREE_H

#include <nw4r/math/math_types.h>

class CBSPTree {
public:
    struct Node {
        nw4r::math::VEC3 mNormal;
        f32 mDistance;
        union {
            u16 mChildren[4];
            struct {
                u32 mMarker;
                u32 mPayloadOffset;
            } mLeaf;
        };
    };

    struct Trace {
        nw4r::math::VEC3 mStart;
        nw4r::math::VEC3 mDirection;
        f32 mFraction;
        const Node* mpNode;
        const void* mpPayload;
    };

    CBSPTree();
    ~CBSPTree();
    void SetData(void* pData);
    bool TraceSegment(Trace& rTrace, f32 first, f32 last) const;

private:
    const Node* GetChild(const Node* pNode, int child) const {
        const Node* pBase = static_cast<const Node*>(mpData);
        u16 index = pNode->mChildren[child];
        if (index == 0) {
            return nullptr;
        }
        if (index == 0xFFFF) {
            return nullptr;
        }
        return pBase + index;
    }
    bool TraceSegment(const Node* pNode, Trace& rTrace, f32 first, f32 last) const;
    void* mpData;
};

#endif
