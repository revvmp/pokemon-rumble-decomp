#ifndef NW4R_LYT_TYPES_H
#define NW4R_LYT_TYPES_H
#include <nw4r/types_nw4r.h>
#include <nw4r/ut/ut_LinkList.h>

namespace nw4r {
namespace lyt {

enum HorizontalPosition {
    HORIZONTALPOSITION_LEFT,
    HORIZONTALPOSITION_CENTER,
    HORIZONTALPOSITION_RIGHT,
    HORIZONTALPOSITION_MAX
};
enum VerticalPosition {
    VERTICALPOSITION_TOP,
    VERTICALPOSITION_CENTER,
    VERTICALPOSITION_BOTTOM,
    VERTICALPOSITION_MAX
};

namespace detail {
inline u8 GetHorizontalPosition(u8 packed) {
    return packed % HORIZONTALPOSITION_MAX;
}
inline u8 GetVerticalPosition(u8 packed) {
    return packed / HORIZONTALPOSITION_MAX;
}
template <typename T> inline const T* ConvertOffsToPtr(const void* pBase, unsigned int offset) {
    return reinterpret_cast<const T*>(reinterpret_cast<u32>(pBase) + offset);
}
template <typename T> inline T* ConvertOffsToPtr(void* pBase, unsigned int offset) {
    return reinterpret_cast<T*>(reinterpret_cast<u32>(pBase) + offset);
}
}

class AnimTransform;

struct AnimationShareInfo {
    const char *GetTargetGroupName() const {
        return targetGroupName;
    }
    const char *GetSrcPaneName() const {
        return srcPaneName;
    }

    char srcPaneName[17];
    char targetGroupName[17];
    u8 padding[2];
};

struct AnimationGroupRef {
    const char *GetName() const {
        return name;
    }

    char name[17];
    u8 flag;
    u8 padding[2];
};

class AnimationLink {
public:
    inline AnimationLink() : mLink() {
        Reset();
    }

    AnimTransform *GetAnimTransform() const {
        return mAnimTrans;
    }
    u16 GetIndex() const {
        return mIdx;
    }
    void SetIndex(u16 val) {
        mIdx = val;
    }

    bool IsEnable() const {
        return !mbDisable;
    }
    void SetEnable(bool enable) {
        mbDisable = !enable;
    }

    void Reset() {
        Set(nullptr, 0, false);
    }

    void Set(AnimTransform *pTransform, u16 index, bool disable) {
        mAnimTrans = pTransform;
        mIdx = index;
        mbDisable = disable;
    }

private:
    ut::LinkListNode mLink;
    AnimTransform *mAnimTrans;
    u16 mIdx;
    bool mbDisable;
};


typedef ut::LinkList<AnimationLink, 0> AnimationLinkList;

struct Size {
    Size() : width(0.0f), height(0.0f) {}
    Size(f32 w, f32 h) : width(w), height(h) {}

    Size(const Size& rSize) : width(rSize.width), height(rSize.height) {}

    f32 width;
    f32 height;
};

namespace res {

struct DataBlockHeader {
    u32 kind;
    u32 size;
};

struct BinaryFileHeader {
    char signature[4];
    u16 byteOrder;
    u16 version;
    u32 fileSize;
    u16 headerSize;
    u16 dataBlocks;
};

}
}
}

#endif
