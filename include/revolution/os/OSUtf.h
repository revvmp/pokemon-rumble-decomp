#ifndef REVOLUTION_OS_OSUTF_H
#define REVOLUTION_OS_OSUTF_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

const char* OSUTF8to32(const char* utf8, u32* utf32);
const u16* OSUTF16to32(const u16* utf16, u32* utf32);
u8 OSUTF32toANSI(u32 utf32);
u16 OSUTF32toSJIS(u32 utf32);

#ifdef __cplusplus
}
#endif

#endif
