#ifndef NW4R_G3D_RES_RES_TEV_H
#define NW4R_G3D_RES_RES_TEV_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_rescommon.h>

#include <revolution/GX.h>

namespace nw4r {
namespace g3d {

static const int TEV_STAGES_PER_DL = 2;

struct ResTevCommonDL {
    union {
        struct {
            u8 swapModeTable[GX_MAX_TEVSWAP][GX_BP_CMD_SZ * 4];
            u8 indTexOrder[1][GX_BP_CMD_SZ];
            u8 PADDING_0x55[0x60 - 0x55];
        } dl;

        u8 data[0x60];
    };
};

struct ResTevVariableDL {
    union {
        struct {
            u8 tevKonstantSel[GX_BP_CMD_SZ * 2];
            u8 tevOrder[GX_BP_CMD_SZ];
            u8 tevColorCalc[TEV_STAGES_PER_DL][GX_BP_CMD_SZ];
            u8 alphaCalcAndSwap[TEV_STAGES_PER_DL][GX_BP_CMD_SZ];
            u8 tevIndirect[TEV_STAGES_PER_DL][GX_BP_CMD_SZ];
            u8 PADDING_0x2D[0x30 - 0x2D];
        } dl;

        u8 data[0x30];
    };
};

struct ResTevDL {
    union {
        struct {
            ResTevCommonDL common;
            ResTevVariableDL var[GX_MAX_TEVSTAGE / TEV_STAGES_PER_DL];
        } dl;

        u8 data[0x1E0];
    };
};

struct ResTevData {
    u32 size;
    s32 toResMdlData;
    u32 id;
    u8 nStages;
    u8 dummy_[0x10 - 0xD];
    u8 texCoordToTexMapID[GX_MAX_TEXCOORD];
    u8 dummy[0x20 - 0x18];
    ResTevDL dl;
};

class ResTev : public ResCommon<ResTevData> {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResTev);

    ResMdl GetParent();
    const ResMdl GetParent() const;

    bool GXGetTevSwapModeTable(
        GXTevSwapSel swap, GXTevColorChan *pR, GXTevColorChan *pG, GXTevColorChan *pB, GXTevColorChan *pA
    ) const;
    void
    GXSetTevSwapModeTable(GXTevSwapSel swap, GXTevColorChan r, GXTevColorChan g, GXTevColorChan b, GXTevColorChan a);

    bool GXGetTevOrder(GXTevStageID stage, GXTexCoordID *pCoord, GXTexMapID *pMap, GXChannelID *pChannel) const;
    void GXSetTevOrder(GXTevStageID stage, GXTexCoordID coord, GXTexMapID map, GXChannelID channel);

    bool GXGetIndTexOrder(GXIndTexStageID stage, GXTexCoordID *pCoord, GXTexMapID *pMap) const;
    void GXSetIndTexOrder(GXIndTexStageID stage, GXTexCoordID coord, GXTexMapID map);

    void GXSetTevSwapMode(GXTevStageID stage, GXTevSwapSel ras, GXTevSwapSel tex);

    void GXSetTevAlphaIn(GXTevStageID stage, GXTevAlphaArg a, GXTevAlphaArg b, GXTevAlphaArg c, GXTevAlphaArg d);
    void GXSetTevAlphaOp(GXTevStageID stage, GXTevOp op, GXTevBias bias, GXTevScale scale, GXBool clamp, GXTevRegID reg);

    void GXSetTevKColorSel(GXTevStageID stage, GXTevKColorSel sel);

    void GXSetTevColorIn(GXTevStageID stage, GXTevColorArg a, GXTevColorArg b, GXTevColorArg c, GXTevColorArg d);
    void GXSetTevColorOp(GXTevStageID stage, GXTevOp op, GXTevBias bias, GXTevScale scale, GXBool clamp, GXTevRegID reg);

    void SetNumTevStages(u8 num);

    void CallDisplayList(bool sync) const;

    ResTev CopyTo(void *pDst);

    void DCStore(bool sync);

    u8 GetNumTevStages() const {
        return ref().nStages;
    }

    void EndEdit() {
        DCStore(false);
    }
};

}
}

#endif
