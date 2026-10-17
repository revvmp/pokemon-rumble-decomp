#ifndef NW4R_LYT_PANE_H
#define NW4R_LYT_PANE_H

#include <nw4r/lyt/lyt_types.h>
#include <nw4r/math/math_types.h>
#include <nw4r/ut/ut_Color.h>
#include <nw4r/ut/ut_LinkList.h>
#include <nw4r/ut/ut_Rect.h>
#include <nw4r/ut/ut_RuntimeTypeInfo.h>

namespace nw4r {
namespace lyt {

class AnimResource;
class AnimTransform;
class AnimationLink;
class DrawInfo;
class Material;

namespace res {
struct Pane;
struct ExtUserData;
struct ExtUserDataList;
}

namespace detail {
struct PaneBase {
    PaneBase();
    virtual ~PaneBase();
    ut::LinkListNode mLink;
};
}

class Pane : detail::PaneBase {
public:
    Pane();
    Pane(const res::Pane* pBlock);

    virtual ~Pane();
    virtual const ut::detail::RuntimeTypeInfo* GetRuntimeTypeInfo() const;
    virtual void CalculateMtx(const DrawInfo& rInfo);
    virtual void Draw(const DrawInfo& rInfo);
    virtual void DrawSelf(const DrawInfo& rInfo);
    virtual void Animate(u32 option);
    virtual void AnimateSelf(u32 option);
    virtual ut::Color GetVtxColor(u32 index) const;
    virtual void SetVtxColor(u32 index, ut::Color color);
    virtual u8 GetColorElement(u32 index) const;
    virtual void SetColorElement(u32 index, u8 value);
    virtual u8 GetVtxColorElement(u32 index) const;
    virtual void SetVtxColorElement(u32 index, u8 value);
    virtual Pane* FindPaneByName(const char* pName, bool recursive);
    virtual Material* FindMaterialByName(const char* pName, bool recursive);
    virtual void BindAnimation(AnimTransform* pTransform, bool recursive, bool disable);
    virtual void UnbindAnimation(AnimTransform* pTransform, bool recursive);
    virtual void UnbindAllAnimation(bool recursive);
    virtual void UnbindAnimationSelf(AnimTransform* pTransform);
    virtual AnimationLink* FindAnimationLinkSelf(AnimTransform* pTransform);
    virtual AnimationLink* FindAnimationLinkSelf(const AnimResource& rResource);
    virtual void SetAnimationEnable(AnimTransform* pTransform, bool enable, bool recursive);
    virtual void SetAnimationEnable(const AnimResource& rResource, bool enable, bool recursive);
    virtual u8 GetMaterialNum() const;
    virtual Material* GetMaterial() const;
    virtual Material* GetMaterial(u32 index) const;
    virtual void LoadMtx(const DrawInfo& rInfo);

    static ut::detail::RuntimeTypeInfo typeInfo;

protected:
    Pane* mpParent;
    ut::LinkList<Pane, 4> mChildList;
    ut::LinkList<AnimationLink, 0> mAnimList;
    Material* mpMaterial;
    math::VEC3 mTranslate;
    math::VEC3 mRotate;
    math::VEC2 mScale;
    Size mSize;
    math::MTX34 mMtx;
    math::MTX34 mGlbMtx;
    const res::ExtUserDataList* mpExtUserDataList;
    u8 mAlpha;
    u8 mGlbAlpha;
    u8 mBasePosition;
    u8 mFlag;
    char mName[17];
    char mUserData[9];
    bool mbUserAllocated;
    u8 mPadding;
};

}
}

#endif
