#include <game/RenderPrimitive.h>

#include <revolution/GX.h>
#include <revolution/gx/GXTransform.h>
#include <revolution/mtx.h>
#include <nw4r/g3d/g3d_state.h>
void SetupVertexColorRendering() {
    GXSetNumTexGens(0);
    GXSetNumChans(1);
    GXSetNumTevStages(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetClipMode(GX_CLIP_ENABLE);
    GXSetColorUpdate(GX_TRUE);
    GXSetAlphaUpdate(GX_FALSE);
    nw4r::g3d::G3DState::Invalidate();
}
void SetupOverlayRendering() {
    GXSetZMode(GX_FALSE, GX_NEVER, GX_FALSE);
    GXSetCullMode(GX_CULL_NONE);
    SetupVertexColorRendering();
}
void SetupScreenProjection() {
    const GXRenderModeObj* pMode = nw4r::g3d::G3DState::GetRenderModeObj();
    const f32 height = pMode->efbHeight;
    const f32 width = pMode->fbWidth;
    Mtx44 projection;
    C_MTXOrtho(projection, 0.0f, height, 0.0f, width, 0.0f, 1.0f);
    GXSetProjection(projection, GX_ORTHOGRAPHIC);
    GXSetViewport(0.0f, 0.0f, pMode->fbWidth, pMode->efbHeight, 0.0f, 1.0f);
    GXSetScissor(0, 0, pMode->fbWidth, pMode->efbHeight);
    Mtx modelView;
    PSMTXIdentity(modelView);
    GXLoadPosMtxImm(modelView, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    nw4r::g3d::G3DState::Invalidate();
}
void DrawScreenQuad(f32 x, f32 y, f32 width, f32 height, f32 z, nw4r::ut::Color color) {
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(x, y, z);
    GXColor1u32(color);
    GXPosition3f32(x + width, y, z);
    GXColor1u32(color);
    GXPosition3f32(x + width, y + height, z);
    GXColor1u32(color);
    GXPosition3f32(x, y + height, z);
    GXColor1u32(color);
    GXEnd();
}
