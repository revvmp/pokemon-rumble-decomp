#ifndef NW4R_LYT_ANIMATION_H
#define NW4R_LYT_ANIMATION_H
#include <nw4r/lyt/lyt_types.h>

namespace nw4r {
namespace lyt {
class Pane;
class Material;
class ResourceAccessor;
class Group;
class Layout;
namespace res {
struct AnimationBlock;
struct AnimationTagBlock;
struct AnimationShareBlock;
}

class AnimTransform {
public:
    AnimTransform();
    u16 GetFrameSize() const;
    bool IsLoopData() const;
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
    void Init();
    u16 GetGroupNum() const;
    const AnimationGroupRef* GetGroupArray() const;
    bool IsDescendingBind() const;
    u16 GetAnimationShareInfoNum() const;
    const AnimationShareInfo* GetAnimationShareInfoArray() const;
    u16 CalcAnimationNum(Pane* pPane, bool recursive) const;
    u16 CalcAnimationNum(Group* pGroup, bool recursive) const;
    const res::AnimationBlock* GetResourceBlock() const { return mpResBlock; }
    const res::AnimationTagBlock* GetTagBlock() const { return mpTagBlock; }
    const res::AnimationShareBlock* GetShareBlock() const { return mpShareBlock; }
protected:
    const res::BinaryFileHeader* mpFileHeader;
    const res::AnimationBlock* mpResBlock;
    const res::AnimationTagBlock* mpTagBlock;
    const res::AnimationShareBlock* mpShareBlock;
};

class AnimTransformBasic : public AnimTransform {
public:
    AnimTransformBasic();
    AnimationLink *FindUnbindLink(AnimationLink *pLink) const;

    virtual ~AnimTransformBasic();
    virtual void SetResource(const res::AnimationBlock *pRes, ResourceAccessor *pResAccessor);
    virtual void SetResource(const res::AnimationBlock *pRes, ResourceAccessor *pResAccessor, u16 animNum);
    virtual void Bind(Pane *pPane, bool bRecursive, bool bDisable);
    virtual void Bind(Material *pMaterial, bool bDisable);
    virtual void Animate(u32 idx, Pane *pPane);
    virtual void Animate(u32 idx, Material *pMaterial);

    template <typename T>
    AnimationLink *Bind(T *pTarget, AnimationLink *pAnimLink, u16 idx, bool bDisable);

    AnimationLink *GetAnimLinkAry() const {
        return mAnimLinkAry;
    }

protected:
    void **mpFileResAry;
    AnimationLink *mAnimLinkAry;
    u16 mAnimLinkNum;
};

namespace detail {
class AnimPaneTree {
public:
    AnimPaneTree(Pane *pTargetPane, const AnimResource &animRes) : mAnimRes() {
        Init();
        Set(pTargetPane, animRes);
    }
    static u16 FindAnimContent(const res::AnimationBlock *pAnimBlock, const char *animContName, u8 animContType);
    void Init();
    void Set(Pane *pTargetPane, const AnimResource &animRes);
    AnimTransform *Bind(Layout *pLayout, Pane *pTargetPane, ResourceAccessor *pResAccessor) const;

    u16 GetLinkNum() const {
        return mLinkNum;
    }

private:
    AnimResource mAnimRes;
    u16 mAnimPaneIdx;
    u16 mLinkNum;
    u16 mAnimMatIdx[9];
    u8 mAnimMatCnt;
};

AnimationLink* FindAnimationLink(AnimationLinkList* pList, AnimTransform* pTransform);
AnimationLink* FindAnimationLink(AnimationLinkList* pList, const AnimResource& rResource);
void UnbindAnimationLink(AnimationLinkList* pList, AnimTransform* pTransform);
}
}
}
#endif
