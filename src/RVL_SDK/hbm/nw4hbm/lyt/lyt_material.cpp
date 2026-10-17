#include "nw4hbm/lyt/lyt_material.h"
#include "nw4hbm/lyt/lyt_animation.h"
#include "nw4hbm/lyt/lyt_common.h"
#include "nw4hbm/lyt/lyt_layout.h"
#include "nw4hbm/lyt/lyt_resourceAccessor.h"
#include "nw4hbm/math/math_arithmetic.h"
#include "nw4hbm/math/math_triangular.h"

#include "revolution/gx/GXBump.h"
#include "revolution/gx/GXGet.h"
#include "revolution/gx/GXTexture.h"
#include "revolution/gx/GXTransform.h"

#include <string.h>

namespace {

using namespace nw4hbm;
using namespace nw4hbm::lyt;

const GXColorS10 DefaultBlackColor = {0x0000, 0x0000, 0x0000, 0x0000};
const GXColorS10 DefaultWhiteColor = {0x00FF, 0x00FF, 0x00FF, 0x00FF};

bool operator==(const GXColorS10& rLhs, const GXColorS10& rRhs) {
    return rLhs.r == rRhs.r && rLhs.g == rRhs.g && rLhs.b == rRhs.b && rLhs.a == rRhs.a;
}

bool operator!=(const GXColorS10& rLhs, const GXColorS10& rRhs) {
    return !(rLhs == rRhs);
}

void CalcTextureMtx(math::MTX34* pMtx, const TexSRT& rTexSRT) {
    math::VEC2 center(0.5f, 0.5f);

    f32 cosR = math::CosDeg(rTexSRT.rotate);
    f32 sinR = math::SinDeg(rTexSRT.rotate);

    f32 a0 = cosR * rTexSRT.scale.x;
    f32 a1 = -sinR * rTexSRT.scale.y;
    pMtx->m[0][0] = a0;
    pMtx->m[0][1] = a1;
    pMtx->m[0][2] = 0.0f;
    pMtx->m[0][3] = rTexSRT.translate.x + center.x + a0 * -center.x + a1 * -center.y;

    a0 = sinR * rTexSRT.scale.x;
    a1 = cosR * rTexSRT.scale.y;
    pMtx->m[1][0] = a0;
    pMtx->m[1][1] = a1;
    pMtx->m[1][2] = 0.0f;
    pMtx->m[1][3] = rTexSRT.translate.y + center.y + a0 * -center.x + a1 * -center.y;

    pMtx->m[2][0] = 0.0f;
    pMtx->m[2][1] = 0.0f;
    pMtx->m[2][2] = 1.0f;
    pMtx->m[2][3] = 0.0f;
}

void CalcIndTexMtx(Mtx23 mtx, const TexSRT& rTexSRT) {
    f32 cosR = math::CosDeg(rTexSRT.rotate);
    f32 sinR = math::SinDeg(rTexSRT.rotate);

    mtx[0][0] = cosR * rTexSRT.scale.x;
    mtx[0][1] = -sinR * rTexSRT.scale.y;
    mtx[0][2] = rTexSRT.translate.x;

    mtx[1][0] = sinR * rTexSRT.scale.x;
    mtx[1][1] = cosR * rTexSRT.scale.y;
    mtx[1][2] = rTexSRT.translate.y;
}

void SetColorComponentValue(ut::Color* pColor, u32 component, s16 value) {
    const u8 clamped = ut::Min<s16>(ut::Max<s16>(value, 0), 0xFF);

    switch (component) {
    case 0:
        pColor->r = clamped;
        break;
    case 1:
        pColor->g = clamped;
        break;
    case 2:
        pColor->b = clamped;
        break;
    case 3:
        pColor->a = clamped;
        break;
    }
}

void SetIndTexMtx(GXIndTexMtxID id, const Mtx23 mtx) {
    f32 m00, m01, m02;
    f32 m10, m11, m12;

    f32 a00, a01, a02;
    f32 a10, a11, a12;

    s8 scaleExp = 0;

    m00 = mtx[0][0];
    m01 = mtx[0][1];
    m02 = mtx[0][2];
    m10 = mtx[1][0];
    m11 = mtx[1][1];
    m12 = mtx[1][2];

    a00 = math::FAbs(m00);
    a01 = math::FAbs(m01);
    a02 = math::FAbs(m02);
    a10 = math::FAbs(m10);
    a11 = math::FAbs(m11);
    a12 = math::FAbs(m12);

    if (a00 >= 1.0f || a01 >= 1.0f || a02 >= 1.0f || a10 >= 1.0f || a11 >= 1.0f || a12 >= 1.0f) {
        do {
            if (scaleExp >= 46) {
                break;
            }

            scaleExp++;

            m00 /= 2.0f;
            m01 /= 2.0f;
            m02 /= 2.0f;
            m10 /= 2.0f;
            m11 /= 2.0f;
            m12 /= 2.0f;

            a00 /= 2.0f;
            a01 /= 2.0f;
            a02 /= 2.0f;
            a10 /= 2.0f;
            a11 /= 2.0f;
            a12 /= 2.0f;
        } while (a00 >= 1.0f || a01 >= 1.0f || a02 >= 1.0f || a10 >= 1.0f || a11 >= 1.0f || a12 >= 1.0f);
    } else if (a00 < 0.5f && a01 < 0.5f && a02 < 0.5f && a10 < 0.5f && a11 < 0.5f && a12 < 0.5f) {
        do {
            scaleExp--;

            m00 *= 2.0f;
            m01 *= 2.0f;
            m02 *= 2.0f;
            m10 *= 2.0f;
            m11 *= 2.0f;
            m12 *= 2.0f;

            a00 *= 2.0f;
            a01 *= 2.0f;
            a02 *= 2.0f;
            a10 *= 2.0f;
            a11 *= 2.0f;
            a12 *= 2.0f;
        } while (a00 < 0.5f && a01 < 0.5f && a02 < 0.5f && a10 < 0.5f && a11 < 0.5f && a12 < 0.5f && scaleExp > -17);
    }

    Mtx23 outMtx = {{m00, m01, m02}, {m10, m11, m12}};

    GXSetIndTexMtx(id, outMtx, scaleExp);
}

void InitTexSRT(TexSRT* pTexSRTs, u32 num) {
    for (u32 i = 0; i < num; i++) {
        pTexSRTs[i].translate = math::VEC2(0.0f, 0.0f);
        pTexSRTs[i].rotate = 0.0f;
        pTexSRTs[i].scale = math::VEC2(1.0f, 1.0f);
    }
}

u32 CalcOffsetTexSRTAry(const detail::BitGXNums& rNums) {
    return sizeof(GXTexObj) * rNums.texMap;
}

u32 CalcOffsetTexCoordGenAry(const detail::BitGXNums& rNums) {
    return CalcOffsetTexSRTAry(rNums) + sizeof(TexSRT) * rNums.texSRT;
}

u32 CalcOffsetChanCtrlAry(const detail::BitGXNums& rNums) {
    return CalcOffsetTexCoordGenAry(rNums) + sizeof(TexCoordGen) * rNums.texCoordGen;
}

u32 CalcOffsetMatColAry(const detail::BitGXNums& rNums) {
    return CalcOffsetChanCtrlAry(rNums) + sizeof(ChanCtrl) * rNums.chanCtrl;
}

u32 CalcOffsetTevSwapAry(const detail::BitGXNums& rNums) {
    return CalcOffsetMatColAry(rNums) + sizeof(ut::Color) * rNums.matCol;
}

u32 CalcOffsetGetAlphaCompare(const detail::BitGXNums& rNums) {
    return CalcOffsetTevSwapAry(rNums) + sizeof(TevSwapMode) * GX_MAX_TEVSWAP * rNums.tevSwap;
}

u32 CalcOffsetBlendMode(const detail::BitGXNums& rNums) {
    return CalcOffsetGetAlphaCompare(rNums) + sizeof(AlphaCompare) * rNums.alpComp;
}

u32 CalcOffsetIndirectStageAry(const detail::BitGXNums& rNums) {
    return CalcOffsetBlendMode(rNums) + sizeof(BlendMode) * rNums.blendMode;
}

u32 CalcOffsetIndTexSRTAry(const detail::BitGXNums& rNums) {
    return CalcOffsetIndirectStageAry(rNums) + sizeof(IndirectStage) * rNums.indStage;
}

u32 CalcOffsetTevStageAry(const detail::BitGXNums& rNums) {
    return CalcOffsetIndTexSRTAry(rNums) + sizeof(TexSRT) * rNums.indSRT;
}

void CopyGXTexObj(GXTexObj* pDst, const GXTexObj* pSrc) {
    *pDst = *pSrc;
    GXInitTexObjUserData(pDst, nullptr);
}

u32 GetTexMtx(u32 idx) {
    return GX_TEXMTX0 + idx * (GX_TEXMTX1 - GX_TEXMTX0);
}

u32 GetTexMtxIdx(u32 texMtx) {
    return (texMtx - GX_TEXMTX0) / (GX_TEXMTX1 - GX_TEXMTX0);
}

}

namespace nw4hbm {
namespace lyt {

Material::Material() {
    Init();
    memset(mName, 0, sizeof(mName));
}

Material::Material(const res::Material* pRes, const ResBlockSet& rBlockSet) {
    Init();
    SetName(pRes->name);

    for (int i = 0; i < TEVCOLOR_MAX; i++) {
        mTevCols[i] = pRes->tevCols[i];
    }

    for (int i = 0; i < GX_MAX_KCOLOR; i++) {
        mTevKCols[i] = pRes->tevKCols[i].ToU32();
    }

    u32 resOffs = sizeof(res::Material);

    const res::TexMap* pResTexMaps = detail::ConvertOffsToPtr<res::TexMap>(pRes, resOffs);
    resOffs += sizeof(res::TexMap) * pRes->resNum.GetTexMapNum();

    const TexSRT* pResTexSRTs = detail::ConvertOffsToPtr<TexSRT>(pRes, resOffs);
    resOffs += sizeof(TexSRT) * pRes->resNum.GetTexSRTNum();

    const TexCoordGen* pResTexCoordGens = detail::ConvertOffsToPtr<TexCoordGen>(pRes, resOffs);
    resOffs += sizeof(TexCoordGen) * pRes->resNum.GetTexCoordGenNum();

    u8 texMapNum = ut::Min<u8>(pRes->resNum.GetTexMapNum(), GX_MAX_TEXMAP);
    u8 texSRTNum = ut::Min<u8>(pRes->resNum.GetTexSRTNum(), 10);
    u8 texCoordGenNum = ut::Min<u8>(pRes->resNum.GetTexCoordGenNum(), GX_MAX_TEXCOORD);
    bool allocChanCtrl = pRes->resNum.GetChanCtrlNum() != 0;
    bool allocMatCol = pRes->resNum.GetMatColNum() != 0;
    bool allocTevSwap = pRes->resNum.HasTevSwapTable();
    bool allocAlpComp = pRes->resNum.HasAlphaCompare();
    bool allocBlendMode = pRes->resNum.HasBlendMode();
    u8 indTexSRTNum = ut::Min<u8>(pRes->resNum.GetIndTexSRTNum(), 3);
    u8 indStageNum = ut::Min<u8>(pRes->resNum.GetIndTexStageNum(), GX_MAX_INDTEXSTAGE);
    u8 tevStageNum = ut::Min<u8>(pRes->resNum.GetTevStageNum(), GX_MAX_TEVSTAGE);

    ReserveGXMem(texMapNum, texSRTNum, texCoordGenNum, tevStageNum, allocTevSwap, indStageNum, indTexSRTNum,
                 allocChanCtrl, allocMatCol, allocAlpComp, allocBlendMode);

    if (mpGXMem) {
        SetTextureNum(texMapNum);

        if (texMapNum) {
            const res::Texture* const pTextures =
                detail::ConvertOffsToPtr<res::Texture>(rBlockSet.pTextureList, sizeof(res::TextureList));
            GXTexObj* const pTexObjs = GetTexMapAry();

            for (u8 i = 0; i < mGXMemNum.texMap; i++) {
                const char* pFileName =
                    detail::ConvertOffsToPtr<char>(pTextures, pTextures[pResTexMaps[i].texIdx].nameStrOffset);
                void* pTplRes = rBlockSet.pResAccessor->GetResource('timg', pFileName, nullptr);

                SetTexture(i, static_cast<TPLPalette*>(pTplRes));
                GXInitTexObjWrapMode(&pTexObjs[i], static_cast<GXTexWrapMode>(pResTexMaps[i].wrapS),
                                     static_cast<GXTexWrapMode>(pResTexMaps[i].wrapT));
            }
        }

        TexSRT* pTexSRTs = GetTexSRTAry();
        for (int i = 0; i < texSRTNum; i++) {
            pTexSRTs[i].translate = pResTexSRTs[i].translate;
            pTexSRTs[i].rotate = pResTexSRTs[i].rotate;
            pTexSRTs[i].scale = pResTexSRTs[i].scale;
        }

        TexCoordGen* pTexCoordGens = GetTexCoordGenAry();
        if (texCoordGenNum) {
            for (u32 i = mGXMemNum.texCoordGen; i < texCoordGenNum; i++) {
                pTexCoordGens[i] = TexCoordGen();
            }

            mGXMemNum.texCoordGen = texCoordGenNum;
        }

        for (int i = 0; i < mGXMemNum.texCoordGen; i++) {
            pTexCoordGens[i] = pResTexCoordGens[i];
        }

        if (allocChanCtrl) {
            *GetChanCtrlAry() = *detail::ConvertOffsToPtr<ChanCtrl>(pRes, resOffs);
            resOffs += sizeof(ChanCtrl);
        }

        if (allocMatCol) {
            *GetMatColAry() = *detail::ConvertOffsToPtr<ut::Color>(pRes, resOffs);
            resOffs += sizeof(ut::Color);
        }

        if (allocTevSwap) {
            const TevSwapMode* pResTevSwaps = detail::ConvertOffsToPtr<TevSwapMode>(pRes, resOffs);
            TevSwapMode* pTevSwaps = GetTevSwapAry();

            for (int i = 0; i < GX_MAX_TEVSWAP; i++) {
                pTevSwaps[i] = pResTevSwaps[i];
            }

            resOffs += sizeof(TevSwapMode) * GX_MAX_TEVSWAP;
        }

        if (indTexSRTNum) {
            TexSRT* pIndTexSRTs = GetIndTexSRTAry();
            const TexSRT* pResIndTexSRTs = detail::ConvertOffsToPtr<TexSRT>(pRes, resOffs);

            for (int i = 0; i < indTexSRTNum; i++) {
                pIndTexSRTs[i] = pResIndTexSRTs[i];
            }
        }
        resOffs += sizeof(TexSRT) * pRes->resNum.GetIndTexSRTNum();

        if (indStageNum) {
            SetIndStageNum(indStageNum);

            IndirectStage* pIndStages = GetIndirectStageAry();
            const IndirectStage* pResIndStages = detail::ConvertOffsToPtr<IndirectStage>(pRes, resOffs);

            for (int i = 0; i < indStageNum; i++) {
                pIndStages[i] = pResIndStages[i];
            }
        }
        resOffs += sizeof(IndirectStage) * pRes->resNum.GetIndTexStageNum();

        if (tevStageNum) {
            SetTevStageNum(tevStageNum);

            TevStage* pTevStages = GetTevStageAry();
            const TevStage* pResTevStages = detail::ConvertOffsToPtr<TevStage>(pRes, resOffs);

            for (int i = 0; i < tevStageNum; i++) {
                pTevStages[i] = pResTevStages[i];
            }
        }
        resOffs += sizeof(TevStage) * pRes->resNum.GetTevStageNum();

        if (allocAlpComp) {
            *GetAlphaComparePtr() = *detail::ConvertOffsToPtr<AlphaCompare>(pRes, resOffs);
            resOffs += sizeof(AlphaCompare);
        }

        if (allocBlendMode) {
            *GetBlendModePtr() = *detail::ConvertOffsToPtr<BlendMode>(pRes, resOffs);
            resOffs += sizeof(BlendMode);
        }
    }
}

void Material::Init() {
    mTevCols[TEVCOLOR_REG0] = DefaultBlackColor;
    mTevCols[TEVCOLOR_REG1] = DefaultWhiteColor;
    mTevCols[TEVCOLOR_REG2] = DefaultWhiteColor;

    InitBitGXNums(&mGXMemCap);
    InitBitGXNums(&mGXMemNum);

    mbUserAllocated = false;
    mpGXMem = nullptr;
}

Material::~Material() {
    UnbindAllAnimation();

    if (mpGXMem) {
        Layout::FreeMemory(mpGXMem);
        mpGXMem = nullptr;
    }
}

void Material::InitBitGXNums(detail::BitGXNums* pNums) {
    pNums->texMap = 0;
    pNums->texSRT = 0;
    pNums->texCoordGen = 0;
    pNums->indSRT = 0;
    pNums->indStage = 0;
    pNums->tevSwap = 0;
    pNums->tevStage = 0;
    pNums->chanCtrl = 0;
    pNums->matCol = 0;
    pNums->alpComp = 0;
    pNums->blendMode = 0;
}

void Material::ReserveGXMem(u8 texMapNum, u8 texSRTNum, u8 texCoordGenNum, u8 tevStageNum, bool allocTevSwap,
                            u8 indStageNum, u8 indSRTNum, bool allocChanCtrl, bool allocMatCol, bool allocAlpComp,
                            bool allocBlendMode) {
    int tevSwapNum = allocTevSwap ? 1 : 0;
    int chanCtrlNum = allocChanCtrl ? 1 : 0;
    int matColNum = allocMatCol ? 1 : 0;
    int alpCompNum = allocAlpComp ? 1 : 0;
    int blendModeNum = allocBlendMode ? 1 : 0;

    if (mGXMemCap.texMap >= texMapNum && mGXMemCap.texSRT >= texSRTNum && mGXMemCap.texCoordGen >= texCoordGenNum &&
        mGXMemCap.tevStage >= tevStageNum && mGXMemCap.tevSwap >= tevSwapNum && mGXMemCap.indStage >= indStageNum &&
        mGXMemCap.indSRT >= indSRTNum && mGXMemCap.chanCtrl >= chanCtrlNum && mGXMemCap.matCol >= matColNum &&
        mGXMemCap.alpComp >= alpCompNum && mGXMemCap.blendMode >= blendModeNum) {
        return;
    }

    if (mpGXMem) {
        Layout::FreeMemory(mpGXMem);
        mpGXMem = nullptr;

        InitBitGXNums(&mGXMemCap);
        InitBitGXNums(&mGXMemNum);
    }

    mpGXMem = Layout::AllocMemory(4 * (matColNum + chanCtrlNum + texCoordGenNum + tevSwapNum + alpCompNum +
                                       blendModeNum + indStageNum) +
                                  static_cast<int>(sizeof(TexSRT)) * texSRTNum +
                                  static_cast<int>(sizeof(GXTexObj)) * texMapNum +
                                  static_cast<int>(sizeof(TexSRT)) * indSRTNum +
                                  static_cast<int>(sizeof(TevStage)) * tevStageNum);

    if (!mpGXMem) {
        return;
    }

    mGXMemCap.texMap = texMapNum;
    mGXMemCap.texSRT = texSRTNum;
    mGXMemCap.texCoordGen = texCoordGenNum;
    mGXMemCap.indSRT = indSRTNum;
    mGXMemCap.indStage = indStageNum;
    mGXMemCap.tevSwap = tevSwapNum;
    mGXMemCap.tevStage = tevStageNum;
    mGXMemCap.chanCtrl = chanCtrlNum;
    mGXMemCap.matCol = matColNum;
    mGXMemCap.alpComp = alpCompNum;
    mGXMemCap.blendMode = blendModeNum;

    mGXMemNum.texSRT = mGXMemCap.texSRT;
    InitTexSRT(GetTexSRTAry(), mGXMemNum.texSRT);

    mGXMemNum.indSRT = mGXMemCap.indSRT;
    InitTexSRT(GetIndTexSRTAry(), mGXMemNum.indSRT);

    mGXMemNum.chanCtrl = mGXMemCap.chanCtrl;
    if (mGXMemNum.chanCtrl) {
        *GetChanCtrlAry() = ChanCtrl();
    }

    mGXMemNum.matCol = mGXMemCap.matCol;
    if (mGXMemNum.matCol) {
        *GetMatColAry() = 0xFFFFFFFF;
    }

    mGXMemNum.tevSwap = mGXMemCap.tevSwap;
    if (mGXMemNum.tevSwap) {
        TevSwapMode* pTevSwaps = GetTevSwapAry();
        pTevSwaps[GX_TEV_SWAP0].Set(GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
        pTevSwaps[GX_TEV_SWAP1].Set(GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
        pTevSwaps[GX_TEV_SWAP2].Set(GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
        pTevSwaps[GX_TEV_SWAP3].Set(GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
    }

    mGXMemNum.alpComp = mGXMemCap.alpComp;
    if (mGXMemNum.alpComp) {
        *GetAlphaComparePtr() = AlphaCompare();
    }

    mGXMemNum.blendMode = mGXMemCap.blendMode;
    if (mGXMemNum.blendMode) {
        *GetBlendModePtr() = BlendMode();
    }
}

const GXTexObj* Material::GetTexMapAry() const {
    return detail::ConvertOffsToPtr<GXTexObj>(mpGXMem, 0);
}

GXTexObj* Material::GetTexMapAry() {
    return detail::ConvertOffsToPtr<GXTexObj>(mpGXMem, 0);
}

const TexSRT* Material::GetTexSRTAry() const {
    return detail::ConvertOffsToPtr<TexSRT>(mpGXMem, CalcOffsetTexSRTAry(mGXMemCap));
}

TexSRT* Material::GetTexSRTAry() {
    return detail::ConvertOffsToPtr<TexSRT>(mpGXMem, CalcOffsetTexSRTAry(mGXMemCap));
}

const TexCoordGen* Material::GetTexCoordGenAry() const {
    return detail::ConvertOffsToPtr<TexCoordGen>(mpGXMem, CalcOffsetTexCoordGenAry(mGXMemCap));
}

TexCoordGen* Material::GetTexCoordGenAry() {
    return detail::ConvertOffsToPtr<TexCoordGen>(mpGXMem, CalcOffsetTexCoordGenAry(mGXMemCap));
}

const ChanCtrl* Material::GetChanCtrlAry() const {
    return detail::ConvertOffsToPtr<ChanCtrl>(mpGXMem, CalcOffsetChanCtrlAry(mGXMemCap));
}

ChanCtrl* Material::GetChanCtrlAry() {
    return detail::ConvertOffsToPtr<ChanCtrl>(mpGXMem, CalcOffsetChanCtrlAry(mGXMemCap));
}

const ut::Color* Material::GetMatColAry() const {
    return detail::ConvertOffsToPtr<ut::Color>(mpGXMem, CalcOffsetMatColAry(mGXMemCap));
}

ut::Color* Material::GetMatColAry() {
    return detail::ConvertOffsToPtr<ut::Color>(mpGXMem, CalcOffsetMatColAry(mGXMemCap));
}

const TevSwapMode* Material::GetTevSwapAry() const {
    return detail::ConvertOffsToPtr<TevSwapMode>(mpGXMem, CalcOffsetTevSwapAry(mGXMemCap));
}

TevSwapMode* Material::GetTevSwapAry() {
    return detail::ConvertOffsToPtr<TevSwapMode>(mpGXMem, CalcOffsetTevSwapAry(mGXMemCap));
}

const AlphaCompare* Material::GetAlphaComparePtr() const {
    return detail::ConvertOffsToPtr<AlphaCompare>(mpGXMem, CalcOffsetGetAlphaCompare(mGXMemCap));
}

AlphaCompare* Material::GetAlphaComparePtr() {
    return detail::ConvertOffsToPtr<AlphaCompare>(mpGXMem, CalcOffsetGetAlphaCompare(mGXMemCap));
}

const BlendMode* Material::GetBlendModePtr() const {
    return detail::ConvertOffsToPtr<BlendMode>(mpGXMem, CalcOffsetBlendMode(mGXMemCap));
}

BlendMode* Material::GetBlendModePtr() {
    return detail::ConvertOffsToPtr<BlendMode>(mpGXMem, CalcOffsetBlendMode(mGXMemCap));
}

const IndirectStage* Material::GetIndirectStageAry() const {
    return detail::ConvertOffsToPtr<IndirectStage>(mpGXMem, CalcOffsetIndirectStageAry(mGXMemCap));
}

IndirectStage* Material::GetIndirectStageAry() {
    return detail::ConvertOffsToPtr<IndirectStage>(mpGXMem, CalcOffsetIndirectStageAry(mGXMemCap));
}

const TexSRT* Material::GetIndTexSRTAry() const {
    return detail::ConvertOffsToPtr<TexSRT>(mpGXMem, CalcOffsetIndTexSRTAry(mGXMemCap));
}

TexSRT* Material::GetIndTexSRTAry() {
    return detail::ConvertOffsToPtr<TexSRT>(mpGXMem, CalcOffsetIndTexSRTAry(mGXMemCap));
}

const TevStage* Material::GetTevStageAry() const {
    return detail::ConvertOffsToPtr<TevStage>(mpGXMem, CalcOffsetTevStageAry(mGXMemCap));
}

TevStage* Material::GetTevStageAry() {
    return detail::ConvertOffsToPtr<TevStage>(mpGXMem, CalcOffsetTevStageAry(mGXMemCap));
}

void Material::SetName(const char* pName) {
    strncpy(mName, pName, sizeof(mName));
}

void Material::SetTextureNum(u8 num) {
    if (num) {
        GXTexObj* pTexObjs = GetTexMapAry();

        for (u32 i = mGXMemNum.texMap; i < num; i++) {
            memset(&pTexObjs[i], 0, sizeof(GXTexObj));
        }

        mGXMemNum.texMap = num;
    }
}

void Material::SetTexCoordGenNum(u8 num) {
    if (num) {
        TexCoordGen* const pTexCoordGens = GetTexCoordGenAry();

        for (u32 i = mGXMemNum.texCoordGen; i < num; i++) {
            pTexCoordGens[i] = TexCoordGen();
        }

        mGXMemNum.texCoordGen = num;
    }
}

void Material::SetTevStageNum(u8 num) {
    if (num) {
        TevStage* pTevStages = GetTevStageAry();

        for (u32 i = mGXMemNum.tevStage; i < num; i++) {
            pTevStages[i] = TevStage();
        }

        mGXMemNum.tevStage = num;
    }
}

void Material::SetIndStageNum(u8 num) {
    if (num) {
        IndirectStage* pIndStages = GetIndirectStageAry();

        for (u32 i = mGXMemNum.indStage; i < num; i++) {
            pIndStages[i] = IndirectStage();
        }

        mGXMemNum.indStage = num;
    }
}

void Material::GetTexture(GXTexObj* pTexObj, u8 idx) const {
    CopyGXTexObj(pTexObj, &GetTexMapAry()[idx]);
}

void Material::SetTexture(u8 idx, TPLPalette* pTplRes) {
    GXTexObj* pTexObj = &GetTexMapAry()[idx];
    detail::InitGXTexObjFromTPL(pTexObj, pTplRes, 0);
}

void Material::SetTextureNoWrap(u8 idx, TPLPalette* pTplRes) {
    GXTexObj* pTexObj = &GetTexMapAry()[idx];
    GXTexWrapMode wrapS = GXGetTexObjWrapS(pTexObj);
    GXTexWrapMode wrapT = GXGetTexObjWrapT(pTexObj);
    detail::InitGXTexObjFromTPL(pTexObj, pTplRes, 0);
    GXInitTexObjWrapMode(pTexObj, wrapS, wrapT);
}

void Material::SetTexture(u8 idx, const GXTexObj& rTexObj) {
    GXTexObj* pTexObj = &GetTexMapAry()[idx];
    CopyGXTexObj(pTexObj, &rTexObj);
}

void Material::SetColorElement(u32 idx, s16 value) {
    switch (idx) {
    case ANIMTARGET_MATCOLOR_MATR:
    case ANIMTARGET_MATCOLOR_MATG:
    case ANIMTARGET_MATCOLOR_MATB:
    case ANIMTARGET_MATCOLOR_MATA:
        if (mGXMemNum.matCol >= 1) {
            SetColorComponentValue(GetMatColAry(), idx & 3, value);
        }
        break;
    case ANIMTARGET_MATCOLOR_TEV0R:
    case ANIMTARGET_MATCOLOR_TEV0G:
    case ANIMTARGET_MATCOLOR_TEV0B:
    case ANIMTARGET_MATCOLOR_TEV0A:
    case ANIMTARGET_MATCOLOR_TEV1R:
    case ANIMTARGET_MATCOLOR_TEV1G:
    case ANIMTARGET_MATCOLOR_TEV1B:
    case ANIMTARGET_MATCOLOR_TEV1A:
    case ANIMTARGET_MATCOLOR_TEV2R:
    case ANIMTARGET_MATCOLOR_TEV2G:
    case ANIMTARGET_MATCOLOR_TEV2B:
    case ANIMTARGET_MATCOLOR_TEV2A: {
        u32 regIdx = (idx - ANIMTARGET_MATCOLOR_TEV0R) / 4;

        switch ((idx - ANIMTARGET_MATCOLOR_TEV0R) & 3) {
        case 0:
            mTevCols[regIdx].r = value;
            break;
        case 1:
            mTevCols[regIdx].g = value;
            break;
        case 2:
            mTevCols[regIdx].b = value;
            break;
        case 3:
            mTevCols[regIdx].a = value;
            break;
        }
        break;
    }
    case ANIMTARGET_MATCOLOR_TEVK0R:
    case ANIMTARGET_MATCOLOR_TEVK0G:
    case ANIMTARGET_MATCOLOR_TEVK0B:
    case ANIMTARGET_MATCOLOR_TEVK0A:
    case ANIMTARGET_MATCOLOR_TEVK1R:
    case ANIMTARGET_MATCOLOR_TEVK1G:
    case ANIMTARGET_MATCOLOR_TEVK1B:
    case ANIMTARGET_MATCOLOR_TEVK1A:
    case ANIMTARGET_MATCOLOR_TEVK2R:
    case ANIMTARGET_MATCOLOR_TEVK2G:
    case ANIMTARGET_MATCOLOR_TEVK2B:
    case ANIMTARGET_MATCOLOR_TEVK2A:
    case ANIMTARGET_MATCOLOR_TEVK3R:
    case ANIMTARGET_MATCOLOR_TEVK3G:
    case ANIMTARGET_MATCOLOR_TEVK3B:
    case ANIMTARGET_MATCOLOR_TEVK3A: {
        u32 regIdx = (idx - ANIMTARGET_MATCOLOR_TEVK0R) / 4;
        SetColorComponentValue(&mTevKCols[regIdx], (idx - ANIMTARGET_MATCOLOR_TEVK0R) & 3, value);
        break;
    }
    }
}

bool Material::SetupGX(bool modulate, u8 alpha) {
    static GXTevKColorSel kColSels[GX_MAX_TEXMAP] = {
        GX_TEV_KCSEL_K3_A, GX_TEV_KCSEL_K3_B, GX_TEV_KCSEL_K3_G, GX_TEV_KCSEL_K3_R,
        GX_TEV_KCSEL_K2_A, GX_TEV_KCSEL_K2_B, GX_TEV_KCSEL_K2_G, GX_TEV_KCSEL_K2_R,
    };

    static GXTevKAlphaSel kAlpSels[GX_MAX_TEXMAP] = {
        GX_TEV_KASEL_K3_A, GX_TEV_KASEL_K3_B, GX_TEV_KASEL_K3_G, GX_TEV_KASEL_K3_R,
        GX_TEV_KASEL_K2_A, GX_TEV_KASEL_K2_B, GX_TEV_KASEL_K2_G, GX_TEV_KASEL_K2_R,
    };

    bool useVtxColor = true;
    bool useMatColor = false;

    GXSetNumChans(1);

    if (IsChanCtrlCap()) {
        ChanCtrl* pChanCtrl = GetChanCtrlAry();

        GXSetChanCtrl(GX_COLOR0, GX_FALSE, GX_SRC_REG, pChanCtrl->GetColorSrc(), GX_LIGHT_NULL, GX_DF_NONE,
                      GX_AF_NONE);
        GXSetChanCtrl(GX_ALPHA0, GX_FALSE, GX_SRC_REG, pChanCtrl->GetAlphaSrc(), GX_LIGHT_NULL, GX_DF_NONE,
                      GX_AF_NONE);

        useVtxColor = pChanCtrl->GetColorSrc() == GX_SRC_VTX || pChanCtrl->GetAlphaSrc() == GX_SRC_VTX;
        useMatColor = pChanCtrl->GetColorSrc() == GX_SRC_REG || pChanCtrl->GetAlphaSrc() == GX_SRC_REG;
    } else {
        GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    }

    bool useRasStage = false;

    if (useVtxColor) {
        useRasStage = useRasStage || modulate;
    }

    if (useMatColor) {
        ut::Color matColor(0xFFFFFFFF);

        if (IsMatColorCap()) {
            matColor = *GetMatColAry();
        }

        matColor = detail::MultipleAlpha(matColor, alpha);
        GXSetChanMatColor(GX_COLOR0A0, matColor);

        useRasStage = useRasStage || matColor != 0xFFFFFFFF;
    }

    bool setTexMtx = false;
    bool useTexMtx[10];

    for (int i = 0; i < 10; i++) {
        useTexMtx[i] = false;
    }

    GXSetNumTexGens(mGXMemNum.texCoordGen);

    if (mGXMemNum.texCoordGen) {
        TexCoordGen* pTexCoordGens = GetTexCoordGenAry();

        for (int i = 0; i < mGXMemNum.texCoordGen; i++) {
            u32 texMtx = pTexCoordGens[i].GetTexMtx();

            if (pTexCoordGens[i].GetTexGenType() == GX_TG_MTX2x4 && texMtx != GX_IDENTITY) {
                useTexMtx[GetTexMtxIdx(texMtx)] = true;
                setTexMtx = true;
            }

            GXSetTexCoordGen(static_cast<GXTexCoordID>(i), pTexCoordGens[i].GetTexGenType(),
                             pTexCoordGens[i].GetTexGenSrc(), texMtx);
        }
    }

    if (setTexMtx) {
        TexSRT* const pTexSRTs = GetTexSRTAry();

        for (u8 i = 0; i < mGXMemNum.texSRT; i++) {
            if (useTexMtx[i]) {
                math::MTX34 texMtx;
                CalcTextureMtx(&texMtx, pTexSRTs[i]);
                GXLoadTexMtxImm(texMtx, GetTexMtx(i), GX_MTX2x4);
            }
        }
    }

    if (mGXMemNum.texMap) {
        u32 tlutName = GX_TLUT0;
        GXTexObj* pTexObjs = GetTexMapAry();

        for (int i = 0; i < mGXMemNum.texMap; i++) {
            GXTexObj texObj = pTexObjs[i];
            int format = GXGetTexObjFmt(&texObj);

            if (format == GX_TF_C4 || format == GX_TF_C8) {
                TPLClutHeader* pClutHeader = static_cast<TPLClutHeader*>(GXGetTexObjUserData(&texObj));

                if (pClutHeader) {
                    GXTlutObj tlutObj;
                    GXInitTlutObj(&tlutObj, pClutHeader->data, pClutHeader->format, pClutHeader->numEntries);
                    GXLoadTlut(&tlutObj, tlutName);
                    GXInitTexObjTlut(&texObj, tlutName);

                    tlutName++;
                }
            }

            GXLoadTexObj(&texObj, static_cast<GXTexMapID>(i));
        }
    }

    GXSetTevColorS10(GX_TEVREG0, mTevCols[TEVCOLOR_REG0]);
    GXSetTevColorS10(GX_TEVREG1, mTevCols[TEVCOLOR_REG1]);
    GXSetTevColorS10(GX_TEVREG2, mTevCols[TEVCOLOR_REG2]);

    GXSetTevKColor(GX_KCOLOR0, mTevKCols[0]);
    GXSetTevKColor(GX_KCOLOR1, mTevKCols[1]);
    GXSetTevKColor(GX_KCOLOR2, mTevKCols[2]);
    GXSetTevKColor(GX_KCOLOR3, mTevKCols[3]);

    if (IsTevSwapCap()) {
        TevSwapMode* pTevSwaps = GetTevSwapAry();

        for (int i = 0; i < GX_MAX_TEVSWAP; i++) {
            GXSetTevSwapModeTable(static_cast<GXTevSwapSel>(i), pTevSwaps[i].GetR(), pTevSwaps[i].GetG(),
                                  pTevSwaps[i].GetB(), pTevSwaps[i].GetA());
        }
    } else {
        GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
        GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
        GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
        GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
    }

    bool setIndTexMtx = false;
    bool useIndTexMtx[3];

    for (int i = 0; i < 3; i++) {
        useIndTexMtx[i] = false;
    }

    if (mGXMemNum.tevStage) {
        GXSetNumTevStages(mGXMemNum.tevStage);

        TevStage* pTevStages = GetTevStageAry();

        for (int i = 0; i < mGXMemNum.tevStage; i++) {
            GXTevStageID tevStage = static_cast<GXTevStageID>(i);

            GXSetTevOrder(tevStage, pTevStages[i].GetTexCoordGen(), pTevStages[i].GetTexMap(),
                          pTevStages[i].GetColorChan());
            GXSetTevSwapMode(tevStage, pTevStages[i].GetRasSwapSel(), pTevStages[i].GetTexSwapSel());

            GXSetTevColorIn(tevStage, pTevStages[i].GetColorInA(), pTevStages[i].GetColorInB(),
                            pTevStages[i].GetColorInC(), pTevStages[i].GetColorInD());
            GXSetTevColorOp(tevStage, pTevStages[i].GetColorOp(), pTevStages[i].GetColorBias(),
                            pTevStages[i].GetColorScale(), pTevStages[i].IsColorClamp(),
                            pTevStages[i].GetColorOutReg());
            GXSetTevKColorSel(tevStage, pTevStages[i].GetKColorSel());

            GXSetTevAlphaIn(tevStage, pTevStages[i].GetAlphaInA(), pTevStages[i].GetAlphaInB(),
                            pTevStages[i].GetAlphaInC(), pTevStages[i].GetAlphaInD());
            GXSetTevAlphaOp(tevStage, pTevStages[i].GetAlphaOp(), pTevStages[i].GetAlphaBias(),
                            pTevStages[i].GetAlphaScale(), pTevStages[i].IsAlphaClamp(),
                            pTevStages[i].GetAlphaOutReg());
            GXSetTevKAlphaSel(tevStage, pTevStages[i].GetKAlphaSel());

            GXIndTexMtxID indMtxSel = pTevStages[i].GetIndMtxSel();
            GXSetTevIndirect(tevStage, pTevStages[i].GetIndStage(), pTevStages[i].GetIndFormat(),
                             pTevStages[i].GetIndBiasSel(), indMtxSel, pTevStages[i].GetIndWrapS(),
                             pTevStages[i].GetIndWrapT(), pTevStages[i].IsIndAddPrev(), pTevStages[i].IsIndUtcLod(),
                             pTevStages[i].GetIndAlphaSel());

            if (GX_ITM_0 <= indMtxSel && indMtxSel <= GX_ITM_2) {
                useIndTexMtx[indMtxSel - GX_ITM_0] = true;
                setIndTexMtx = true;
            }
        }

        useRasStage = true;
    } else {
        u8 tevStageID = GX_TEVSTAGE0;

        if (mGXMemNum.texMap == 0) {
            GXTevStageID tevStage = static_cast<GXTevStageID>(tevStageID);

            GXSetTevOrder(tevStage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
            GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_C1, GX_CC_RASC, GX_CC_ZERO);
            GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_A1, GX_CA_RASA, GX_CA_ZERO);

            tevStageID++;
            useRasStage = true;
        } else {
            if (mGXMemNum.texMap == 1) {
                GXTevStageID tevStage = static_cast<GXTevStageID>(tevStageID);

                GXSetTevOrder(tevStage, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
                GXSetTevColorIn(tevStage, GX_CC_C0, GX_CC_C1, GX_CC_TEXC, GX_CC_ZERO);
                GXSetTevAlphaIn(tevStage, GX_CA_A0, GX_CA_A1, GX_CA_TEXA, GX_CA_ZERO);

                tevStageID++;
            } else {
                if (mGXMemNum.texMap == 2) {
                    GXTevStageID tevStage;

                    tevStage = static_cast<GXTevStageID>(tevStageID);
                    GXSetTevOrder(tevStage, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
                    GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
                    GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);

                    tevStageID++;

                    tevStage = static_cast<GXTevStageID>(tevStageID);
                    GXSetTevOrder(tevStage, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
                    GXSetTevColorIn(tevStage, GX_CC_TEXC, GX_CC_CPREV, GX_CC_KONST, GX_CC_ZERO);
                    GXSetTevAlphaIn(tevStage, GX_CA_TEXA, GX_CA_APREV, GX_CA_KONST, GX_CA_ZERO);
                    GXSetTevKColorSel(tevStage, kColSels[0]);
                    GXSetTevKAlphaSel(tevStage, kAlpSels[0]);

                    tevStageID++;
                } else {
                    for (int i = 0; i < mGXMemNum.texMap; i++) {
                        GXTevStageID tevStage = static_cast<GXTevStageID>(tevStageID);

                        GXSetTevOrder(tevStage, static_cast<GXTexCoordID>(i), static_cast<GXTexMapID>(i),
                                      GX_COLOR_NULL);

                        GXTevColorArg colorD = i == 0 ? GX_CC_ZERO : GX_CC_CPREV;
                        GXTevAlphaArg alphaD = i == 0 ? GX_CA_ZERO : GX_CA_APREV;

                        GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, colorD);
                        GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, alphaD);
                        GXSetTevKColorSel(tevStage, kColSels[i]);
                        GXSetTevKAlphaSel(tevStage, kAlpSels[i]);

                        tevStageID++;
                    }
                }

                if (mTevCols[TEVCOLOR_REG0] != DefaultBlackColor || mTevCols[TEVCOLOR_REG1] != DefaultWhiteColor) {
                    GXTevStageID tevStage = static_cast<GXTevStageID>(tevStageID);

                    GXSetTevOrder(tevStage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
                    GXSetTevColorIn(tevStage, GX_CC_C0, GX_CC_C1, GX_CC_CPREV, GX_CC_ZERO);
                    GXSetTevAlphaIn(tevStage, GX_CA_A0, GX_CA_A1, GX_CA_APREV, GX_CA_ZERO);

                    tevStageID++;
                }
            }

            if (useRasStage) {
                GXTevStageID tevStage = static_cast<GXTevStageID>(tevStageID);

                GXSetTevOrder(tevStage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
                GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
                GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);

                tevStageID++;
            }
        }

        const u8 tevStageNum = tevStageID;

        for (u8 id = GX_TEVSTAGE0; id < tevStageNum; id++) {
            GXTevStageID tevStage = static_cast<GXTevStageID>(id);

            GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
            GXSetTevDirect(tevStage);
            GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
        }

        GXSetNumTevStages(tevStageNum);
    }

    if (setIndTexMtx) {
        TexSRT* pIndTexSRTs = GetIndTexSRTAry();

        for (int i = 0; i < mGXMemNum.indSRT; i++) {
            if (useIndTexMtx[i]) {
                Mtx23 mtx;
                CalcIndTexMtx(mtx, pIndTexSRTs[i]);
                SetIndTexMtx(static_cast<GXIndTexMtxID>(GX_ITM_0 + i), mtx);
            }
        }
    }

    GXSetNumIndStages(mGXMemNum.indStage);

    if (mGXMemNum.indStage) {
        IndirectStage* pIndStages = GetIndirectStageAry();

        for (int i = 0; i < mGXMemNum.indStage; i++) {
            GXIndTexStageID indStage = static_cast<GXIndTexStageID>(i);

            GXSetIndTexOrder(indStage, pIndStages[i].GetTexCoordGen(), pIndStages[i].GetTexMap());
            GXSetIndTexCoordScale(indStage, pIndStages[i].GetScaleS(), pIndStages[i].GetScaleT());
        }
    }

    if (IsAlphaCompareCap()) {
        AlphaCompare* pAlphaCompare = GetAlphaComparePtr();

        GXSetAlphaCompare(pAlphaCompare->GetComp0(), pAlphaCompare->GetRef0(), pAlphaCompare->GetOp(),
                          pAlphaCompare->GetComp1(), pAlphaCompare->GetRef1());
    } else {
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    }

    if (IsBlendModeCap()) {
        BlendMode* pBlendMode = GetBlendModePtr();

        GXSetBlendMode(pBlendMode->GetType(), pBlendMode->GetSrcFactor(), pBlendMode->GetDstFactor(),
                       pBlendMode->GetOp());
    } else {
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
    }

    return useRasStage && useVtxColor;
}

void Material::BindAnimation(AnimTransform* pAnimTrans) {
    pAnimTrans->Bind(this);
}

void Material::UnbindAnimation(AnimTransform* pAnimTrans) {
    for (AnimationLinkList::Iterator it = mAnimList.GetBeginIter(); it != mAnimList.GetEndIter();) {
        AnimationLinkList::Iterator currIt = it++;

        if (pAnimTrans == nullptr || currIt->GetAnimTransform() == pAnimTrans) {
            mAnimList.Erase(currIt);
            currIt->Reset();
        }
    }
}

void Material::UnbindAllAnimation() {
    UnbindAnimation(nullptr);
}

void Material::Animate() {
    for (AnimationLinkList::Iterator it = mAnimList.GetBeginIter(); it != mAnimList.GetEndIter(); ++it) {
        if (it->IsEnable()) {
            AnimTransform* pAnimTrans = it->GetAnimTransform();
            pAnimTrans->Animate(it->GetIndex(), this);
        }
    }
}

void Material::AddAnimationLink(AnimationLink* pAnimLink) {
    mAnimList.PushBack(pAnimLink);
}

AnimationLink* Material::FindAnimationLink(AnimTransform* pAnimTrans) {
    return detail::FindAnimationLink(&mAnimList, pAnimTrans);
}

void Material::SetAnimationEnable(AnimTransform* pAnimTrans, bool enable) {
    if (AnimationLink* pAnimLink = FindAnimationLink(pAnimTrans)) {
        pAnimLink->SetEnable(enable);
    }
}

Size detail::GetTextureSize(Material* pMaterial, u8 idx) {
    if (idx >= pMaterial->GetTextureNum()) {
        return Size(0.0f, 0.0f);
    }

    GXTexObj texObj;
    pMaterial->GetTexture(&texObj, idx);

    return Size(GXGetTexObjWidth(&texObj), GXGetTexObjHeight(&texObj));
}

}
}
