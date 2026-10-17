#ifndef NW4R_G3D_PLATFORM_GX_HARDWARE_H
#define NW4R_G3D_PLATFORM_GX_HARDWARE_H
#include <nw4r/types_nw4r.h>

#include <revolution/GX.h>

#include <nw4r/g3d/platform/g3d_gxhardware_bp.h>
#include <nw4r/g3d/platform/g3d_gxhardware_cp.h>
#include <nw4r/g3d/platform/g3d_gxhardware_xf.h>

#define GX_BITSET(field, pos, size, value) __rlwimi((field), (value), 31 - (pos) - (size) + 1, (pos), (pos) + (size) - 1)
#define GX_BITGET(field, pos, size) ((field) >> (31 - (pos) - (size) + 1) & ((1 << (size)) - 1))

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GX_FIFO_CMD_NOOP = 0x00,

    GX_FIFO_CMD_LOAD_BP_REG = 0x61,
    GX_FIFO_CMD_LOAD_CP_REG = 0x08,
    GX_FIFO_CMD_LOAD_XF_REG = 0x10,

    GX_FIFO_CMD_LOAD_INDX_A = 0x20,
    GX_FIFO_CMD_LOAD_INDX_B = 0x28,
    GX_FIFO_CMD_LOAD_INDX_C = 0x30,
    GX_FIFO_CMD_LOAD_INDX_D = 0x38,

    GX_FIFO_CMD_CALL_DL = 0x40,
    GX_FIFO_CMD_INVAL_VTX = 0x48
} GXFifoCmd;

#define GX_BP_SET_OPCODE(cmd, opcode) (cmd) = GX_BITSET(cmd, 0, 8, (opcode))

#define GX_BP_OPCODE_SHIFT 24
#define GX_BP_CMD_SZ (sizeof(u8) + sizeof(u32))

#define GX_CP_CMD_SZ (sizeof(u8) + sizeof(u8) + sizeof(u32))

typedef enum {
    GX_XF_MEM_POSMTX = 0x0000,
    GX_XF_MEM_NRMMTX = 0x0400,
    GX_XF_MEM_DUALTEXMTX = 0x0500,
    GX_XF_MEM_LIGHTOBJ = 0x0600
} GXXfMem;

#define GX_XF_CMD_SZ (sizeof(u8) + sizeof(u32) + sizeof(u32))

typedef enum {
    GX_XF_TEX_PROJ_ST,
    GX_XF_TEX_PROJ_STQ
} GXXfTexProj;

typedef enum {
    GX_XF_TEX_FORM_AB11,
    GX_XF_TEX_FORM_ABC1
} GXXfTexForm;

typedef enum {
    GX_XF_TG_REGULAR,
    GX_XF_TG_BUMP,

    GX_XF_TG_CLR0,

    GX_XF_TG_CLR1
} GXXfTexGen;

typedef enum {
    GX_RAS_COLOR0A0,
    GX_RAS_COLOR1A1,
    GX_RAS_ALPHA_BUMP = 5,
    GX_RAS_ALPHA_BUMPN,
    GX_RAS_COLOR_ZERO,

    GX_RAS_MAX_CHANNEL
} GXRasChannelID;

typedef enum {
    GX_TEVREG_COLOR,
    GX_TEVREG_KONST,
} GXTevRegType;

#ifdef __cplusplus
}
#endif

#define GX_BP_RAS1_TREF_TEXMAP_SHIFT 0
#define GX_BP_RAS1_TREF_TEXCOORD_SHIFT 3
#define GX_BP_RAS1_TREF_ENABLE_TEX_SHIFT 6
#define GX_BP_RAS1_TREF_COLORCHAN_SHIFT 7
#define GX_BP_RAS1_TREF_ODD_SHIFT 12
#define GX_BP_RAS1_TREF_TEXMAP_LMASK 0x7
#define GX_BP_RAS1_TREF_TEXCOORD_LMASK 0x7
#define GX_BP_RAS1_TREF_ENABLE_TEX_LMASK 0x1
#define GX_BP_RAS1_TREF_COLORCHAN_LMASK 0x7
#define GX_BP_RAS1_TREF_STAGE_MASK 0x3FF
#define GX_BP_TEVCOMBINER_D_SHIFT 4
#define GX_BP_TEVCOMBINER_C_SHIFT 7
#define GX_BP_TEVCOMBINER_B_SHIFT 10
#define GX_BP_TEVCOMBINER_A_SHIFT 13
#define GX_BP_TEVCOMBINER_BIAS_SHIFT 16
#define GX_BP_TEVCOMBINER_OP_SHIFT 18
#define GX_BP_TEVCOMBINER_CLAMP_SHIFT 19
#define GX_BP_TEVCOMBINER_SCALE_SHIFT 20
#define GX_BP_TEVCOMBINER_DEST_SHIFT 22
#define GX_BP_TEVCOMBINER_OP_MASK 0xFFFF0000
#define GX_BP_TEVALPHACOMBINER_IN_MASK 0xFF00FFF0
#define GX_BP_TEVCOLORCOMBINER_IN_MASK 0xFF00FFFF
#define GX_BP_TEVALPHACOMBINER_RSWAP_SHIFT 0
#define GX_BP_TEVALPHACOMBINER_TSWAP_SHIFT 2
#define GX_BP_TEVALPHACOMBINER_SWAP_MASK 0xFF00000F
#define GX_BP_TEVKSEL_KCSEL_LMASK 0x1F

#endif
