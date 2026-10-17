#ifndef PRIVATE_FLIPPER_H
#define PRIVATE_FLIPPER_H

#include "revolution/types.h"

vu16 __VIRegs[0x3B] : 0xCC002000;
vu32 __PIRegs[0xC] : 0xCC003000;
vu16 __MEMRegs[0x40] : 0xCC004000;
vu16 __DSPRegs[0x20] : 0xCC005000;
vu32 __EXIRegs[0x10] : 0xCD006800;
vu32 __AIRegs[0x8] : 0xCD006C00;
vu32 __ACRRegs[0x59] : 0xCD000000;
vu32 __DIRegs[0x10] : 0xCD006000;
vu32 __SIRegs[0x40] : 0xCD006400;

#endif
