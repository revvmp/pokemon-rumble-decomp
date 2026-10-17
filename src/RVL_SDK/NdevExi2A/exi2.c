#include "NdevExi2A/exi2.h"
#include "revolution/os/OSInterrupt.h"

#define EXI2 EXI_CHAN_PARAMS[2]

static BOOL __EXI2Select(void) {
    u32 cpr = EXI2.cpr;
    EXI2.cpr = (cpr & EXI_CPR_INTMASKS) | EXI_CPR_CLK_16MHZ | EXI_CPR_CS0B;
    return TRUE;
}

static BOOL __EXI2Deselect(void) {
    u32 cpr = EXI2.cpr;
    EXI2.cpr = cpr & EXI_CPR_INTMASKS;
    return TRUE;
}

static BOOL __EXI2Sync(void) {
    while (EXI2.cr & EXI_CR_TSTART) {
    }

    return TRUE;
}

BOOL __EXI2Imm(void* mem, s32 size, u32 type) {
    int i;

    if (type != EXI_READ) {
        u32 imm = 0;

        for (i = 0; i < size; i++) {
            const u8* bmem = (const u8*)mem;
            imm |= bmem[i] << (3 - i) * 8;
        }

        EXI2.data = imm;
    }

    EXI2.cr = type << 2 | EXI_CR_TSTART | (size - 1) * 16;

    __EXI2Sync();

    if (type == EXI_READ) {
        u32 imm = EXI2.data;
        u8* bmem = (u8*)mem;

        for (i = 0; i < size; i++, bmem++) {
            *bmem = imm >> (3 - i) * 8;
        }
    }

    return TRUE;
}

void __DBEXIInit(void) {
    u32 initCmd;
    u32 resetCmd;

    __OSMaskInterrupts(OS_INTERRUPTMASK_EXI_2);

    while ((EXI2.cr & EXI_CR_TSTART) == EXI_CR_TSTART) {
    }

    EXI2.cpr = 0;

    initCmd = 0xB4000000;
    resetCmd = 0xD4000000;

    __EXI2Select();
    __EXI2Imm(&initCmd, sizeof(initCmd), EXI_WRITE);
    __EXI2Sync();
    __EXI2Imm(&resetCmd, sizeof(resetCmd), EXI_WRITE);
    __EXI2Sync();
    __EXI2Deselect();
}

BOOL __DBEXIReadReg(u32 cmd, void* mem, s32 size) {
    BOOL error = FALSE;
    u32 readVal = 0;

    error |= !__EXI2Select();
    error |= !__EXI2Imm(&cmd, sizeof(cmd), EXI_WRITE);
    error |= !__EXI2Sync();
    error |= !__EXI2Imm(&readVal, sizeof(readVal), EXI_READ);
    error |= !__EXI2Sync();
    error |= !__EXI2Deselect();

    switch (size) {
    case 1:
        *(u8*)mem = readVal >> 24;
        break;
    case 2:
        *(u16*)mem = readVal >> 24 | readVal >> 8 & 0xFF00;
        break;
    default:
        *(u32*)mem = __EXISwap32(readVal);
        break;
    }

    return error == FALSE;
}

BOOL __DBEXIWriteReg(u32 cmd, const void* mem, s32 size) {
    BOOL error = FALSE;
    u32 writeVal = 0;

    switch (size) {
    case 1:
        writeVal = *(u8*)mem << 24;
        break;
    case 2:
        writeVal = *(u16*)mem << 24 | (*(u16*)mem & 0xFF00) << 8;
        break;
    default:
        writeVal = __EXISwap32(*(u32*)mem);
        break;
    }

    error |= !__EXI2Select();
    error |= !__EXI2Imm(&cmd, sizeof(cmd), EXI_WRITE);
    error |= !__EXI2Sync();
    error |= !__EXI2Imm(&writeVal, sizeof(writeVal), EXI_WRITE);
    error |= !__EXI2Sync();
    error |= !__EXI2Deselect();

    return error == FALSE;
}

BOOL __DBEXIReadRam(u32 cmd, void* mem, s32 size) {
    BOOL error = FALSE;
    u32 readVal;
    u32* lmem = (u32*)mem;

    error |= !__EXI2Select();
    error |= !__EXI2Imm(&cmd, sizeof(cmd), EXI_WRITE);
    error |= !__EXI2Sync();

    for (; size > 0; size -= 4) {
        error |= !__EXI2Imm(&readVal, sizeof(readVal), EXI_READ);
        error |= !__EXI2Sync();
        *lmem++ = readVal;
    }

    error |= !__EXI2Deselect();

    return error == FALSE;
}

BOOL __DBEXIWriteRam(u32 cmd, const void* mem, s32 size) {
    BOOL error = FALSE;
    u32* lmem = (u32*)mem;

    error |= !__EXI2Select();
    error |= !__EXI2Imm(&cmd, sizeof(cmd), EXI_WRITE);
    error |= !__EXI2Sync();

    for (; size > 0; size -= 4) {
        u32 writeVal = *lmem++;
        error |= !__EXI2Imm(&writeVal, sizeof(writeVal), EXI_WRITE);
        error |= !__EXI2Sync();
    }

    error |= !__EXI2Deselect();

    return error == FALSE;
}
