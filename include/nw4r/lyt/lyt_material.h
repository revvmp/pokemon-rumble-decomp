#ifndef NW4R_LYT_MATERIAL_H
#define NW4R_LYT_MATERIAL_H
#include <nw4r/lyt/lyt_types.h>
#include <nw4r/ut/ut_Color.h>
#include <revolution/GX.h>
namespace nw4r {
namespace lyt {
class AnimResource;
struct ResBlockSet;
namespace res { struct Material; }
namespace detail {
struct BitGXNums {
    u32 texMap : 4;
    u32 texSRT : 4;
    u32 texCoordGen : 4;
    u32 indSRT : 2;
    u32 indStage : 3;
    u32 tevSwap : 1;
    u32 tevStage : 5;
    u32 chanCtrl : 1;
    u32 matCol : 1;
    u32 alpComp : 1;
    u32 blendMode : 1;
};
}
class Material {
public:
    Material();
    Material(const res::Material* pRes, const ResBlockSet& rBlockSet);
    virtual ~Material();

    virtual bool SetupGX(bool modulate, u8 alpha);

    virtual void BindAnimation(AnimTransform* pAnimTrans);
    virtual void UnbindAnimation(AnimTransform* pAnimTrans);
    virtual void UnbindAllAnimation();

    virtual void Animate();

    virtual AnimationLink*
    FindAnimationLink(AnimTransform* pAnimTrans);
    virtual AnimationLink*
    FindAnimationLink(const AnimResource& rResource);

    virtual void SetAnimationEnable(AnimTransform* pAnimTrans,
                                    bool enable);
    virtual void SetAnimationEnable(const AnimResource& rResource,
                                    bool enable);


    const char* GetName() const { return mName; }
    bool IsUserAllocated() const { return mbUserAllocated; }
protected:
    AnimationLinkList mAnimList;

    GXColorS10 mTevCols[3];
    ut::Color mTevKCols[4];

    detail::BitGXNums mGXMemCap;
    detail::BitGXNums mGXMemNum;
    void* mpGXMem;

    char mName[21];
    bool mbUserAllocated;

    u8 mPadding[2];


};
}
}
#endif
