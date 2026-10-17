#include <nw4r/g3d/res/g3d_restev.h>
#include <nw4r/g3d/platform/g3d_cpu.h>
#include <nw4r/db/db_assert.h>

#include <revolution/base/PPCArch.h>

namespace nw4r {
namespace g3d {

void ResTev::GXSetTevSwapMode(GXTevStageID stage, GXTevSwapSel ras, GXTevSwapSel tex) {
    u8* pCmd = ref().dl.dl.var[stage / TEV_STAGES_PER_DL].dl.alphaCalcAndSwap[stage % TEV_STAGES_PER_DL];

    detail::ResWriteBPCmd(pCmd,
                          (ras << GX_BP_TEVALPHACOMBINER_RSWAP_SHIFT) | (tex << GX_BP_TEVALPHACOMBINER_TSWAP_SHIFT) |
                              ((GX_BP_REG_TEVALPHACOMBINER0 + stage * 2) << GX_BP_OPCODE_SHIFT),
                          GX_BP_TEVALPHACOMBINER_SWAP_MASK);
}

void ResTev::GXSetTevAlphaIn(GXTevStageID stage, GXTevAlphaArg a, GXTevAlphaArg b, GXTevAlphaArg c, GXTevAlphaArg d) {
    u8* pCmd = ref().dl.dl.var[stage / TEV_STAGES_PER_DL].dl.alphaCalcAndSwap[stage % TEV_STAGES_PER_DL];

    detail::ResWriteBPCmd(pCmd,
                          (d << GX_BP_TEVCOMBINER_D_SHIFT) | (c << GX_BP_TEVCOMBINER_C_SHIFT) |
                              (b << GX_BP_TEVCOMBINER_B_SHIFT) | (a << GX_BP_TEVCOMBINER_A_SHIFT) |
                              ((GX_BP_REG_TEVALPHACOMBINER0 + stage * 2) << GX_BP_OPCODE_SHIFT),
                          GX_BP_TEVALPHACOMBINER_IN_MASK);
}

void ResTev::GXSetTevAlphaOp(GXTevStageID stage, GXTevOp op, GXTevBias bias, GXTevScale scale, GXBool clamp,
                             GXTevRegID reg) {
    u8* pCmd = ref().dl.dl.var[stage / TEV_STAGES_PER_DL].dl.alphaCalcAndSwap[stage % TEV_STAGES_PER_DL];

    if (op <= GX_TEV_SUB) {
        detail::ResWriteBPCmd(pCmd,
                              (bias << GX_BP_TEVCOMBINER_BIAS_SHIFT) | ((op & 1) << GX_BP_TEVCOMBINER_OP_SHIFT) |
                                  (clamp << GX_BP_TEVCOMBINER_CLAMP_SHIFT) | (scale << GX_BP_TEVCOMBINER_SCALE_SHIFT) |
                                  (reg << GX_BP_TEVCOMBINER_DEST_SHIFT) |
                                  ((GX_BP_REG_TEVALPHACOMBINER0 + stage * 2) << GX_BP_OPCODE_SHIFT),
                              GX_BP_TEVCOMBINER_OP_MASK);
    } else {
        detail::ResWriteBPCmd(pCmd,
                              (3 << GX_BP_TEVCOMBINER_BIAS_SHIFT) | ((op & 1) << GX_BP_TEVCOMBINER_OP_SHIFT) |
                                  (clamp << GX_BP_TEVCOMBINER_CLAMP_SHIFT) | ((op & 6) << 19) |
                                  (reg << GX_BP_TEVCOMBINER_DEST_SHIFT) |
                                  ((GX_BP_REG_TEVALPHACOMBINER0 + stage * 2) << GX_BP_OPCODE_SHIFT),
                              GX_BP_TEVCOMBINER_OP_MASK);
    }
}

void ResTev::GXSetTevKColorSel(GXTevStageID stage, GXTevKColorSel sel) {
    u8* pCmd = ref().dl.dl.var[stage / TEV_STAGES_PER_DL].dl.tevKonstantSel;
    u32 shift = (stage & 1) ? GX_BP_TEVKSEL_KCSEL_ODD_SHIFT : GX_BP_TEVKSEL_KCSEL_EVEN_SHIFT;
    u32 mask = GX_BP_TEVKSEL_KCSEL_LMASK << shift;

    detail::ResWriteSSMask(pCmd, mask);
    detail::ResWriteBPCmd(&pCmd[GX_BP_CMD_SZ],
                          ((GX_BP_REG_TEVKSEL0 + stage / TEV_STAGES_PER_DL) << GX_BP_OPCODE_SHIFT) | (sel << shift),
                          0xFF000000 | mask);
}

bool ResTev::GXGetTevOrder(GXTevStageID stage, GXTexCoordID* pCoord, GXTexMapID* pMap, GXChannelID* pChannel) const {
    static const GXChannelID r2c[GX_RAS_MAX_CHANNEL] = {GX_COLOR0A0,   GX_COLOR1A1,   GX_COLOR_NULL,  GX_COLOR_NULL,
                                                        GX_COLOR_NULL, GX_ALPHA_BUMP, GX_ALPHA_BUMPN, GX_COLOR_ZERO};

    const u8* pCmd = ref().dl.dl.var[stage / TEV_STAGES_PER_DL].dl.tevOrder;

    if (pCmd[0] == 0) {
        return false;
    }

    u32 cmd;
    detail::ResReadBPCmd(pCmd, &cmd);

    bool enabled;
    GXTexCoordID coord;
    GXTexMapID map;
    GXChannelID channel;

    if (stage & 1) {
        channel = r2c[cmd >> (GX_BP_RAS1_TREF_ODD_SHIFT + GX_BP_RAS1_TREF_COLORCHAN_SHIFT) &
                      GX_BP_RAS1_TREF_COLORCHAN_LMASK];
        coord = static_cast<GXTexCoordID>(cmd >> (GX_BP_RAS1_TREF_ODD_SHIFT + GX_BP_RAS1_TREF_TEXCOORD_SHIFT) &
                                          GX_BP_RAS1_TREF_TEXCOORD_LMASK);
        enabled = cmd >> (GX_BP_RAS1_TREF_ODD_SHIFT + GX_BP_RAS1_TREF_ENABLE_TEX_SHIFT) &
                  GX_BP_RAS1_TREF_ENABLE_TEX_LMASK;
        map = static_cast<GXTexMapID>(cmd >> (GX_BP_RAS1_TREF_ODD_SHIFT + GX_BP_RAS1_TREF_TEXMAP_SHIFT) &
                                      GX_BP_RAS1_TREF_TEXMAP_LMASK);
    } else {
        channel = r2c[cmd >> GX_BP_RAS1_TREF_COLORCHAN_SHIFT & GX_BP_RAS1_TREF_COLORCHAN_LMASK];
        coord = static_cast<GXTexCoordID>(cmd >> GX_BP_RAS1_TREF_TEXCOORD_SHIFT & GX_BP_RAS1_TREF_TEXCOORD_LMASK);
        enabled = cmd >> GX_BP_RAS1_TREF_ENABLE_TEX_SHIFT & GX_BP_RAS1_TREF_ENABLE_TEX_LMASK;
        map = static_cast<GXTexMapID>(cmd >> GX_BP_RAS1_TREF_TEXMAP_SHIFT & GX_BP_RAS1_TREF_TEXMAP_LMASK);
    }

    if (pCoord != nullptr) {
        *pCoord = coord;
    }

    if (pChannel != nullptr) {
        *pChannel = channel;
    }

    if (!enabled) {
        map = GX_TEXMAP_NULL;
    }

    if (pMap) {
        *pMap = map;
    }

    return true;
}

void ResTev::GXSetTevOrder(GXTevStageID stage, GXTexCoordID coord, GXTexMapID map, GXChannelID channel) {
    static u8 c2r[0x10] = {0, 1, 0, 1, 0, 1, 7, 5, 6, 0, 0, 0, 0, 0, 0, 7};

    NW4R_ASSERT(stage < GX_MAX_TEVSTAGE);
    NW4R_ASSERT(coord < GX_MAX_TEXCOORD || coord == GX_TEXCOORD_NULL);

    GXTexCoordID currCoord;
    GXTexMapID currMap;
    if (GXGetTevOrder(stage, &currCoord, &currMap, nullptr)) {
        if (currCoord != GX_TEXCOORD_NULL && currMap != GX_TEXMAP_NULL) {
            ref().texCoordToTexMapID[currCoord] = GX_TEXMAP_NULL;
        }
    }

    if (coord != GX_TEXCOORD_NULL) {
        ref().texCoordToTexMapID[coord] = map;
    }

    u8* pCmd = ref().dl.dl.var[stage / TEV_STAGES_PER_DL].dl.tevOrder;

    u32 shift = (stage & 1) ? GX_BP_RAS1_TREF_ODD_SHIFT : 0;
    u32 mask = (0xFF << GX_BP_OPCODE_SHIFT) | (GX_BP_RAS1_TREF_STAGE_MASK << shift);

    u32 cmd = ((map & GX_BP_RAS1_TREF_TEXMAP_LMASK) << GX_BP_RAS1_TREF_TEXMAP_SHIFT) |
              ((coord & GX_BP_RAS1_TREF_TEXCOORD_LMASK) << GX_BP_RAS1_TREF_TEXCOORD_SHIFT) |
              ((map != GX_TEXMAP_NULL && !(map & GX_TEX_DISABLE)) << GX_BP_RAS1_TREF_ENABLE_TEX_SHIFT) |
              (c2r[channel & 0xF] << GX_BP_RAS1_TREF_COLORCHAN_SHIFT);

    cmd <<= shift;
    cmd |= (GX_BP_REG_RAS1_TREF0 + (stage / TEV_STAGES_PER_DL)) << GX_BP_OPCODE_SHIFT;

    detail::ResWriteBPCmd(pCmd, cmd, mask);
}

void ResTev::GXSetTevColorIn(GXTevStageID stage, GXTevColorArg a, GXTevColorArg b, GXTevColorArg c, GXTevColorArg d) {
    u8* pCmd = ref().dl.dl.var[stage / TEV_STAGES_PER_DL].dl.tevColorCalc[stage % TEV_STAGES_PER_DL];

    detail::ResWriteBPCmd(pCmd,
                          (d << GX_BP_TEVCOLORCOMBINER_D_SHIFT) | (c << GX_BP_TEVCOLORCOMBINER_C_SHIFT) |
                              (b << GX_BP_TEVCOLORCOMBINER_B_SHIFT) | (a << GX_BP_TEVCOLORCOMBINER_A_SHIFT) |
                              ((GX_BP_REG_TEVCOLORCOMBINER0 + stage * 2) << GX_BP_OPCODE_SHIFT),
                          GX_BP_TEVCOLORCOMBINER_IN_MASK);
}

void ResTev::GXSetTevColorOp(GXTevStageID stage, GXTevOp op, GXTevBias bias, GXTevScale scale, GXBool clamp,
                             GXTevRegID reg) {
    u8* pCmd = ref().dl.dl.var[stage / TEV_STAGES_PER_DL].dl.tevColorCalc[stage % TEV_STAGES_PER_DL];

    if (op <= GX_TEV_SUB) {
        detail::ResWriteBPCmd(pCmd,
                              (bias << GX_BP_TEVCOMBINER_BIAS_SHIFT) | ((op & 1) << GX_BP_TEVCOMBINER_OP_SHIFT) |
                                  (clamp << GX_BP_TEVCOMBINER_CLAMP_SHIFT) | (scale << GX_BP_TEVCOMBINER_SCALE_SHIFT) |
                                  (reg << GX_BP_TEVCOMBINER_DEST_SHIFT) |
                                  ((GX_BP_REG_TEVCOLORCOMBINER0 + stage * 2) << GX_BP_OPCODE_SHIFT),
                              GX_BP_TEVCOMBINER_OP_MASK);
    } else {
        detail::ResWriteBPCmd(pCmd,
                              (3 << GX_BP_TEVCOMBINER_BIAS_SHIFT) | ((op & 1) << GX_BP_TEVCOMBINER_OP_SHIFT) |
                                  (clamp << GX_BP_TEVCOMBINER_CLAMP_SHIFT) | ((op & 6) << 19) |
                                  (reg << GX_BP_TEVCOMBINER_DEST_SHIFT) |
                                  ((GX_BP_REG_TEVCOLORCOMBINER0 + stage * 2) << GX_BP_OPCODE_SHIFT),
                              GX_BP_TEVCOMBINER_OP_MASK);
    }
}

void ResTev::SetNumTevStages(u8 num) {
    if (num < 1 || num > GX_MAX_TEVSTAGE) {
        return;
    }

    ResTevData& r = ref();

    if (r.nStages > num) {
        u32 i;

        for (i = num; i < r.nStages; i++) {
            GXSetTevOrder(static_cast<GXTevStageID>(i), GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        }

        for (i = (num + 1) / TEV_STAGES_PER_DL; i < static_cast<u32>(r.nStages + 1) / TEV_STAGES_PER_DL; i++) {
            ResTevVariableDL* pVar = &r.dl.dl.var[i];

            detail::ZeroMemory16ByteBlocks(pVar, sizeof(ResTevVariableDL));
            DCStoreRangeNoSync(pVar, sizeof(ResTevVariableDL));
        }
    }

    r.nStages = num;
}

void ResTev::CallDisplayList(bool sync) const {
    static const u32 dlsize[GX_MAX_TEVSTAGE] = {
        0x0A0, 0x0A0, 0x0C0, 0x0C0, 0x100, 0x100, 0x120, 0x120,
        0x160, 0x160, 0x180, 0x180, 0x1C0, 0x1C0, 0x1E0, 0x1E0,
    };

    if (sync) {
        PPCSync();
    }

    GXCallDisplayList(const_cast<ResTevDL*>(&ref().dl), dlsize[GetNumTevStages() - 1]);
}

ResTev ResTev::CopyTo(void* pDst) {
    const ResTevData* pSrc = &ref();
    detail::Copy32ByteBlocks(pDst, pSrc, sizeof(ResTevData));

    ResTev tev(pDst);
    tev.ref().toResMdlData -= reinterpret_cast<u32>(pDst) - reinterpret_cast<u32>(pSrc);

    tev.DCStore(false);
    return tev;
}

void ResTev::DCStore(bool sync) {
    void* pBase = &ref();
    u32 size = ref().size;

    if (sync) {
        DC::StoreRange(pBase, size);
    } else {
        DC::StoreRangeNoSync(pBase, size);
    }
}

}
}
