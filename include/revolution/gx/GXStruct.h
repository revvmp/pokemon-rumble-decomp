#ifndef REVOLUTION_GX_GXSTRUCT_H
#define REVOLUTION_GX_GXSTRUCT_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GXTexObj {
    u32 dummy[8];
} GXTexObj;

typedef struct _GXTexRegion {
    u32 dummy[4];
} GXTexRegion;

typedef struct _GXTlutRegion {
    u32 dummy[4];
} GXTlutRegion;

#ifdef __cplusplus
}
#endif

#endif
