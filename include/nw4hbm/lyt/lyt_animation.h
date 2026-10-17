#ifndef NW4HBM_LYT_ANIMATION_H
#define NW4HBM_LYT_ANIMATION_H
#include "nw4hbm/types_nw4hbm.h"

#include "nw4hbm/lyt/lyt_common.h"
#include "nw4hbm/lyt/lyt_resources.h"
#include "nw4hbm/lyt/lyt_types.h"

#include "nw4hbm/ut.h"

namespace nw4hbm {
namespace lyt {

class Pane;
class Material;
class ResourceAccessor;

namespace res {

struct AnimationBlock {
    DataBlockHeader blockHeader;
    u16 frameSize;
    u8 loop;
    u8 PADDING_0xB;
    u16 fileNum;
    u16 animContNum;
    u32 animContOffsetsOffset;
};

struct AnimationTagBlock {
    DataBlockHeader blockHeader;
    u16 tagOrder;
    u16 groupNum;
    u32 nameOffset;
    u32 groupsOffset;
    s16 startFrame;
    s16 endFrame;
    u8 flag;
    u8 padding[3];
};

struct AnimationShareBlock {
    DataBlockHeader blockHeader;
    u32 animShareInfoOffset;
    u16 shareNum;
    u8 padding[2];
};

struct AnimationInfo {
    u32 kind;
    u8 num;
    u8 PADDING_0x5[0x8 - 0x5];

    static const u32 SIGNATURE_ANMPANESRT = FOURCC('R', 'L', 'P', 'A');
    static const u32 SIGNATURE_ANMPANEVIS = FOURCC('R', 'L', 'V', 'I');
    static const u32 SIGNATURE_ANMVTXCLR = FOURCC('R', 'L', 'V', 'C');

    static const u32 SIGNATURE_ANMMATCLR = FOURCC('R', 'L', 'M', 'C');
    static const u32 SIGNATURE_ANMTEXSRT = FOURCC('R', 'L', 'T', 'S');
    static const u32 SIGNATURE_ANMTEXPAT = FOURCC('R', 'L', 'T', 'P');
    static const u32 SIGNATURE_ANMINDTEXSRT = FOURCC('R', 'L', 'I', 'M');
};

struct AnimationContent {
    static const int NAME_LEN =
        MAX(NW4R_LYT_RES_NAME_LEN, NW4R_LYT_MATERIAL_NAME_LEN);

    enum AnimType { ANIMTYPE_PANE, ANIMTYPE_MATERIAL, ANIMTYPE_MAX };

    char name[NAME_LEN];
    u8 num;
    u8 type;
    u8 PADDING_0x16[0x18 - 0x16];
};

}

class AnimResource {
protected:
    const res::BinaryFileHeader* mpFileHeader;
    const res::AnimationBlock* mpResBlock;
    const res::AnimationTagBlock* mpTagBlock;
    const res::AnimationShareBlock* mpShareBlock;
};

class AnimTransform {
public:
    AnimTransform();
    virtual ~AnimTransform();

    virtual void SetResource(const res::AnimationBlock* pBlock,
                             ResourceAccessor* pAccessor) = 0;
    virtual void SetResource(const res::AnimationBlock* pBlock,
                             ResourceAccessor* pAccessor,
                             u16 animNum) = 0;

    virtual void Bind(Pane* pPane, bool recursive) = 0;
    virtual void Bind(Material* pMaterial) = 0;

    virtual void Animate(u32 idx, Pane* pPane) = 0;
    virtual void Animate(u32 idx, Material* pMaterial) = 0;

    u16 GetFrameSize() const;

    bool IsLoopData() const;

    f32 GetFrame() const {
        return mFrame;
    }
    void SetFrame(f32 frame) {
        mFrame = frame;
    }

    const res::AnimationBlock* GetAnimResource() const {
        return mpRes;
    }

    f32 GetFrameMax() const {
        return static_cast<f32>(GetFrameSize());
    }

public:
    NW4R_UT_LINKLIST_NODE_DECL();

protected:
    const res::AnimationBlock* mpRes;
    f32 mFrame;
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(AnimTransform);

class AnimTransformBasic : public AnimTransform {
public:
    AnimTransformBasic();
    virtual ~AnimTransformBasic();

    virtual void SetResource(const res::AnimationBlock* pBlock,
                             ResourceAccessor* pAccessor);
    virtual void SetResource(const res::AnimationBlock* pBlock,
                             ResourceAccessor* pAccessor,
                             u16 animNum);

    virtual void Bind(Pane* pPane, bool recursive);
    virtual void Bind(Material* pMaterial);

    virtual void Animate(u32 idx, Pane* pPane);
    virtual void Animate(u32 idx, Material* pMaterial);

protected:
    void** mpFileResAry;
    AnimationLink* mAnimLinkAry;
    u16 mAnimLinkNum;
};

namespace detail {

AnimationLink* FindAnimationLink(AnimationLinkList* pAnimList,
                                 AnimTransform* pAnimTrans);
AnimationLink* FindAnimationLink(AnimationLinkList* pAnimList,
                                 const AnimResource& rResource);
void UnbindAnimationLink(AnimationLinkList* pAnimList,
                                 AnimTransform* pAnimTrans);

}

enum AnimTargetPane {
    ANIMTARGET_PANE_TRANSX,
    ANIMTARGET_PANE_TRANSY,
    ANIMTARGET_PANE_TRANSZ,

    ANIMTARGET_PANE_ROTX,
    ANIMTARGET_PANE_ROTY,
    ANIMTARGET_PANE_ROTZ,

    ANIMTARGET_PANE_SCALEX,
    ANIMTARGET_PANE_SCALEY,

    ANIMTARGET_PANE_SIZEX,
    ANIMTARGET_PANE_SIZEY,

    ANIMTARGET_PANE_MAX,

    ANIMTARGET_PANE_COLOR_ALPHA = 16,
    ANIMTARGET_PANE_COLOR_MAX,
};

enum AnimTargetVtxColor {
    ANIMTARGET_VERTEXCOLOR_LT_RED,
    ANIMTARGET_VERTEXCOLOR_LT_GREEN,
    ANIMTARGET_VERTEXCOLOR_LT_BLUE,
    ANIMTARGET_VERTEXCOLOR_LT_ALPHA,

    ANIMTARGET_VERTEXCOLOR_RT_RED,
    ANIMTARGET_VERTEXCOLOR_RT_GREEN,
    ANIMTARGET_VERTEXCOLOR_RT_BLUE,
    ANIMTARGET_VERTEXCOLOR_RT_ALPHA,

    ANIMTARGET_VERTEXCOLOR_LB_RED,
    ANIMTARGET_VERTEXCOLOR_LB_GREEN,
    ANIMTARGET_VERTEXCOLOR_LB_BLUE,
    ANIMTARGET_VERTEXCOLOR_LB_ALPHA,

    ANIMTARGET_VERTEXCOLOR_RB_RED,
    ANIMTARGET_VERTEXCOLOR_RB_GREEN,
    ANIMTARGET_VERTEXCOLOR_RB_BLUE,
    ANIMTARGET_VERTEXCOLOR_RB_ALPHA,

    ANIMTARGET_VERTEXCOLOR_MAX
};

enum AnimTargetMatColor {
    ANIMTARGET_MATCOLOR_MATR,
    ANIMTARGET_MATCOLOR_MATG,
    ANIMTARGET_MATCOLOR_MATB,
    ANIMTARGET_MATCOLOR_MATA,

    ANIMTARGET_MATCOLOR_TEV0R,
    ANIMTARGET_MATCOLOR_TEV0G,
    ANIMTARGET_MATCOLOR_TEV0B,
    ANIMTARGET_MATCOLOR_TEV0A,

    ANIMTARGET_MATCOLOR_TEV1R,
    ANIMTARGET_MATCOLOR_TEV1G,
    ANIMTARGET_MATCOLOR_TEV1B,
    ANIMTARGET_MATCOLOR_TEV1A,

    ANIMTARGET_MATCOLOR_TEV2R,
    ANIMTARGET_MATCOLOR_TEV2G,
    ANIMTARGET_MATCOLOR_TEV2B,
    ANIMTARGET_MATCOLOR_TEV2A,

    ANIMTARGET_MATCOLOR_TEVK0R,
    ANIMTARGET_MATCOLOR_TEVK0G,
    ANIMTARGET_MATCOLOR_TEVK0B,
    ANIMTARGET_MATCOLOR_TEVK0A,

    ANIMTARGET_MATCOLOR_TEVK1R,
    ANIMTARGET_MATCOLOR_TEVK1G,
    ANIMTARGET_MATCOLOR_TEVK1B,
    ANIMTARGET_MATCOLOR_TEVK1A,

    ANIMTARGET_MATCOLOR_TEVK2R,
    ANIMTARGET_MATCOLOR_TEVK2G,
    ANIMTARGET_MATCOLOR_TEVK2B,
    ANIMTARGET_MATCOLOR_TEVK2A,

    ANIMTARGET_MATCOLOR_TEVK3R,
    ANIMTARGET_MATCOLOR_TEVK3G,
    ANIMTARGET_MATCOLOR_TEVK3B,
    ANIMTARGET_MATCOLOR_TEVK3A,

    ANIMTARGET_MATCOLOR_MAX
};

enum AnimTargetTexSRT {
    ANIMTARGET_TEXSRT_TRANSX,
    ANIMTARGET_TEXSRT_TRANSY,

    ANIMTARGET_TEXSRT_ROT,

    ANIMTARGET_TEXSRT_SCALEX,
    ANIMTARGET_TEXSRT_SCALEY,

    ANIMTARGET_TEXSRT_MAX
};

enum AnimTargetTexPat {
    ANIMTARGET_TEXPATTURN_IMAGE,

    ANIMTARGET_TEXPATTURN_MAX
};

}
}

#endif
