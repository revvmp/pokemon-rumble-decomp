#ifndef NDEVEXI2A_EXI2_H
#define NDEVEXI2A_EXI2_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EXIChannelParam {
    u32 cpr;
    void* mar;
    u32 length;
    u32 cr;
    u32 data;
} EXIChannelParam;

volatile EXIChannelParam EXI_CHAN_PARAMS[3] : 0xCD006800;

#define EXI_CPR_EXIINTMASK (1 << 0)
#define EXI_CPR_TCINTMASK (1 << 2)
#define EXI_CPR_CLK_16MHZ (4 << 4)
#define EXI_CPR_CS0B (1 << 7)
#define EXI_CPR_EXTINTMASK (1 << 10)
#define EXI_CPR_INTMASKS (EXI_CPR_EXIINTMASK | EXI_CPR_TCINTMASK | EXI_CPR_EXTINTMASK)

#define EXI_CR_TSTART (1 << 0)

#define EXI_READ 0
#define EXI_WRITE 1

BOOL __EXI2Imm(void* mem, s32 size, u32 type);
void __DBEXIInit(void);
BOOL __DBEXIReadReg(u32 cmd, void* mem, s32 size);
BOOL __DBEXIWriteReg(u32 cmd, const void* mem, s32 size);
BOOL __DBEXIReadRam(u32 cmd, void* mem, s32 size);
BOOL __DBEXIWriteRam(u32 cmd, const void* mem, s32 size);

static inline u32 __EXISwap32(u32 val) {
    return val >> 24 & 0x000000FF | val >> 8 & 0x0000FF00 | val << 8 & 0x00FF0000 | val << 24 & 0xFF000000;
}

#ifdef __cplusplus
}
#endif

#endif
