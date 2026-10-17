#ifndef NW4R_LYT_ANIMATION_H
#define NW4R_LYT_ANIMATION_H
#include <nw4r/lyt/lyt_types.h>

namespace nw4r {
namespace lyt {
class Pane;
class Material;
class ResourceAccessor;
namespace res {
struct AnimationBlock;
struct AnimationTagBlock;
struct AnimationShareBlock;
}

class AnimTransform {
public:
    AnimTransform();
    virtual ~AnimTransform() = 0;
    virtual void SetResource(const res::AnimationBlock* pResource, ResourceAccessor* pAccessor) = 0;
    virtual void SetResource(const res::AnimationBlock* pResource, ResourceAccessor* pAccessor, u16 animationNum) = 0;
    virtual void Bind(Pane* pPane, bool recursive, bool disable) = 0;
    virtual void Bind(Material* pMaterial, bool disable) = 0;
    virtual void Animate(u32 index, Pane* pPane) = 0;
    virtual void Animate(u32 index, Material* pMaterial) = 0;
    f32 GetFrame() const { return mFrame; }
    void SetFrame(f32 frame) { mFrame = frame; }
    const res::AnimationBlock* GetAnimResource() const { return mpRes; }

    ut::LinkListNode mLink;
protected:
    const res::AnimationBlock* mpRes;
    f32 mFrame;
};

class AnimResource {
public:
    AnimResource();
    explicit AnimResource(const void* pBinary) { Set(pBinary); }
    void Set(const void* pBinary);
    const res::AnimationBlock* GetResourceBlock() const { return mpResBlock; }
protected:
    const res::BinaryFileHeader* mpFileHeader;
    const res::AnimationBlock* mpResBlock;
    const res::AnimationTagBlock* mpTagBlock;
    const res::AnimationShareBlock* mpShareBlock;
};

namespace detail {
AnimationLink* FindAnimationLink(AnimationLinkList* pList, AnimTransform* pTransform);
AnimationLink* FindAnimationLink(AnimationLinkList* pList, const AnimResource& rResource);
void UnbindAnimationLink(AnimationLinkList* pList, AnimTransform* pTransform);
}
}
}
#endif
