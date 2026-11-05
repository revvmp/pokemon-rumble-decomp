#include <game/world/CBSPTree.h>
#include <new>

CBSPTree::CBSPTree() : mpData(nullptr) {}
CBSPTree::~CBSPTree() {}
void CBSPTree::SetData(void* pData) { mpData = pData; }

bool CBSPTree::TraceSegment(Trace& rTrace, f32 first, f32 last) const {
    return TraceSegment(static_cast<const Node*>(mpData) + 2, rTrace, first, last);
}

bool CBSPTree::TraceSegment(const Node* pNode, Trace& rTrace, f32 first, f32 last) const {
    if (pNode == nullptr) {
        return false;
    }
    f32 direction = pNode->mNormal.Dot(rTrace.mDirection);
    if (pNode->mLeaf.mMarker == 0xFFFFFFFF) {
        if (0.0f <= direction) {
            return false;
        }
        f32 fraction = (-pNode->mDistance - pNode->mNormal.Dot(rTrace.mStart)) / direction;
        bool hit = first - 0.001f <= fraction && fraction <= last + 0.001f;
        if (hit) {
            rTrace.mFraction = fraction;
            rTrace.mpNode = pNode;
            rTrace.mpPayload = static_cast<const u8*>(mpData) + pNode->mLeaf.mPayloadOffset;
        }
        return hit;
    }
    f32 distance = -pNode->mDistance - pNode->mNormal.Dot(rTrace.mStart);
    int side = distance < 0.0f ? 0 : 3;
    if (distance == 0.0f) {
        side = 0.0f < direction ? 0 : 3;
    }
    if (static_cast<f32>(__fabs(direction)) <= 0.001f) {
        if (static_cast<f32>(__fabs(distance)) <= 0.001f) {
            if (TraceSegment(GetChild(pNode, 3 - side), rTrace, first, last)) {
                return true;
            }
        }
        return TraceSegment(GetChild(pNode, side), rTrace, first, last);
    }
    f32 fraction = distance / direction;
    if (fraction <= 0.0f) {
        if (TraceSegment(GetChild(pNode, side), rTrace, first, last)) {
            return true;
        }
    } else if (fraction < first - 0.001f) {
        if (TraceSegment(GetChild(pNode, 3 - side), rTrace, first, last)) {
            return true;
        }
    } else if (fraction <= last + 0.001f) {
        if (side == 0) {
            if (TraceSegment(GetChild(pNode, 0), rTrace, first, fraction)) {
                return true;
            }
            if (TraceSegment(GetChild(pNode, 1), rTrace, fraction, fraction)) {
                return true;
            }
            if (TraceSegment(GetChild(pNode, 3), rTrace, fraction, last)) {
                return true;
            }
        } else {
            if (TraceSegment(GetChild(pNode, 3), rTrace, first, fraction)) {
                return true;
            }
            if (TraceSegment(GetChild(pNode, 2), rTrace, fraction, fraction)) {
                return true;
            }
            if (TraceSegment(GetChild(pNode, 0), rTrace, fraction, last)) {
                return true;
            }
        }
    } else {
        if (TraceSegment(GetChild(pNode, side), rTrace, first, last)) {
            return true;
        }
    }
    return false;
}
