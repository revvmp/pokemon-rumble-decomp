#ifndef REVOLUTION_BASE_PPCARCH_H
#define REVOLUTION_BASE_PPCARCH_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HID4_H4A 0x80000000
#define HID0_SPD 0x00000200

u32 PPCMfmsr(void);
void PPCMtmsr(u32 msr);
u32 PPCMfhid0(void);
void PPCMthid0(u32 hid0);
u32 PPCMfl2cr(void);
void PPCMtl2cr(u32 l2cr);
void PPCMtdec(u32 dec);
void PPCSync(void);
void PPCHalt(void);
void PPCMtmmcr0(u32 mmcr0);
void PPCMtmmcr1(u32 mmcr1);
void PPCMtpmc1(u32 pmc1);
void PPCMtpmc2(u32 pmc2);
void PPCMtpmc3(u32 pmc3);
void PPCMtpmc4(u32 pmc4);
u32 PPCMffpscr(void);
void PPCMtfpscr(u32 fpscr);
u32 PPCMfhid2(void);
void PPCMthid2(u32 hid2);
u32 PPCMfwpar(void);
void PPCMtwpar(u32 wpar);
void PPCDisableSpeculation(void);
void PPCSetFpNonIEEEMode(void);
void PPCMthid4(u32 hid4);

#ifdef __cplusplus
}
#endif

#endif
