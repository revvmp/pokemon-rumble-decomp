#include "revolution/gx.h"
#include "revolution/gx/GXRegs.h"
#include "revolution/gx/GXTypes.h"

inline void __GXSetAmbMat(u32 amState) {
    if (amState & GX_DIRTY_AMB_COLOR0) {
        GX_WRITE_XF_REG(XF_REG_AMBIENT0, gx->ambColor[0], 0);
    }

    if (amState & GX_DIRTY_AMB_COLOR1) {
        GX_WRITE_XF_REG(XF_REG_AMBIENT1, gx->ambColor[1], 0);
    }

    if (amState & GX_DIRTY_MAT_COLOR0) {
        GX_WRITE_XF_REG(XF_REG_MATERIAL0, gx->matColor[0], 0);
    }

    if (amState & GX_DIRTY_MAT_COLOR1) {
        GX_WRITE_XF_REG(XF_REG_MATERIAL1, gx->matColor[1], 0);
    }
}

inline void __GXSetLightChan(u32 chState) {
    u32 bits;
    u32 i;
    u32 chanReg;

    chanReg = XF_REG_COLOR0_CTRL;

    if (chState & GX_DIRTY_NUM_COLORS) {
        bits = (gx->genMode & 0x70) >> GEN_MODE_NCOL_SHIFT;
        GX_WRITE_XF_REG(XF_REG_NUM_COLORS, bits, 0);
    }

    bits = (chState & GX_DIRTY_CHAN_CTRL) >> 12;
    i = 0;

    while (bits) {
        if (bits & 1) {
            GX_WRITE_XF_REG(chanReg, gx->chanCtrl[i], 0);
        }

        i++;
        chanReg++;
        bits >>= 1;
    }
}

inline void __GXSetTexGen(u32 tgState) {
    u32 bits;
    u32 i;
    u32 texGenReg;
    u32 dualTexGenReg;

    texGenReg = XF_REG_TEX0;
    dualTexGenReg = XF_REG_DUAL_TEX0;

    if (tgState & GX_DIRTY_NUM_TEX_GENS) {
        bits = gx->genMode & 0xF;
        GX_WRITE_XF_REG(XF_REG_NUM_TEX_GENS, bits, 0);
    }

    bits = (tgState & GX_DIRTY_TEX_GEN) >> 16;
    i = 0;

    while (bits) {
        if (bits & 1) {
            GX_WRITE_XF_REG(texGenReg, gx->texGenCtrl[i], 0);
            GX_WRITE_XF_REG(dualTexGenReg, gx->dualTexGenCtrl[i], 0);
        }

        dualTexGenReg++;
        texGenReg++;
        i++;
        bits >>= 1;
    }
}

void __GXSetDirtyState(void) {
    u32 dirtyState;

    dirtyState = gx->dirtyState;

    if (dirtyState & GX_DIRTY_SU_TEX) {
        __GXSetSUTexRegs();
    }

    if (dirtyState & GX_DIRTY_BP_MASK) {
        __GXUpdateBPMask();
    }

    if (dirtyState & GX_DIRTY_GEN_MODE) {
        __GXSetGenMode();
    }

    if (dirtyState & GX_DIRTY_VCD) {
        __GXSetVCD();
    }

    if (dirtyState & GX_DIRTY_VAT) {
        __GXSetVAT();
    }

    if (dirtyState & (GX_DIRTY_VCD | GX_DIRTY_VAT)) {
        __GXCalculateVLim();
    }

    dirtyState &= ~0xFF;

    if (dirtyState) {
        u32 subState;

        subState = dirtyState & GX_DIRTY_AMB_MAT;
        if (subState) {
            __GXSetAmbMat(subState);
        }

        subState = dirtyState & GX_DIRTY_LIGHT_CHAN;
        if (subState) {
            __GXSetLightChan(subState);
        }

        subState = dirtyState & GX_DIRTY_TEX_GENS;
        if (subState) {
            __GXSetTexGen(subState);
        }

        subState = dirtyState & GX_DIRTY_MTX_IDX;
        if (subState) {
            __GXSetMatrixIndex(GX_VA_PNMTXIDX);
            __GXSetMatrixIndex(GX_VA_TEX4MTXIDX);
        }

        subState = dirtyState & GX_DIRTY_VIEWPORT;
        if (subState) {
            __GXSetViewport();
        }

        subState = dirtyState & GX_DIRTY_PROJECTION;
        if (subState) {
            __GXSetProjection();
        }

        XF_SENT();
    }

    gx->dirtyState = 0;
}

void GXBegin(GXPrimitive type, GXVtxFmt vtxFmt, u16 numVerts) {
    if (gx->dirtyState) {
        __GXSetDirtyState();
    }

    if (*(u32*)&gx->vNumNot == 0) {
        __GXSendFlushPrim();
    }

    GX_WRITE_U8(vtxFmt | type);
    GX_WRITE_U16(numVerts);
}

void __GXSendFlushPrim(void) {
    u32 i;
    u32 numWords;

    numWords = (u32)(gx->vNum * gx->vLim);
    GX_WRITE_U8(GX_VTXFMT0 | GX_TRIANGLESTRIP);
    GX_WRITE_U16(gx->vNum);

    for (i = 0; i < numWords; i += 4) {
        GX_WRITE_U32(0);
    }

    XF_SENT();
}

void GXSetLineWidth(u8 width, GXTexOffset texOffsets) {
    SC_SU_LPSIZE_SET_LSIZE(gx->lpSize, width);
    SC_SU_LPSIZE_SET_LTOFF(gx->lpSize, texOffsets);
    GX_WRITE_RA_REG(gx->lpSize);
    BP_SENT();
}

void GXSetPointSize(u8 pointSize, GXTexOffset texOffsets) {
    SC_SU_LPSIZE_SET_PSIZE(gx->lpSize, pointSize);
    SC_SU_LPSIZE_SET_PTOFF(gx->lpSize, texOffsets);
    GX_WRITE_RA_REG(gx->lpSize);
    BP_SENT();
}

void GXEnableTexOffsets(GXTexCoordID coord, GXBool lineEnable, GXBool pointEnable) {
    SC_SU_TS0_SET_LF(gx->suTs0[coord], lineEnable);
    SC_SU_TS0_SET_PF(gx->suTs0[coord], pointEnable);
    GX_WRITE_RA_REG(gx->suTs0[coord]);
    BP_SENT();
}

void GXSetCullMode(GXCullMode mode) {
    GXCullMode hwMode;

    hwMode = (GXCullMode)(((mode & 1) << 1) | ((mode & 2) >> 1));
    SC_GEN_MODE_SET_REJECT_EN(gx->genMode, hwMode);
    gx->dirtyState |= GX_DIRTY_GEN_MODE;
}

void GXSetCoPlanar(GXBool enable) {
    u32 reg;

    SC_GEN_MODE_SET_ZFREEZE(gx->genMode, enable);
    reg = SU_SSMASK(0x80000, 0xFE);
    GX_WRITE_RA_REG(reg);
    GX_WRITE_RA_REG(gx->genMode);
    BP_SENT();
}

void __GXSetGenMode(void) {
    GX_WRITE_RA_REG(gx->genMode);
    BP_SENT();
}
