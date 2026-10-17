#ifndef REVOLUTION_GX_GXREGS_H
#define REVOLUTION_GX_GXREGS_H

#include "revolution/types.h"
#include "revolution/base/PPCWGPipe.h"

#ifdef __cplusplus
extern "C" {
#endif

volatile PPCWGPipe gxfifo : 0xCC008000;

extern volatile void* __piReg;
extern volatile void* __cpReg;
extern volatile void* __peReg;
extern volatile void* __memReg;

#define GX_WRITE_U8(value) gxfifo.u8 = (u8)(value)
#define GX_WRITE_U16(value) gxfifo.u16 = (u16)(value)
#define GX_WRITE_U32(value) gxfifo.u32 = (u32)(value)
#define GX_WRITE_F32(value) gxfifo.f32 = (f32)(value)

#define GX_PE_REG_WRITE_U16(index, value) *(vu16*)((vu16*)__peReg + (index)) = (u16)(value)
#define GX_PE_REG_READ_U16(index) *(vu16*)((vu16*)__peReg + (index))
#define GX_MEM_REG_READ_U16(index) *(vu16*)((vu16*)__memReg + (index))

#define FAST_GPFLAGSET(reg, value, field) \
    (reg) = (u32)__rlwimi((int)(reg), (int)(value), field##_SHIFT, 32 - field##_SHIFT - field##_SIZE, 31 - field##_SHIFT)

#define CP_OPCODE(index, cmd) (((u32)(index) << 0) | ((u32)(cmd) << 3))
#define CP_XF_LOADREGS(addr, cnt) (((u32)(addr) << 0) | ((u32)(cnt) << 16))

#define GX_WRITE_XF_REG(addr, data, cnt) \
    { \
        GX_WRITE_U8(CP_OPCODE(0, 2)); \
        GX_WRITE_U32(CP_XF_LOADREGS((addr), (cnt))); \
        GX_WRITE_U32((data)); \
    }

#define GX_WRITE_RA_REG(reg) \
    { \
        GX_WRITE_U8(CP_OPCODE(1, 12)); \
        GX_WRITE_U32((reg)); \
    }

#define XF_REG_NUM_COLORS 0x1009
#define XF_REG_AMBIENT0 0x100A
#define XF_REG_AMBIENT1 0x100B
#define XF_REG_MATERIAL0 0x100C
#define XF_REG_MATERIAL1 0x100D
#define XF_REG_COLOR0_CTRL 0x100E
#define XF_REG_NUM_TEX_GENS 0x103F
#define XF_REG_TEX0 0x1040
#define XF_REG_DUAL_TEX0 0x1050

#define BP_SENT()gx->bpSentNot = GX_FALSE
#define XF_SENT() gx->bpSentNot = GX_TRUE

#define MEM_PE_REQCOUNTH_IDX 0x27
#define MEM_PE_REQCOUNTL_IDX 0x28

inline u32 __GXReadMEMCounterU32(u32 lowIndex, u32 highIndex) {
    u32 high0;
    u32 high1;
    u32 low;

    high0 = GX_MEM_REG_READ_U16(highIndex);
    do {
        high1 = high0;
        low = GX_MEM_REG_READ_U16(lowIndex);
        high0 = GX_MEM_REG_READ_U16(highIndex);
    } while (high0 != high1);

    return (high0 << 16) | low;
}

#define GX_MEM_COUNTER_READ_U32(name) __GXReadMEMCounterU32(name##L_IDX, name##H_IDX)

#define PE_TOKEN_TOKEN_SHIFT 0
#define PE_TOKEN_TOKEN_SIZE 16
#define PE_TOKEN_RID_SHIFT 24
#define PE_TOKEN_RID_SIZE 8
#define PE_TOKEN_INT(token, rid) (((u32)(token) << 0) | ((u32)(rid) << 24))
#define PE_FINISH(dst, rid) (((u32)(dst) << 0) | ((u32)(rid) << 24))

#define SC_PE_TOKEN_SET_TOKEN(reg, token) FAST_GPFLAGSET(reg, token, PE_TOKEN_TOKEN)
#define SC_PE_TOKEN_SET_RID(reg, rid) FAST_GPFLAGSET(reg, rid, PE_TOKEN_RID)

#define PE_CMODE0_BLEND_ENABLE_SHIFT 0
#define PE_CMODE0_BLEND_ENABLE_SIZE 1
#define PE_CMODE0_LOGICOP_ENABLE_SHIFT 1
#define PE_CMODE0_LOGICOP_ENABLE_SIZE 1
#define PE_CMODE0_DITHER_ENABLE_SHIFT 2
#define PE_CMODE0_DITHER_ENABLE_SIZE 1
#define PE_CMODE0_COLOR_MASK_SHIFT 3
#define PE_CMODE0_COLOR_MASK_SIZE 1
#define PE_CMODE0_ALPHA_MASK_SHIFT 4
#define PE_CMODE0_ALPHA_MASK_SIZE 1
#define PE_CMODE0_DFACTOR_SHIFT 5
#define PE_CMODE0_DFACTOR_SIZE 3
#define PE_CMODE0_SFACTOR_SHIFT 8
#define PE_CMODE0_SFACTOR_SIZE 3
#define PE_CMODE0_BLENDOP_SHIFT 11
#define PE_CMODE0_BLENDOP_SIZE 1
#define PE_CMODE0_LOGICOP_SHIFT 12
#define PE_CMODE0_LOGICOP_SIZE 4
#define PE_CMODE0_RID_SHIFT 24
#define PE_CMODE0_RID_SIZE 8

#define SC_PE_CMODE0_SET_BLEND_ENABLE(reg, value) FAST_GPFLAGSET(reg, value, PE_CMODE0_BLEND_ENABLE)
#define SC_PE_CMODE0_SET_LOGICOP_ENABLE(reg, value) FAST_GPFLAGSET(reg, value, PE_CMODE0_LOGICOP_ENABLE)
#define SC_PE_CMODE0_SET_DITHER_ENABLE(reg, value) FAST_GPFLAGSET(reg, value, PE_CMODE0_DITHER_ENABLE)
#define SC_PE_CMODE0_SET_COLOR_MASK(reg, value) FAST_GPFLAGSET(reg, value, PE_CMODE0_COLOR_MASK)
#define SC_PE_CMODE0_SET_ALPHA_MASK(reg, value) FAST_GPFLAGSET(reg, value, PE_CMODE0_ALPHA_MASK)
#define SC_PE_CMODE0_SET_DFACTOR(reg, value) FAST_GPFLAGSET(reg, value, PE_CMODE0_DFACTOR)
#define SC_PE_CMODE0_SET_SFACTOR(reg, value) FAST_GPFLAGSET(reg, value, PE_CMODE0_SFACTOR)
#define SC_PE_CMODE0_SET_BLENDOP(reg, value) FAST_GPFLAGSET(reg, value, PE_CMODE0_BLENDOP)
#define SC_PE_CMODE0_SET_LOGICOP(reg, value) FAST_GPFLAGSET(reg, value, PE_CMODE0_LOGICOP)
#define SC_PE_CMODE0_SET_RID(reg, value) FAST_GPFLAGSET(reg, value, PE_CMODE0_RID)

#define PE_CMODE1_CONSTANT_ALPHA_SHIFT 0
#define PE_CMODE1_CONSTANT_ALPHA_SIZE 8
#define PE_CMODE1_CONSTANT_ALPHA_ENABLE_SHIFT 8
#define PE_CMODE1_CONSTANT_ALPHA_ENABLE_SIZE 1

#define SC_PE_CMODE1_SET_CONSTANT_ALPHA(reg, value) FAST_GPFLAGSET(reg, value, PE_CMODE1_CONSTANT_ALPHA)
#define SC_PE_CMODE1_SET_CONSTANT_ALPHA_ENABLE(reg, value) FAST_GPFLAGSET(reg, value, PE_CMODE1_CONSTANT_ALPHA_ENABLE)

#define PE_ZMODE_ENABLE_SHIFT 0
#define PE_ZMODE_ENABLE_SIZE 1
#define PE_ZMODE_FUNC_SHIFT 1
#define PE_ZMODE_FUNC_SIZE 3
#define PE_ZMODE_MASK_SHIFT 4
#define PE_ZMODE_MASK_SIZE 1

#define SC_PE_ZMODE_SET_ENABLE(reg, value) FAST_GPFLAGSET(reg, value, PE_ZMODE_ENABLE)
#define SC_PE_ZMODE_SET_FUNC(reg, value) FAST_GPFLAGSET(reg, value, PE_ZMODE_FUNC)
#define SC_PE_ZMODE_SET_MASK(reg, value) FAST_GPFLAGSET(reg, value, PE_ZMODE_MASK)

#define PE_PI_CTL_AFMT_SHIFT 0
#define PE_PI_CTL_AFMT_SIZE 2
#define PE_PI_CTL_ZFMT_SHIFT 2
#define PE_PI_CTL_ZFMT_SIZE 1

#define SC_PE_PI_CTL_SET_AFMT(reg, value) FAST_GPFLAGSET(reg, value, PE_PI_CTL_AFMT)
#define SC_PE_PI_CTL_SET_ZFMT(reg, value) FAST_GPFLAGSET(reg, value, PE_PI_CTL_ZFMT)

#define PE_PI_EFB_ADDR_X_SHIFT 2
#define PE_PI_EFB_ADDR_X_SIZE 10
#define PE_PI_EFB_ADDR_Y_SHIFT 12
#define PE_PI_EFB_ADDR_Y_SIZE 10
#define PE_PI_EFB_ADDR_TYPE_SHIFT 22
#define PE_PI_EFB_ADDR_TYPE_SIZE 2

#define SC_PE_PI_EFB_ADDR_SET_X(reg, value) FAST_GPFLAGSET(reg, value, PE_PI_EFB_ADDR_X)
#define SC_PE_PI_EFB_ADDR_SET_Y(reg, value) FAST_GPFLAGSET(reg, value, PE_PI_EFB_ADDR_Y)
#define SC_PE_PI_EFB_ADDR_SET_TYPE(reg, value) FAST_GPFLAGSET(reg, value, PE_PI_EFB_ADDR_TYPE)

#define PE_INTRCTL_INT0EN_SHIFT 0
#define PE_INTRCTL_INT0EN_SIZE 1
#define PE_INTRCTL_INT1EN_SHIFT 1
#define PE_INTRCTL_INT1EN_SIZE 1
#define PE_INTRCTL_INT0CLR_SHIFT 2
#define PE_INTRCTL_INT0CLR_SIZE 1
#define PE_INTRCTL_INT1CLR_SHIFT 3
#define PE_INTRCTL_INT1CLR_SIZE 1

#define SC_PE_INTRCTL_SET_INT0EN(reg, value) FAST_GPFLAGSET(reg, value, PE_INTRCTL_INT0EN)
#define SC_PE_INTRCTL_SET_INT1EN(reg, value) FAST_GPFLAGSET(reg, value, PE_INTRCTL_INT1EN)
#define SC_PE_INTRCTL_SET_INT0CLR(reg, value) FAST_GPFLAGSET(reg, value, PE_INTRCTL_INT0CLR)
#define SC_PE_INTRCTL_SET_INT1CLR(reg, value) FAST_GPFLAGSET(reg, value, PE_INTRCTL_INT1CLR)

#define PE_REG_ZMODE 0
#define PE_REG_CMODE0 1
#define PE_REG_CMODE1 2
#define PE_REG_ALPHA_MODE 3
#define PE_REG_PI_CTL 4
#define PE_REG_INTRCTL 5
#define PE_REG_TOKEN 7

#define TX_LOADBLOCK3(count, format, rid) (((u32)(count) << 0) | ((u32)(format) << 15) | ((u32)(rid) << 24))

#define SU_LPSIZE_LSIZE_SHIFT 0
#define SU_LPSIZE_LSIZE_SIZE 8
#define SU_LPSIZE_PSIZE_SHIFT 8
#define SU_LPSIZE_PSIZE_SIZE 8
#define SU_LPSIZE_LTOFF_SHIFT 16
#define SU_LPSIZE_LTOFF_SIZE 3
#define SU_LPSIZE_PTOFF_SHIFT 19
#define SU_LPSIZE_PTOFF_SIZE 3

#define SC_SU_LPSIZE_SET_LSIZE(reg, value) FAST_GPFLAGSET(reg, value, SU_LPSIZE_LSIZE)
#define SC_SU_LPSIZE_SET_PSIZE(reg, value) FAST_GPFLAGSET(reg, value, SU_LPSIZE_PSIZE)
#define SC_SU_LPSIZE_SET_LTOFF(reg, value) FAST_GPFLAGSET(reg, value, SU_LPSIZE_LTOFF)
#define SC_SU_LPSIZE_SET_PTOFF(reg, value) FAST_GPFLAGSET(reg, value, SU_LPSIZE_PTOFF)

#define SU_TS0_LF_SHIFT 18
#define SU_TS0_LF_SIZE 1
#define SU_TS0_PF_SHIFT 19
#define SU_TS0_PF_SIZE 1

#define SC_SU_TS0_SET_LF(reg, value) FAST_GPFLAGSET(reg, value, SU_TS0_LF)
#define SC_SU_TS0_SET_PF(reg, value) FAST_GPFLAGSET(reg, value, SU_TS0_PF)

#define SU_SSMASK(mask, rid) (((u32)(mask) << 0) | ((u32)(rid) << 24))

#define GEN_MODE_NTEX_SHIFT 0
#define GEN_MODE_NTEX_SIZE 4
#define GEN_MODE_NCOL_SHIFT 4
#define GEN_MODE_NCOL_SIZE 3
#define GEN_MODE_REJECT_EN_SHIFT 14
#define GEN_MODE_REJECT_EN_SIZE 2
#define GEN_MODE_ZFREEZE_SHIFT 19
#define GEN_MODE_ZFREEZE_SIZE 1

#define SC_GEN_MODE_SET_REJECT_EN(reg, value) FAST_GPFLAGSET(reg, value, GEN_MODE_REJECT_EN)
#define SC_GEN_MODE_SET_ZFREEZE(reg, value) FAST_GPFLAGSET(reg, value, GEN_MODE_ZFREEZE)

#ifdef __cplusplus
}
#endif

#endif
