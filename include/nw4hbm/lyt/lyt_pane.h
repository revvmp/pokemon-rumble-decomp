#ifndef NW4HBM_LYT_PANE_H
#define NW4HBM_LYT_PANE_H
#include "nw4hbm/types_nw4hbm.h"

#include "nw4hbm/lyt/lyt_animation.h"
#include "nw4hbm/lyt/lyt_common.h"
#include "nw4hbm/lyt/lyt_resources.h"
#include "nw4hbm/lyt/lyt_types.h"
#include "nw4hbm/math.h"
#include "nw4hbm/ut.h"

namespace nw4hbm {
namespace lyt {

class DrawInfo;
class Material;
class AnimResource;

enum AnimOption {
    ANIMOPTION_SKIP_INVISIBLE = (1 << 0),
};

namespace detail {

class PaneBase {
public:
    PaneBase() {}
    virtual ~PaneBase() {}

public:
    NW4R_UT_LINKLIST_NODE_DECL();
};

}

namespace res {

struct Pane {
    static const u32 SIGNATURE = FOURCC('p', 'a', 'n', '1');

    DataBlockHeader blockHeader;
    u8 flag;
    u8 basePosition;
    u8 alpha;
    u8 padding;
    char name[NW4R_LYT_RES_NAME_LEN];
    char userData[NW4R_LYT_PANE_USERDATA_LEN];
    math::VEC3 translate;
    math::VEC3 rotate;
    math::VEC2 scale;
    Size size;
};

}

class Pane : public detail::PaneBase {
public:
    NW4R_UT_RTTI_DECL(Pane);

private:
    enum {
        BIT_VISIBLE,
        BIT_INFLUENCED_ALPHA,
        BIT_LOCATION_ADJUST,
    };

public:
    typedef ut::LinkList<Pane, offsetof(PaneBase, node)> PaneList;

    Pane();
    explicit Pane(const res::Pane* pRes);
    virtual ~Pane();

    virtual void CalculateMtx(const DrawInfo& rInfo);
    void CalculateMtxChild(const DrawInfo& rInfo);

    virtual void Draw(const DrawInfo& rInfo);
    virtual void DrawSelf(const DrawInfo& rInfo);

    virtual void Animate(u32 option = 0);
    virtual void AnimateSelf(u32 option = 0);

    virtual ut::Color GetVtxColor(u32 idx) const;
    virtual void SetVtxColor(u32 idx, ut::Color color);
    virtual u8 GetColorElement(u32 idx) const;
    virtual void SetColorElement(u32 idx, u8 value);
    virtual u8 GetVtxColorElement(u32 idx) const;
    virtual void SetVtxColorElement(u32 idx, u8 value);

    virtual Pane* FindPaneByName(const char* pName, bool recursive);
    virtual Material* FindMaterialByName(const char* pName,
                                         bool recursive);

    virtual void BindAnimation(AnimTransform* pAnimTrans,
                               bool recursive);
    virtual void UnbindAnimation(AnimTransform* pAnimTrans,
                                 bool recursive);

    virtual void UnbindAllAnimation(bool recursive);
    virtual void UnbindAnimationSelf(AnimTransform* pAnimTrans);

    virtual AnimationLink*
    FindAnimationLink(AnimTransform* pAnimTrans);

    virtual void
    SetAnimationEnable(AnimTransform* pAnimTrans, bool enable, bool recursive);

    virtual Material* GetMaterial() const;
    virtual void LoadMtx(const DrawInfo& rInfo);

    void AppendChild(Pane* pChild);
    void PrependChild(Pane* pChild);
    void RemoveChild(Pane* pChild);

    ut::Rect GetPaneRect(const DrawInfo& rInfo) const;

    void AddAnimationLink(AnimationLink* pAnimLink);

    math::VEC2 GetVtxPos() const;

    Pane* GetParent() const {
        return mpParent;
    }

    PaneList& GetChildList() {
        return mChildList;
    }

    void SetSRTElement(u32 idx, f32 value) {
        reinterpret_cast<f32*>(&mTranslate)[idx] = value;
    }

    const math::VEC3& GetTranslate() const {
        return mTranslate;
    }
    void SetTranslate(const math::VEC2& rTransXY) {
        SetTranslate(math::VEC3(rTransXY.x, rTransXY.y, 0.0f));
    }
    void SetTranslate(const math::VEC3& rTrans) {
        mTranslate = rTrans;
    }

    const math::VEC3& GetRotate() const {
        return mRotate;
    }
    void SetRotate(const math::VEC3& rRotate) {
        mRotate = rRotate;
    }

    const math::VEC2& GetScale() const {
        return mScale;
    }
    void SetScale(const math::VEC2& rScale) {
        mScale = rScale;
    }

    const Size& GetSize() const {
        return mSize;
    }
    void SetSize(const Size& rSize) {
        mSize = rSize;
    }

    const math::MTX34& GetMtx() const {
        return mMtx;
    }
    void SetMtx(const math::MTX34& rMtx) {
        mMtx = rMtx;
    }

    const math::MTX34& GetGlobalMtx() const {
        return mGlbMtx;
    }
    void SetGlobalMtx(const math::MTX34& rGlbMtx) {
        mGlbMtx = rGlbMtx;
    }

    u8 GetAlpha() const {
        return mAlpha;
    }
    void SetAlpha(u8 alpha) {
        mAlpha = alpha;
    }

    u8 GetGlbAlpha() const {
        return mGlbAlpha;
    }
    void SetGlbAlpha(u8 alpha) {
        mGlbAlpha = alpha;
    }

    u8 GetBasePositionH() const {
        return detail::GetHorizontalPosition(mBasePosition);
    }
    void SetBasePositionH(u8 position) {
        detail::SetHorizontalPosition(&mBasePosition, position);
    }

    u8 GetBasePositionV() const {
        return detail::GetVerticalPosition(mBasePosition);
    }
    void SetBasePositionV(u8 position) {
        detail::SetVerticalPosition(&mBasePosition, position);
    }

    bool IsVisible() const {
        return detail::TestBit(mFlag, BIT_VISIBLE);
    }
    void SetVisible(bool visible) {
        detail::SetBit(&mFlag, BIT_VISIBLE, visible);
    }

    bool IsInfluencedAlpha() const {
        return detail::TestBit(mFlag, BIT_INFLUENCED_ALPHA);
    }
    void SetInfluencedAlpha(bool influenced) {
        detail::SetBit(&mFlag, BIT_INFLUENCED_ALPHA, influenced);
    }

    bool IsLocationAdjust() const {
        return detail::TestBit(mFlag, BIT_LOCATION_ADJUST);
    }
    void SetLocationAdjust(bool adjust) {
        detail::SetBit(&mFlag, BIT_LOCATION_ADJUST, adjust);
    }

    const char* GetName() const {
        return mName;
    }
    void SetName(const char* pName);

    const char* GetUserData() const {
        return mUserData;
    }
    void SetUserData(const char* pUserData);

    bool IsUserAllocated() const {
        return mbUserAllocated;
    }

protected:
    Pane* mpParent;
    PaneList mChildList;
    AnimationLinkList mAnimList;
    Material* mpMaterial;

    math::VEC3 mTranslate;
    math::VEC3 mRotate;
    math::VEC2 mScale;
    Size mSize;

    math::MTX34 mMtx;
    math::MTX34 mGlbMtx;

    char mName[NW4R_LYT_RES_NAME_LEN];
    char mUserData[NW4R_LYT_PANE_USERDATA_LEN];

    u8 mBasePosition;
    u8 mAlpha;
    u8 mGlbAlpha;
    u8 mFlag;

    bool mbUserAllocated;

protected:
    void InsertChild(PaneList::Iterator next, Pane* pChild);

private:
    void Init();
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(Pane);

}
}

#endif
