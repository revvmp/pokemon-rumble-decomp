#ifndef NW4R_LYT_WINDOW_H
#define NW4R_LYT_WINDOW_H

#include <nw4r/lyt/lyt_resources.h>
#include <nw4r/lyt/lyt_material.h>
#include "nw4r/lyt/lyt_common.h"
#include "nw4r/lyt/lyt_layout.h"
#include "nw4r/lyt/lyt_pane.h"
#include "nw4r/lyt/lyt_types.h"

namespace nw4r {

namespace lyt {

enum TextureFlip {
    TEXFLIP_NONE,
    TEXFLIP_HORIZONTAL,
    TEXFLIP_VERTICAL,
    TEXFLIP_ROTATE_90,
    TEXFLIP_ROTATE_180,
    TEXFLIP_ROTATE_270,
};

enum TextureCoord {
    TopL,
    TopR,
    BotL,
    BotR
};

struct WindowFrameSize {
    f32 l;
    f32 r;
    f32 t;
    f32 b;
};

class Window : public Pane {
public:
    struct Content {
        Content() : vtxColors(), texCoordAry() {}

        ut::Color vtxColors[4];
        detail::TexCoordAry texCoordAry;
    };
    struct Frame {
        Frame() : textureFlip(false), pMaterial(nullptr) {}
        ~Frame() {
            Layout::DeleteObj(pMaterial);
        }
        u8 textureFlip;
        Material *pMaterial;
    };

    Window(const res::Window *pBlock, const ResBlockSet &ResBlockSet);

    void InitContent(u8 texNum);
    void InitFrame(u8 frameNum);
    void ReserveTexCoord(u8 num);

    static WindowFrameSize GetFrameSize(u8 frameNum, const Frame *frames);

    virtual ~Window();
    NW4R_UT_RTTI_DECL(Window);
    virtual void DrawSelf(const DrawInfo &drawInfo);
    virtual void AnimateSelf(u32 option);
    virtual ut::Color GetVtxColor(u32 idx) const;
    virtual void SetVtxColor(u32 idx, ut::Color value);
    virtual u8 GetVtxColorElement(u32 idx) const;
    virtual void SetVtxColorElement(u32 idx, u8 value);
    virtual Material *FindMaterialByName(const char *findName, bool bRecursive);
    virtual void UnbindAnimationSelf(AnimTransform *pAnimTrans);
    virtual u8 GetMaterialNum() const;
    virtual Material *GetMaterial(u32 idx) const;
    virtual Material *GetContentMaterial() const;
    virtual Material *GetFrameMaterial(u32 frameIdx) const;
    virtual void DrawContent(const math::VEC2 &basePt, const WindowFrameSize &frameSize, u8 alpha);
    virtual void DrawFrame(
        const math::VEC2 &basePt, const Frame &frame, const WindowFrameSize &frameSize,
        u8 alpha
    );
    virtual void DrawFrame4(
        const math::VEC2 &basePt, const Frame *frame, const WindowFrameSize &frameSize,
        u8 alpha
    );
    virtual void DrawFrame8(
        const math::VEC2 &basePt, const Frame *frame, const WindowFrameSize &frameSize,
        u8 alpha
    );

private:
    res::InflationLRTB mContentInflation;
    Content mContent;
    Frame *mFrames;
    u8 mFrameNum;
};

}

}

#endif
