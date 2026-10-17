#ifndef REVOLUTION_GX_GXGEOMETRY_H
#define REVOLUTION_GX_GXGEOMETRY_H

#include "revolution/types.h"
#include "revolution/gx/GXEnum.h"

#ifdef __cplusplus
extern "C" {
#endif

void GXBegin(GXPrimitive type, GXVtxFmt vtxFmt, u16 numVerts);

static inline void GXEnd(void) {}

void GXSetLineWidth(u8 width, GXTexOffset texOffsets);
void GXSetPointSize(u8 pointSize, GXTexOffset texOffsets);
void GXEnableTexOffsets(GXTexCoordID coord, GXBool lineEnable, GXBool pointEnable);

#ifdef __cplusplus
}
#endif

#endif
