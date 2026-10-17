#ifndef REVOLUTION_GX_GXCULL_H
#define REVOLUTION_GX_GXCULL_H

#include "revolution/types.h"
#include "revolution/gx/GXEnum.h"

#ifdef __cplusplus
extern "C" {
#endif

void GXSetCullMode(GXCullMode mode);
void GXSetCoPlanar(GXBool enable);

#ifdef __cplusplus
}
#endif

#endif
