#ifndef REVOLUTION_GX_GXSTATE_H
#define REVOLUTION_GX_GXSTATE_H

#include "revolution/types.h"
#include "revolution/gx/GXEnum.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GX_DIRTY_SU_TEX 0x00000001
#define GX_DIRTY_BP_MASK 0x00000002
#define GX_DIRTY_GEN_MODE 0x00000004
#define GX_DIRTY_VCD 0x00000008
#define GX_DIRTY_VAT 0x00000010
#define GX_DIRTY_AMB_COLOR0 0x00000100
#define GX_DIRTY_AMB_COLOR1 0x00000200
#define GX_DIRTY_MAT_COLOR0 0x00000400
#define GX_DIRTY_MAT_COLOR1 0x00000800
#define GX_DIRTY_AMB_MAT 0x00000F00
#define GX_DIRTY_CHAN_CTRL 0x0000F000
#define GX_DIRTY_TEX_GEN 0x00FF0000
#define GX_DIRTY_NUM_COLORS 0x01000000
#define GX_DIRTY_NUM_TEX_GENS 0x02000000
#define GX_DIRTY_MTX_IDX 0x04000000
#define GX_DIRTY_PROJECTION 0x08000000
#define GX_DIRTY_VIEWPORT 0x10000000

#define GX_DIRTY_LIGHT_CHAN (GX_DIRTY_NUM_COLORS | GX_DIRTY_CHAN_CTRL)
#define GX_DIRTY_TEX_GENS (GX_DIRTY_NUM_TEX_GENS | GX_DIRTY_TEX_GEN)

#define XF_REG_NUM_COLORS 0x1009
#define XF_REG_AMBIENT0 0x100A
#define XF_REG_AMBIENT1 0x100B
#define XF_REG_MATERIAL0 0x100C
#define XF_REG_MATERIAL1 0x100D
#define XF_REG_COLOR0_CTRL 0x100E
#define XF_REG_NUM_TEX_GENS 0x103F
#define XF_REG_TEX0 0x1040
#define XF_REG_DUAL_TEX0 0x1050

#define PE_REG_ZMODE 0
#define PE_REG_CMODE0 1
#define PE_REG_CMODE1 2
#define PE_REG_ALPHA_MODE 3
#define PE_REG_PI_CTL 4
#define PE_REG_INTRCTL 5
#define PE_REG_TOKEN 7

void __GXSetDirtyState(void);
void __GXSendFlushPrim(void);
void __GXSetGenMode(void);
void __GXSetSUTexRegs(void);
void __GXUpdateBPMask(void);
void __GXSetVCD(void);
void __GXSetVAT(void);
void __GXCalculateVLim(void);
void __GXSetMatrixIndex(GXAttr matIdxAttr);
void __GXSetViewport(void);
void __GXSetProjection(void);
void __GXCleanGPFifo(void);
void __GXInitRevisionBits(void);
void __GXPEInit(void);

#ifdef __cplusplus
}
#endif

#endif
