#include "revolution/base/PPCArch.h"
#include "revolution/os.h"

typedef union FpscrUnion {
    f64 f;
    struct {
        u32 fpscr_pad;
        u32 fpscr;
    } u;
} FpscrUnion;

asm u32 PPCMfmsr(void) {
    nofralloc
    mfmsr r3
    blr
}

asm void PPCMtmsr(register u32 msr) {
    nofralloc
    mtmsr msr
    blr
}

asm u32 PPCMfhid0(void) {
    nofralloc
    mfspr r3, HID0
    blr
}

asm void PPCMthid0(register u32 hid0) {
    nofralloc
    mtspr HID0, hid0
    blr
}

asm u32 PPCMfl2cr(void) {
    nofralloc
    mfspr r3, L2CR
    blr
}

asm void PPCMtl2cr(register u32 l2cr) {
    nofralloc
    mtspr L2CR, l2cr
    blr
}

asm void PPCMtdec(register u32 dec) {
    nofralloc
    mtdec dec
    blr
}

asm void PPCSync(void) {
    nofralloc
    sc
    blr
}

asm void PPCHalt(void) {
    nofralloc
    sync
loop:
    nop
    li r3, 0
    nop
    b loop
}

asm void PPCMtmmcr0(register u32 mmcr0) {
    nofralloc
    mtspr MMCR0, mmcr0
    blr
}

asm void PPCMtmmcr1(register u32 mmcr1) {
    nofralloc
    mtspr MMCR1, mmcr1
    blr
}

asm void PPCMtpmc1(register u32 pmc1) {
    nofralloc
    mtspr PMC1, pmc1
    blr
}

asm void PPCMtpmc2(register u32 pmc2) {
    nofralloc
    mtspr PMC2, pmc2
    blr
}

asm void PPCMtpmc3(register u32 pmc3) {
    nofralloc
    mtspr PMC3, pmc3
    blr
}

asm void PPCMtpmc4(register u32 pmc4) {
    nofralloc
    mtspr PMC4, pmc4
    blr
}

u32 PPCMffpscr(void) {
    FpscrUnion m;

    asm {
        mffs fp31
        stfd fp31, m.f
    }

    return m.u.fpscr;
}

void PPCMtfpscr(register u32 fpscr) {
    FpscrUnion m;

    asm {
        li r4, 0
        stw r4, m.u.fpscr_pad
        stw fpscr, m.u.fpscr
        lfd fp31, m.f
        mtfsf 0xFF, fp31
    }
}

asm u32 PPCMfhid2(void) {
    nofralloc
    mfspr r3, 0x398
    blr
}

asm void PPCMthid2(register u32 hid2) {
    nofralloc
    mtspr 0x398, hid2
    blr
}

asm u32 PPCMfwpar(void) {
    nofralloc
    sync
    mfspr r3, 0x399
    blr
}

asm void PPCMtwpar(register u32 wpar) {
    nofralloc
    mtspr 0x399, wpar
    blr
}

void PPCDisableSpeculation(void) {
    PPCMthid0(PPCMfhid0() | HID0_SPD);
}

asm void PPCSetFpNonIEEEMode(void) {
    nofralloc
    mtfsb1 29
    blr
}

void PPCMthid4(register u32 hid4) {
    if (hid4 & HID4_H4A) {
        asm { mtspr 0x3F3, hid4 }
    } else {
        OSReport("H4A should not be cleared because of Broadway errata.\n");
        hid4 |= HID4_H4A;
        asm { mtspr 0x3F3, hid4 }
    }
}
