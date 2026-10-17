#ifndef NW4R_LYT_MATERIAL_TYPES_H
#define NW4R_LYT_MATERIAL_TYPES_H
#include <nw4r/lyt/lyt_common.h>
namespace nw4r {
namespace lyt {
namespace detail {
template <typename T> inline bool TestBit(T bits, int index) { return (bits & (1 << index)) != 0; }
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
struct TexCoordGen {

    TexCoordGen() {
        reserve = 0;
        Set(GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
    }

    void Set(GXTexGenType aTexGenType, GXTexGenSrc aTexGenSrc, u32 aTexMtx) {
        texGenType = aTexGenType;
        texGenSrc = aTexGenSrc;
        texMtx = aTexMtx;
    }

    GXTexGenType GetTexGenType() const {
        return (GXTexGenType)texGenType;
    }

    GXTexGenSrc GetTexGenSrc() const {
        return (GXTexGenSrc)texGenSrc;
    }

    u32 GetTexMtx() const {
        return texMtx;
    }

    u8 texGenType;
    u8 texGenSrc;
    u8 texMtx;
    u8 reserve;
};
struct ChanCtrl {

    ChanCtrl() : reserve1(0), reserve2(0) {
        Set(GX_SRC_VTX, GX_SRC_VTX);
    }

    void Set(GXColorSrc colSrc, GXColorSrc alpSrc) {
        matSrcCol = colSrc;
        matSrcAlp = alpSrc;
    }

    GXColorSrc GetColorSrc() const {
        return (GXColorSrc)matSrcCol;
    }

    GXColorSrc GetAlphaSrc() const {
        return (GXColorSrc)matSrcAlp;
    }

    u8 matSrcCol;
    u8 matSrcAlp;
    u8 reserve1;
    u8 reserve2;
};
struct AlphaCompare {

    AlphaCompare() {
        Set(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    }

    void Set(GXCompare aComp0, u8 aRef0, GXAlphaOp aOp, GXCompare aComp1, u8 aRef1) {
        comp = (aComp0 & 0x7) | ((aComp1 & 0x7) << 4);
        op = aOp;
        ref0 = aRef0;
        ref1 = aRef1;
    }

    GXCompare GetComp0() const {
        return (GXCompare)((comp >> 0) & 0xF);
    }

    u8 GetRef0() const {
        return ref0;
    }

    GXAlphaOp GetOp() const {
        return (GXAlphaOp)op;
    }

    GXCompare GetComp1() const {
        return (GXCompare)((comp >> 4) & 0xF);
    }

    u8 GetRef1() const {
        return ref1;
    }

    u8 comp;
    u8 op;
    u8 ref0;
    u8 ref1;
};
struct BlendMode {

    BlendMode() {
        Set(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
    }

    void Set(GXBlendMode aType, GXBlendFactor aSrcFactor, GXBlendFactor aDestFactor, GXLogicOp aOp) {
        type = aType;
        srcFactor = aSrcFactor;
        dstFactor = aDestFactor;
        op = aOp;
    }

    GXBlendMode GetType() const {
        return GXBlendMode(type);
    }

    GXBlendFactor GetSrcFactor() const {
        return (GXBlendFactor)srcFactor;
    }

    GXBlendFactor GetDstFactor() const {
        return (GXBlendFactor)dstFactor;
    }

    GXLogicOp GetOp() const {
        return (GXLogicOp)op;
    }

    u8 type;
    u8 srcFactor;
    u8 dstFactor;
    u8 op;
};
struct IndirectStage {

    IndirectStage() {
        Set(GX_TEXCOORD0, GX_TEXMAP0, GX_ITS_1, GX_ITS_1);
    }

    void Set(GXTexCoordID aTexCoordGen, GXTexMapID aTexMap, GXIndTexScale aScaleS, GXIndTexScale aScaleT) {
        texCoordGen = aTexCoordGen;
        texMap = aTexMap;
        scaleS = aScaleS;
        scaleT = aScaleT;
    }

    GXTexCoordID GetTexCoordGen() const {
        return (GXTexCoordID)texCoordGen;
    }

    GXTexMapID GetTexMap() const {
        return (GXTexMapID)texMap;
    }

    GXIndTexScale GetScaleS() const {
        return (GXIndTexScale)scaleS;
    }

    GXIndTexScale GetScaleT() const {
        return (GXIndTexScale)scaleT;
    }

    u8 texCoordGen;
    u8 texMap;
    u8 scaleS;
    u8 scaleT;
};
struct TexSRT {

    math::VEC2 translate;
    f32 rotate;
    math::VEC2 scale;
};
struct TevStageInOp {

    u8 GetA() const {
        return (ab >> 0) & 0xF;
    }

    u8 GetB() const {
        return (ab >> 4) & 0xF;
    }

    u8 GetC() const {
        return (cd >> 0) & 0xF;
    }

    u8 GetD() const {
        return (cd >> 4) & 0xF;
    }

    u8 GetOp() const {
        return (op >> 0) & 0xF;
    }

    u8 GetBias() const {
        return (op >> 4) & 0x3;
    }

    u8 GetScale() const {
        return (op >> 6) & 0x3;
    }

    bool IsClamp() const {
        return (cl >> 0) & 1;
    }

    u8 GetOutReg() const {
        return (cl >> 1) & 0x3;
    }

    u8 GetKSel() const {
        return (cl >> 3) & 0x1F;
    }

    void SetIn(u8 a, u8 b, u8 c, u8 d) {
        ab = (a & 0xF) | ((b & 0xF) << 4);
        cd = (c & 0xF) | ((d & 0xF) << 4);
    }

    void SetOp(u8 aOp, u8 bias, u8 scale, bool clamp, u8 outReg, u8 kSel) {
        op = (aOp & 0xF) | ((bias & 3) << 4) | ((scale & 0x3) << 6);
        cl = (clamp & 1) | ((outReg & 0x3) << 1) | ((kSel & 0x1F) << 3);
    }

    u8 ab;
    u8 cd;
    u8 op;
    u8 cl;
};
struct TevStage {

    TevStage() {
        SetOrder(GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0, GX_TEV_SWAP0, GX_TEV_SWAP0);
        SetColorIn(GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
        SetAlphaIn(GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
        SetColorOp(GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV, GX_TEV_KCSEL_K0);
        SetAlphaOp(GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV, GX_TEV_KASEL_K0_R);
        SetIndirect(
            GX_INDTEXSTAGE0, GX_ITF_8, GX_ITB_NONE, GX_ITM_OFF, GX_ITW_OFF, GX_ITW_OFF, false, false, GX_ITBA_OFF
        );
    }

    GXTexCoordID GetTexCoordGen() const {
        return (GXTexCoordID)texCoordGen;
    }

    GXTexMapID GetTexMap() const {
        return (GXTexMapID)(texMap | (swapSel & 1) << 8);
    }

    GXChannelID GetColorChan() const {
        return (GXChannelID)colChan;
    }

    GXTevSwapSel GetRasSwapSel() const {
        return (GXTevSwapSel)((swapSel >> 1) & 3);
    }

    GXTevSwapSel GetTexSwapSel() const {
        return (GXTevSwapSel)((swapSel >> 3) & 3);
    }

    GXTevColorArg GetColorInA() const {
        return (GXTevColorArg)colIn.GetA();
    }

    GXTevColorArg GetColorInB() const {
        return (GXTevColorArg)colIn.GetB();
    }

    GXTevColorArg GetColorInC() const {
        return (GXTevColorArg)colIn.GetC();
    }

    GXTevColorArg GetColorInD() const {
        return (GXTevColorArg)colIn.GetD();
    }

    GXTevOp GetColorOp() const {
        return (GXTevOp)colIn.GetOp();
    }

    GXTevBias GetColorBias() const {
        return (GXTevBias)colIn.GetBias();
    }

    GXTevScale GetColorScale() const {
        return (GXTevScale)colIn.GetScale();
    }

    bool IsColorClamp() const {
        return colIn.IsClamp();
    }

    GXTevRegID GetColorOutReg() const {
        return (GXTevRegID)colIn.GetOutReg();
    }

    GXTevKColorSel GetKColorSel() const {
        return (GXTevKColorSel)colIn.GetKSel();
    }

    GXTevAlphaArg GetAlphaInA() const {
        return (GXTevAlphaArg)alpIn.GetA();
    }

    GXTevAlphaArg GetAlphaInB() const {
        return (GXTevAlphaArg)alpIn.GetB();
    }

    GXTevAlphaArg GetAlphaInC() const {
        return (GXTevAlphaArg)alpIn.GetC();
    }

    GXTevAlphaArg GetAlphaInD() const {
        return (GXTevAlphaArg)alpIn.GetD();
    }

    GXTevOp GetAlphaOp() const {
        return (GXTevOp)alpIn.GetOp();
    }

    GXTevBias GetAlphaBias() const {
        return (GXTevBias)alpIn.GetBias();
    }

    GXTevScale GetAlphaScale() const {
        return (GXTevScale)alpIn.GetScale();
    }

    bool IsAlphaClamp() const {
        return alpIn.IsClamp();
    }

    GXTevRegID GetAlphaOutReg() const {
        return (GXTevRegID)alpIn.GetOutReg();
    }

    GXTevKAlphaSel GetKAlphaSel() const {
        return (GXTevKAlphaSel)alpIn.GetKSel();
    }

    GXIndTexMtxID GetIndMtxSel() const {
        return (GXIndTexMtxID)((inBiMt >> 3) & 0xF);
    }

    GXIndTexStageID GetIndStage() const {
        return (GXIndTexStageID)indStage;
    }

    GXIndTexFormat GetIndFormat() const {
        return (GXIndTexFormat)((indFoAdUtAl >> 0) & 0x3);
    }

    GXIndTexBiasSel GetIndBiasSel() const {
        return (GXIndTexBiasSel)((inBiMt >> 0) & 0x7);
    }

    GXIndTexWrap GetIndWrapS() const {
        return (GXIndTexWrap)((indWrap >> 0) & 0x7);
    }

    GXIndTexWrap GetIndWrapT() const {
        return (GXIndTexWrap)((indWrap >> 3) & 0x7);
    }

    u8 IsIndAddPrev() const {
        return ((indFoAdUtAl >> 2) & 1);
    }

    u8 IsIndUtcLod() const {
        return ((indFoAdUtAl >> 3) & 1);
    }

    GXIndTexAlphaSel GetIndAlphaSel() const {
        return (GXIndTexAlphaSel)((indFoAdUtAl >> 4) & 3);
    }

    void SetOrder(
        GXTexCoordID aTexCoordGen, GXTexMapID aTexMap, GXChannelID aColChan, GXTevSwapSel rasSel, GXTevSwapSel texSel
    ) {
        texCoordGen = aTexCoordGen;
        colChan = aColChan;
        texMap = aTexMap;
        swapSel = ((rasSel & 3) << 1) | ((texSel & 3) << 3);
    }

    void SetColorIn(GXTevColorArg a, GXTevColorArg b, GXTevColorArg c, GXTevColorArg d) {
        colIn.SetIn(a, b, c, d);
    }

    void SetAlphaIn(GXTevAlphaArg a, GXTevAlphaArg b, GXTevAlphaArg c, GXTevAlphaArg d) {
        alpIn.SetIn(a, b, c, d);
    }

    void SetColorOp(GXTevOp op, GXTevBias bias, GXTevScale scale, bool clamp, GXTevRegID outReg, GXTevKColorSel kSel) {
        colIn.SetOp(op, bias, scale, clamp, outReg, kSel);
    }

    void SetAlphaOp(GXTevOp op, GXTevBias bias, GXTevScale scale, bool clamp, GXTevRegID outReg, GXTevKAlphaSel kSel) {
        alpIn.SetOp(op, bias, scale, clamp, outReg, kSel);
    }

    void SetIndirect(
        GXIndTexStageID stage, GXIndTexFormat format, GXIndTexBiasSel bias, GXIndTexMtxID mtxSel, GXIndTexWrap wrapS,
        GXIndTexWrap wrapT, bool addPrev, bool utcLod, GXIndTexAlphaSel alphaSel
    ) {
        indStage = stage;
        inBiMt = (bias & 0x7) | ((mtxSel & 0xF) << 4);
        indWrap = (wrapS & 0x7) | ((wrapT & 0x7) << 3);
        indFoAdUtAl = (format & 3) | (addPrev << 2) | (utcLod << 3) | ((alphaSel & 0x3) << 4);
    }

    u8 texCoordGen;
    u8 colChan;
    u8 texMap;
    u8 swapSel;
    TevStageInOp colIn;
    TevStageInOp alpIn;
    u8 indStage;
    u8 inBiMt;
    u8 indWrap;
    u8 indFoAdUtAl;
};
struct TevSwapMode {

    GXTevColorChan GetR() const {
        return (GXTevColorChan)((swap >> 0) & 0x3);
    }

    GXTevColorChan GetG() const {
        return (GXTevColorChan)((swap >> 2) & 0x3);
    }

    GXTevColorChan GetB() const {
        return (GXTevColorChan)((swap >> 4) & 0x3);
    }

    GXTevColorChan GetA() const {
        return (GXTevColorChan)((swap >> 6) & 0x3);
    }

    void Set(GXTevColorChan r, GXTevColorChan g, GXTevColorChan b, GXTevColorChan a) {
        swap = (r << 0) | (g << 2) | (b << 4) | (a << 6);
    }

    u8 swap;
};
}
}
#endif
