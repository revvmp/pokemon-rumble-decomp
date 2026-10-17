#ifndef REVOLUTION_GX_GXCPU2EFB_H
#define REVOLUTION_GX_GXCPU2EFB_H

#include "revolution/types.h"
#include "revolution/gx/GXEnum.h"

#ifdef __cplusplus
extern "C" {
#endif

void GXPokeAlphaMode(GXCompare func, u8 threshold);
void GXPokeAlphaRead(GXAlphaReadMode mode);
void GXPokeAlphaUpdate(GXBool updateEnable);
void GXPokeBlendMode(GXBlendMode type, GXBlendFactor srcFactor, GXBlendFactor dstFactor, GXLogicOp op);
void GXPokeColorUpdate(GXBool updateEnable);
void GXPokeDstAlpha(GXBool enable, u8 alpha);
void GXPokeDither(GXBool dither);
void GXPokeZMode(GXBool compareEnable, GXCompare func, GXBool updateEnable);
void GXPeekARGB(u16 x, u16 y, u32* color);
void GXPeekZ(u16 x, u16 y, u32* z);

#ifdef __cplusplus
}
#endif

#endif
