#include <game/Graphics.h>
#include <revolution/GX.h>
#include <revolution/gx/GXTransform.h>
#include <revolution/gx/GXBump.h>
#include <revolution/gx/GXInit.h>
#include <revolution/VI.h>
#include <nw4r/g3d/g3d_state.h>
#include <nw4r/g3d/g3d_init.h>
#include <nw4r/ut/ut_Color.h>

#include <game/Memory.h>

#include <game/RenderMode.h>

namespace Graphics {

static bool sFirstFrame = true;
static GXPixelFmt sPixelFormat = static_cast<GXPixelFmt>(-1);
static GXZFmt16 sDepthFormat = static_cast<GXZFmt16>(-1);
static void* sFifoBuffer;
static GXFifoObj* sFifo;
static void* sFrameBuffers[2];
static void* sCurrentFrameBuffer;

void Initialize(const GXRenderModeObj* pModeInput) {
    VIInit();
    nw4r::g3d::G3dInit(true);
    nw4r::g3d::G3DState::SetRenderModeObj(*(pModeInput != nullptr ? pModeInput : &ChooseDefaultRenderMode()));
    pModeInput = nw4r::g3d::G3DState::GetRenderModeObj();
    u16 bufferWidth = (pModeInput->fbWidth + 15) & ~15;
    u32 bufferSize = bufferWidth * pModeInput->xfbHeight * 2;
    sFifoBuffer = GameMemory::AllocateMEM1(0x40000, 32);
    sFrameBuffers[0] = GameMemory::AllocateMEM1(bufferSize, 32);
    sFrameBuffers[1] = GameMemory::AllocateMEM1(bufferSize, 32);
    sFifo = GXInit(sFifoBuffer, 0x40000);
    GXSetViewport(0.0f, 0.0f, pModeInput->fbWidth, pModeInput->efbHeight, 0.0f, 1.0f);
    GXSetScissor(0, 0, pModeInput->fbWidth, pModeInput->efbHeight);
    u16 height = GXSetDispCopyYScale(GXGetYScaleFactor(pModeInput->efbHeight, pModeInput->xfbHeight));
    GXSetDispCopySrc(0, 0, pModeInput->fbWidth, pModeInput->efbHeight);
    GXSetDispCopyDst(pModeInput->fbWidth, height);
    GXSetCopyFilter(pModeInput->aa, pModeInput->sample_pattern, GX_TRUE, pModeInput->vfilter);
    GXSetDispCopyGamma(GX_GM_1_0);
    GXSetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
    sPixelFormat = GX_PF_RGB8_Z24;
    sDepthFormat = GX_ZC_LINEAR;
    GXColor clearColor = {};
    SetCopyClear(clearColor, 0xffffff, GX_PF_RGB8_Z24);
    GXCopyDisp(sFrameBuffers[1], GX_TRUE);
    VIConfigure(pModeInput);
    VISetNextFrameBuffer(sFrameBuffers[0]);
    sCurrentFrameBuffer = sFrameBuffers[1];
    VIFlush();
    VIWaitForRetrace();
    if (pModeInput->viTVmode & 1)
        VIWaitForRetrace();
}

void CopyFrameBuffer() {
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXSetColorUpdate(GX_TRUE);
    const GXRenderModeObj& mode = *nw4r::g3d::G3DState::GetRenderModeObj();
    u16 height = GXSetDispCopyYScale(GXGetYScaleFactor(mode.efbHeight, mode.xfbHeight));
    GXSetDispCopySrc(0, 0, mode.fbWidth, mode.efbHeight);
    GXSetDispCopyDst(mode.fbWidth, height);
    GXSetCopyFilter(mode.aa, mode.sample_pattern, GX_TRUE, mode.vfilter);
    GXSetDispCopyGamma(GX_GM_1_0);
    GXSetColorUpdate(GX_TRUE);
    GXSetAlphaUpdate(GX_TRUE);
    GXCopyDisp(sCurrentFrameBuffer, GX_TRUE);
    GXDrawDone();
}

void DisplayFrameBuffer() {
    VISetNextFrameBuffer(sCurrentFrameBuffer);
    if (sFirstFrame) {
        VISetBlack(FALSE);
        sFirstFrame = false;
    }
    VIFlush();
    VIWaitForRetrace();
    sCurrentFrameBuffer = sCurrentFrameBuffer == sFrameBuffers[0] ? sFrameBuffers[1] : sFrameBuffers[0];
}

void ResetState() {
    nw4r::g3d::G3dReset();
    GXSetLineWidth(6, GX_TO_ZERO);
    GXSetPointSize(6, GX_TO_ZERO);
    GXEnableTexOffsets(GX_TEXCOORD0, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(GX_TEXCOORD1, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(GX_TEXCOORD2, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(GX_TEXCOORD3, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(GX_TEXCOORD4, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(GX_TEXCOORD5, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(GX_TEXCOORD6, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(GX_TEXCOORD7, GX_FALSE, GX_FALSE);
    GXSetCullMode(GX_CULL_BACK);
    GXSetClipMode(GX_CLIP_ENABLE);
    GXSetCoPlanar(GX_FALSE);
    nw4r::ut::Color fogColor(0xffffffff);
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, fogColor);
    GXSetFogRangeAdj(GX_FALSE, 0, nullptr);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_COPY);
    GXSetColorUpdate(GX_TRUE);
    GXSetAlphaUpdate(GX_TRUE);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXSetZCompLoc(GX_FALSE);
    GXSetDstAlpha(GX_FALSE, 255);
    GXSetNumTevStages(1);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE4, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE5, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE6, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE7, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE8, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE9, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE10, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE11, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE12, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE13, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE14, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE15, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
    GXSetZTexture(GX_ZT_DISABLE, GX_TF_Z8, 0);
    GXSetNumIndStages(0);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevDirect(GX_TEVSTAGE1);
    GXSetTevDirect(GX_TEVSTAGE2);
    GXSetTevDirect(GX_TEVSTAGE3);
    GXSetTevDirect(GX_TEVSTAGE4);
    GXSetTevDirect(GX_TEVSTAGE5);
    GXSetTevDirect(GX_TEVSTAGE6);
    GXSetTevDirect(GX_TEVSTAGE7);
    GXSetTevDirect(GX_TEVSTAGE8);
    GXSetTevDirect(GX_TEVSTAGE9);
    GXSetTevDirect(GX_TEVSTAGE10);
    GXSetTevDirect(GX_TEVSTAGE11);
    GXSetTevDirect(GX_TEVSTAGE12);
    GXSetTevDirect(GX_TEVSTAGE13);
    GXSetTevDirect(GX_TEVSTAGE14);
    GXSetTevDirect(GX_TEVSTAGE15);
    GXSetNumChans(0);
    GXSetNumTexGens(0);
}

void SetPixelFormat(GXPixelFmt pixelFormat, GXZFmt16 depthFormat) {
    GXSetPixelFmt(pixelFormat, depthFormat);
    sPixelFormat = pixelFormat;
    sDepthFormat = depthFormat;
}

void SetCopyClear(const GXColor& color, u32 depth, GXPixelFmt pixelFormat) {
    if (pixelFormat == sPixelFormat) {
        GXSetCopyClear(color, depth);
    } else if (sPixelFormat == GX_PF_RGB8_Z24 && pixelFormat == GX_PF_RGBA6_Z24) {
        GXColor converted = {
            static_cast<u8>((color.r & 0xfc) | (color.g >> 6)),
            static_cast<u8>((color.g << 2) | (color.b >> 4)),
            static_cast<u8>((color.b << 4) | (color.a >> 2)),
            0
        };
        GXSetCopyClear(converted, depth);
    } else if (sPixelFormat == GX_PF_RGBA6_Z24 && pixelFormat == GX_PF_RGB8_Z24) {
        GXColor converted;
        int green = color.g;
        int red = color.r;
        int blue = color.b;
        int packedGreen = (red << 6) | (green >> 2);
        converted.r = red;
        converted.g = packedGreen;
        converted.b = (green << 4) | (blue >> 4);
        converted.a = blue << 2;
        GXSetCopyClear(converted, depth);
    } else {
        GXSetCopyClear(color, depth);
    }
}

}
