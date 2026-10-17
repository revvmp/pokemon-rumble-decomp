#ifndef PRIVATE_FLIPPER_H
#define PRIVATE_FLIPPER_H

#include "revolution/types.h"

vu16 __VIRegs[0x3B] : 0xCC002000;
vu32 __PIRegs[0xC] : 0xCC003000;
vu32 __SIRegs[0x40] : 0xCD006400;

#endif
