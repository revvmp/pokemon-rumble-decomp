#ifndef NW4R_G3D_PLATFORM_GX_HARDWARE_H
#define NW4R_G3D_PLATFORM_GX_HARDWARE_H
#include <nw4r/types_nw4r.h>

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

#define GX_BP_OPCODE_SHIFT 24
#define GX_BP_CMD_SZ (sizeof(u8) + sizeof(u32))
#define GX_CP_CMD_SZ (sizeof(u8) + sizeof(u8) + sizeof(u32))
#define GX_XF_CMD_SZ (sizeof(u8) + sizeof(u32) + sizeof(u32))

enum {
    GX_BP_REG_SSMASK = 0xFE
};

#endif
