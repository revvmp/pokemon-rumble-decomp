#ifndef NW4HBM_LYT_MATERIAL_H
#define NW4HBM_LYT_MATERIAL_H
#include "nw4hbm/types_nw4hbm.h"

#include "nw4hbm/lyt/lyt_common.h"
#include "nw4hbm/lyt/lyt_resources.h"
#include "nw4hbm/lyt/lyt_types.h"

#include "nw4hbm/ut.h"

#include "revolution/gx.h"

namespace nw4hbm {
namespace lyt {

class AnimTransform;
class AnimResource;
struct ResBlockSet;

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

namespace res {

struct Material {
    char name[NW4R_LYT_MATERIAL_NAME_LEN];
    GXColorS10 tevCols[TEVCOLOR_MAX];
    ut::Color tevKCols[GX_MAX_KCOLOR];
    MaterialResourceNum resNum;
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

    virtual AnimationLink* FindAnimationLink(AnimTransform* pAnimTrans);

    virtual void SetAnimationEnable(AnimTransform* pAnimTrans, bool enable);

    void AddAnimationLink(AnimationLink* pAnimLink);

    const char* GetName() const {
        return mName;
    }
    void SetName(const char* pName);

    GXColorS10 GetTevColor(u32 idx) const {
        return mTevCols[idx];
    }
    void SetTevColor(u32 idx, const GXColorS10& rColor) {
        mTevCols[idx] = rColor;
    }

    ut::Color GetTevKColor(u32 idx) const {
        return mTevKCols[idx];
    }
    void SetTevKColor(u32 idx, ut::Color color) {
        mTevKCols[idx] = color;
    }

    u8 GetTextureNum() const {
        return mGXMemNum.texMap;
    }
    u8 GetTextureCap() const {
        return mGXMemCap.texMap;
    }
    void SetTextureNum(u8 num);

    u8 GetTexSRTCap() const {
        return mGXMemCap.texSRT;
    }

    u8 GetTexCoordGenCap() const {
        return mGXMemCap.texCoordGen;
    }
    void SetTexCoordGenNum(u8 num);

    u8 GetIndTexSRTCap() const {
        return mGXMemCap.indSRT;
    }

    void SetTevStageNum(u8 num);
    void SetIndStageNum(u8 num);

    void SetColorElement(u32 idx, s16 value);

    bool IsTevSwapCap() const {
        return mGXMemCap.tevSwap;
    }
    bool IsChanCtrlCap() const {
        return mGXMemCap.chanCtrl;
    }
    bool IsMatColorCap() const {
        return mGXMemCap.matCol;
    }
    bool IsAlphaCompareCap() const {
        return mGXMemCap.alpComp;
    }
    bool IsBlendModeCap() const {
        return mGXMemCap.blendMode;
    }

    const GXTexObj* GetTexMapAry() const;
    const TexSRT* GetTexSRTAry() const;
    const TexCoordGen* GetTexCoordGenAry() const;
    const ChanCtrl* GetChanCtrlAry() const;
    const ut::Color* GetMatColAry() const;
    const TevSwapMode* GetTevSwapAry() const;
    const AlphaCompare* GetAlphaComparePtr() const;
    const BlendMode* GetBlendModePtr() const;
    const IndirectStage* GetIndirectStageAry() const;
    const TexSRT* GetIndTexSRTAry() const;
    const TevStage* GetTevStageAry() const;

    GXTexObj* GetTexMapAry();
    TexSRT* GetTexSRTAry();
    TexCoordGen* GetTexCoordGenAry();
    ChanCtrl* GetChanCtrlAry();
    ut::Color* GetMatColAry();
    TevSwapMode* GetTevSwapAry();
    AlphaCompare* GetAlphaComparePtr();
    BlendMode* GetBlendModePtr();
    IndirectStage* GetIndirectStageAry();
    TexSRT* GetIndTexSRTAry();
    TevStage* GetTevStageAry();

    void GetTexture(GXTexObj* pTexObj, u8 idx) const;
    void SetTexture(u8 idx, const GXTexObj& rTexObj);
    void SetTexture(u8 idx, TPLPalette* pTplRes);
    void SetTextureNoWrap(u8 idx, TPLPalette* pTplRes);

    void SetTexCoordGen(u32 idx, TexCoordGen gen) {
        GetTexCoordGenAry()[idx] = gen;
    }

    void SetTexSRTElement(u32 srt, u32 idx, f32 value) {
        f32* const pArray = reinterpret_cast<f32*>(&GetTexSRTAry()[srt]);
        pArray[idx] = value;
    }

    void SetIndTexSRTElement(u32 srt, u32 idx, f32 value) {
        f32* const pArray = reinterpret_cast<f32*>(&GetIndTexSRTAry()[srt]);
        pArray[idx] = value;
    }

    bool IsUserAllocated() const {
        return mbUserAllocated;
    }

    void ReserveGXMem(u8 texMapNum, u8 texSrtNum, u8 texCoordGenNum, u8 tevStageNum, bool allocTevSwap,
                      u8 indStageNum, u8 indSrtNum, bool allocChanCtrl, bool allocMatCol, bool allocAlpComp,
                      bool allocBlendMode);

protected:
    char mName[NW4R_LYT_MATERIAL_NAME_LEN];
    AnimationLinkList mAnimList;
    GXColorS10 mTevCols[TEVCOLOR_MAX];
    ut::Color mTevKCols[GX_MAX_KCOLOR];
    detail::BitGXNums mGXMemCap;
    detail::BitGXNums mGXMemNum;
    bool mbUserAllocated;
    void* mpGXMem;

private:
    void Init();
    void InitBitGXNums(detail::BitGXNums* pNums);
};

namespace detail {

Size GetTextureSize(Material* pMaterial, u8 idx);

}
}
}

#endif
