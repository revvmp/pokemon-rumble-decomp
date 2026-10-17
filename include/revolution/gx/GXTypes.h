#ifndef REVOLUTION_GX_GXTYPES_H
#define REVOLUTION_GX_GXTYPES_H

#include "revolution/types.h"
#include "revolution/gx/GXEnum.h"
#include "revolution/gx/GXStruct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GXData {
    u16 vNumNot;
    u16 bpSentNot;
    u16 vNum;
    u16 vLim;
    u32 cpEnable;
    u32 cpStatus;
    u32 cpClr;
    u32 vcdLo;
    u32 vcdHi;
    u32 vatA[8];
    u32 vatB[8];
    u32 vatC[8];
    u32 lpSize;
    u32 matIdxA;
    u32 matIdxB;
    u32 indexBase[4];
    u32 indexStride[4];
    u32 ambColor[2];
    u32 matColor[2];
    u32 chanCtrl[4];
    u32 texGenCtrl[8];
    u32 dualTexGenCtrl[8];
    u32 suTs0[8];
    u32 suTs1[8];
    u32 suScis0;
    u32 suScis1;
    u32 tref[8];
    u32 iref;
    u32 bpMask;
    u32 IndTexScale0;
    u32 IndTexScale1;
    u32 tevc[16];
    u32 teva[16];
    u32 tevKsel[8];
    u32 cmode0;
    u32 cmode1;
    u32 zmode;
    u32 peCtrl;
    u32 cpDispSrc;
    u32 cpDispSize;
    u32 cpDispStride;
    u32 cpDisp;
    u32 cpTexSrc;
    u32 cpTexSize;
    u32 cpTexStride;
    u32 cpTex;
    GXBool cpTexZ;
    u32 genMode;
    GXTexRegion TexRegions0[8];
    GXTexRegion TexRegions1[8];
    GXTexRegion TexRegions2[8];
    GXTlutRegion TlutRegions[20];
    GXTexRegion* (*texRegionCallback)(const GXTexObj* texObj, GXTexMapID id);
    GXTlutRegion* (*tlutRegionCallback)(u32 tlutName);
    GXAttrType nrmType;
    GXBool hasNrms;
    GXBool hasBiNrms;
    u32 projType;
    f32 projMtx[6];
    f32 vpLeft;
    f32 vpTop;
    f32 vpWd;
    f32 vpHt;
    f32 vpNearz;
    f32 vpFarz;
    f32 zOffset;
    f32 zScale;
    u32 tImage0[8];
    u32 tMode0[8];
    u32 texmapId[16];
    u32 tcsManEnab;
    u32 tevTcEnab;
    GXPerf0 perf0;
    GXPerf1 perf1;
    u32 perfSel;
    GXBool inDispList;
    GXBool dlSaveContext;
    GXBool abtWaitPECopy;
    u8 dirtyVAT;
    u32 dirtyState;
} GXData;

extern GXData* const __GXData;

#define gx __GXData

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
void __GXAbort(void);
GXBool __GXIsGPFifoReady(void);
void __GXCleanGPFifo(void);
void __GXInitRevisionBits(void);
void __GXPEInit(void);

#ifdef __cplusplus
}
#endif

#endif
