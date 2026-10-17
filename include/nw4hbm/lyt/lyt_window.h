#ifndef NW4R_LYT_WINDOW_H
#define NW4R_LYT_WINDOW_H
#include "nw4hbm/types_nw4hbm.h"

#include "nw4hbm/lyt/lyt_common.h"
#include "nw4hbm/lyt/lyt_layout.h"
#include "nw4hbm/lyt/lyt_material.h"
#include "nw4hbm/lyt/lyt_pane.h"

#include "nw4hbm/ut.h"

namespace nw4hbm {
namespace lyt {

class Material;
struct ResBlockSet;

struct InflationLRTB {
    f32 l;
    f32 r;
    f32 t;
    f32 b;
};

namespace res {

struct WindowContent {
    u32 vtxCols[VERTEXCOLOR_MAX];
    u16 materialIdx;
    u8 texCoordNum;
    u8 PADDING_0x13[0x14 - 0x13];
};

struct WindowFrame {
    u16 materialIdx;
    u8 textureFlip;
    u8 PADDING_0x3;
};

struct Window : public Pane {
    static const u32 SIGNATURE = FOURCC('w', 'n', 'd', '1');

    InflationLRTB inflation;
    u8 frameNum;
    u8 PADDING_0x5D;
    u8 PADDING_0x5E;
    u8 PADDING_0x5F;
    u32 contentOffset;
    u32 frameOffsetTableOffset;
};

}

enum WindowFrameEnum {
    WINDOWFRAME_LT,
    WINDOWFRAME_RT,
    WINDOWFRAME_LB,
    WINDOWFRAME_RB,

    WINDOWFRAME_L,
    WINDOWFRAME_R,
    WINDOWFRAME_T,
    WINDOWFRAME_B,

    WINDOWFRAME_MAX
};

struct WindowFrameSize {
    f32 l;
    f32 r;
    f32 t;
    f32 b;
};

class Window : public Pane {
public:
    NW4R_UT_RTTI_DECL(Window);

protected:
    struct Content {
        ut::Color vtxColors[VERTEXCOLOR_MAX];
        detail::TexCoordAry texCoordAry;
    };

    struct Frame {
        u8 textureFlip;
        Material* pMaterial;
    };

public:
    Window(const res::Window* pRes, const ResBlockSet& rBlockSet);
    virtual ~Window();

    virtual void DrawSelf(const DrawInfo& rInfo);
    virtual void AnimateSelf(u32 option = 0);

    virtual ut::Color GetVtxColor(u32 idx) const;
    virtual void SetVtxColor(u32 idx, ut::Color color);
    virtual u8 GetVtxColorElement(u32 idx) const;
    virtual void SetVtxColorElement(u32 idx, u8 value);

    virtual Material* FindMaterialByName(const char* pName, bool recursive);

    virtual void UnbindAnimationSelf(AnimTransform* pAnimTrans);

    virtual AnimationLink* FindAnimationLink(AnimTransform* pAnimTrans);

    virtual void SetAnimationEnable(AnimTransform* pAnimTrans, bool enable, bool recursive);

    virtual Material* GetContentMaterial() const;
    virtual Material* GetFrameMaterial(u32 idx) const;

protected:
    virtual void DrawContent(const math::VEC2& rBase, const WindowFrameSize& rFrameSize, u8 alpha);
    virtual void DrawFrame(const math::VEC2& rBase, const Frame& rFrame, const WindowFrameSize& rFrameSize, u8 alpha);
    virtual void DrawFrame4(const math::VEC2& rBase, const Frame* pFrames, const WindowFrameSize& rFrameSize,
                            u8 alpha);
    virtual void DrawFrame8(const math::VEC2& rBase, const Frame* pFrames, const WindowFrameSize& rFrameSize,
                            u8 alpha);

    WindowFrameSize GetFrameSize(u8 frameNum, const Frame* pFrames);

protected:
    InflationLRTB mContentInflation;
    Content mContent;
    Frame* mFrames;
    u8 mFrameNum;
};

}
}

#endif
