#ifndef NW4R_LYT_MATERIAL_H
#define NW4R_LYT_MATERIAL_H
#include <nw4r/lyt/lyt_materialTypes.h>
#include "nw4r/lyt/lyt_animation.h"
#include "nw4r/lyt/lyt_resources.h"
#include "nw4r/lyt/lyt_texMap.h"
#include "nw4r/ut/ut_Color.h"
#include "nw4r/ut/ut_LinkList.h"
#include <revolution/GX.h>

namespace nw4r {
namespace lyt {

class Material {
public:
    Material();
    Material(const res::Material *pRes, const ResBlockSet &resBlockSet);

    void Init();
    static void InitBitGXNums(detail::BitGXNums *ptr);
    void ReserveGXMem(
        u8 texMapNum, u8 texSRTNum, u8 texCoordGenNum, u8 tevStageNum, bool allocTevSwap, u8 indStageNum, u8 indSRTNum,
        bool allocChanCtrl, bool allocMatCol, bool allocAlpComp, bool allocBlendMode
    );

    const TexMap *GetTexMapAry() const;
    TexMap *GetTexMapAry();
    const TexSRT *GetTexSRTAry() const;
    TexSRT *GetTexSRTAry();
    const TexCoordGen *GetTexCoordGenAry() const;
    TexCoordGen *GetTexCoordGenAry();
    const ChanCtrl *GetChanCtrlAry() const;
    ChanCtrl *GetChanCtrlAry();
    const ut::Color *GetMatColAry() const;
    ut::Color *GetMatColAry();
    const TevSwapMode *GetTevSwapAry() const;
    TevSwapMode *GetTevSwapAry();
    const AlphaCompare *GetAlphaComparePtr() const;
    AlphaCompare *GetAlphaComparePtr();
    const BlendMode *GetBlendModePtr() const;
    BlendMode *GetBlendModePtr();
    const IndirectStage *GetIndirectStageAry() const;
    IndirectStage *GetIndirectStageAry();
    const TexSRT *GetIndTexSRTAry() const;
    TexSRT *GetIndTexSRTAry();
    const TevStage *GetTevStageAry() const;
    TevStage *GetTevStageAry();

    void SetName(const char *name);
    void SetTextureNum(u8 val);
    void SetTexCoordGenNum(u8 val);
    void SetTevStageNum(u8 val);
    void SetIndStageNum(u8 val);
    void SetColorElement(u32 colorType, s16 value);
    void AddAnimationLink(AnimationLink *pAnimationLink);

    bool IsBlendModeCap() const {
        return mGXMemCap.blendMode;
    }

    bool IsAlphaCompareCap() const {
        return mGXMemCap.alpComp;
    }

    bool IsTevSwapCap() const {
        return mGXMemCap.tevSwap;
    }

    bool IsMatColorCap() const {
        return mGXMemCap.matCol;
    }

    bool IsChanCtrlCap() const {
        return mGXMemCap.chanCtrl;
    }

    void SetTexSRTElement(u32 texSRTIdx, u32 eleIdx, f32 value) {
        f32 *srtAry = (f32 *)&(GetTexSRTAry()[texSRTIdx]);
        srtAry[eleIdx] = value;
    }

    TexMap *GetTexturePtr(u8 idx) {
        return &GetTexMapAry()[idx];
    }

    u8 GetTexSRTCap() const {
        return mGXMemCap.texSRT;
    }

    u8 GetIndTexSRTCap() const {
        return mGXMemCap.indSRT;
    }

    void SetIndTexSRTElement(u32 texSRTIdx, u32 eleIdx, f32 value) {
        f32 *srtAry = (f32 *)&(GetIndTexSRTAry()[texSRTIdx]);
        srtAry[eleIdx] = value;
    }

    ut::LinkList<AnimationLink, 0> *GetAnimationList() {
        return &mAnimList;
    }
    bool IsUserAllocated() const {
        return mbUserAllocated;
    }
    const char *GetName() const {
        return mName;
    }

    u8 GetTexCoordGenCap() const {
        return mGXMemCap.texCoordGen;
    }
    u8 GetTextureCap() const {
        return mGXMemCap.texMap;
    }

    u8 GetTextureNum() const {
        return mGXMemNum.texMap;
    }

    GXColorS10 GetTevColor(u32 index) const {
        return mTevCols[index];
    }

    void SetTexture(u32 texMapIdx, const TexMap &texMap) {
        GetTexMapAry()[texMapIdx] = texMap;
    }

    void SetTexCoordGen(u32 idx, TexCoordGen value) {
        GetTexCoordGenAry()[idx] = value;
    }

    void SetTevColor(u32 idx, const GXColorS10 &color) {
        mTevCols[idx] = color;
    }

    void SetAlphaCompare(const AlphaCompare &alphaComp) {
        *GetAlphaComparePtr() = alphaComp;
    }

    void SetChanControl(u32 idx, const ChanCtrl &chanCtrl) {
        GetChanCtrlAry()[idx] = chanCtrl;
    }

    void SetTevKColor(u32 idx, const ut::Color &color) {
        mTevKCols[idx] = color;
    }

    void SetTevSwapMode(u32 idx, const TevSwapMode &tevSwapMode) {
        GetTevSwapAry()[idx] = tevSwapMode;
    }

    void SetTexSRT(u32 idx, const TexSRT &texSRT) {
        GetTexSRTAry()[idx] = texSRT;
    }

    void SetIndirectStage(u32 idx, const IndirectStage &indStage) {
        GetIndirectStageAry()[idx] = indStage;
    }

    void SetBlendMode(const BlendMode &blendMode) {
        *GetBlendModePtr() = blendMode;
    }

    void SetTevStage(u32 idx, const TevStage &tevStage) {
        GetTevStageAry()[idx] = tevStage;
    }

    virtual ~Material();
    virtual bool SetupGX(bool bModVtxCol, u8 alpha);
    virtual void BindAnimation(AnimTransform *pAnimTrans);
    virtual void UnbindAnimation(AnimTransform *pAnimTrans);
    virtual void UnbindAllAnimation();
    virtual void Animate();
    virtual AnimationLink *FindAnimationLink(AnimTransform *pAnimTrans);
    virtual AnimationLink *FindAnimationLink(const AnimResource &animRes);
    virtual void SetAnimationEnable(AnimTransform *pAnimTrans, bool bEnable);
    virtual void SetAnimationEnable(const AnimResource &animRes, bool bEnable);

protected:
    AnimationLinkList mAnimList;
    GXColorS10 mTevCols[3];
    ut::Color mTevKCols[4];
    detail::BitGXNums mGXMemCap;
    detail::BitGXNums mGXMemNum;
    void *mpGXMem;
    char mName[21];
    bool mbUserAllocated;
    u8 mPadding[2];
};

namespace detail {

Size GetTextureSize(Material *pMaterial, u8 texMapIdx);

}

}
}

#endif
