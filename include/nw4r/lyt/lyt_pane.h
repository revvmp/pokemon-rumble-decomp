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

enum AnimationOption {
    ANIMOPTION_SKIP_INVISIBLE = 1
};

enum AnimTargetPaneColor {
    ANIMTARGET_PANE_COLOR_ALPHA = 16
};

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
    virtual const ut::detail::RuntimeTypeInfo* GetRuntimeTypeInfo() const { return &typeInfo; }
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
    virtual void SetExtUserDataList(const res::ExtUserDataList* pList);
    virtual void LoadMtx(const DrawInfo& rInfo);

    static ut::detail::RuntimeTypeInfo typeInfo;

    typedef ut::LinkList<Pane, 4> PaneList;
    void AppendChild(Pane* pChild);
    void PrependChild(Pane* pChild);
    void RemoveChild(Pane* pChild);
    ut::Rect GetPaneRect(const DrawInfo& rInfo) const;
    void AddAnimationLink(AnimationLink* pLink);
    math::VEC2 GetVtxPos() const;
    const char* GetName() const { return mName; }
    Pane* GetParent() const { return mpParent; }
    PaneList* GetChildList() { return &mChildList; }
    bool IsUserAllocated() const { return mbUserAllocated; }
    bool IsVisible() const { return (mFlag & 1) != 0; }
    void SetVisible(bool visible) {
        mFlag &= ~1;
        mFlag |= visible ? 1 : 0;
    }
    u8 GetAlpha() const { return mAlpha; }
    bool IsInfluencedAlpha() const { return (mFlag & 2) != 0; }
    bool IsLocationAdjust() const { return (mFlag & 4) != 0; }
    bool IsVisibleForDraw() const {
        bool visible = IsVisible();
        if (mScale.x == 0.0f || mScale.y == 0.0f) {
            visible = false;
        }
        if (mAlpha == 0 && IsInfluencedAlpha()) {
            visible = false;
        }
        return visible;
    }
    const res::ExtUserData* GetExtUserData() const;
    void SetSRTElement(u32 index, f32 value) {
        reinterpret_cast<f32*>(&mTranslate)[index] = value;
    }
    void SetName(const char* pName);
    void SetUserData(const char* pUserData);
    const res::ExtUserData* FindExtUserDataByName(const char* pName);

protected:
    void Init();
    void InsertChild(PaneList::Iterator next, Pane* pChild);
    void CalculateMtxChild(const DrawInfo& rInfo);
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
