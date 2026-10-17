#ifndef REVOLUTION_GD_GDTEXTURE_H
#define REVOLUTION_GD_GDTEXTURE_H

#include "revolution/gx/GXEnum.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BP_TEX_MODE0(wrap_s, wrap_t, mag_filt, min_filt, edge_lod, lod_bias, max_aniso, bias_clamp, id)                                    \
    ((u32)(wrap_s) << 0 | (u32)(wrap_t) << 2 | (u32)(mag_filt) << 4 | (u32)(min_filt) << 5 | (u32)(edge_lod) << 8 |                     \
     (u32)(lod_bias) << 9 | (u32)(max_aniso) << 19 | (u32)(bias_clamp) << 21 | (u32)(id) << 24)

#define BP_TEX_MODE1(min_lod, max_lod, id) ((u32)(min_lod) << 0 | (u32)(max_lod) << 8 | (u32)(id) << 24)

#define BP_IMAGE_ATTR(width, height, format, id) ((u32)(width) << 0 | (u32)(height) << 10 | (u32)(format) << 20 | (u32)(id) << 24)

#define BP_IMAGE_PTR(ptr, id) ((u32)(ptr) << 0 | (u32)(id) << 24)

#define BP_TEX_CACHE_EVEN(tmem, size0, size1, type, id)                                                                                    \
    ((u32)(tmem) << 0 | (u32)(size0) << 15 | (u32)(size1) << 18 | (u32)(type) << 21 | (u32)(id) << 24)

#define BP_TEX_CACHE_ODD(tmem, size0, size1, id) ((u32)(tmem) << 0 | (u32)(size0) << 15 | (u32)(size1) << 18 | (u32)(id) << 24)

#define BP_TEXCOORD_S_SCALE(s_scale, s_bias, s_wrap, line_offset, point_offset, id)                                                        \
    ((u32)(s_scale) << 0 | (u32)(s_bias) << 16 | (u32)(s_wrap) << 17 | (u32)(line_offset) << 18 | (u32)(point_offset) << 19 |           \
     (u32)(id) << 24)

#define BP_TEXCOORD_T_SCALE(t_scale, t_bias, t_wrap, id) ((u32)(t_scale) << 0 | (u32)(t_bias) << 16 | (u32)(t_wrap) << 17 | (u32)(id) << 24)

#define GD_TMEM_MAX 0x100000

#define SU_SSIZE0_ID 0x30
#define SU_TSIZE0_ID 0x31

void GDSetTexLookupMode(GXTexMapID id, GXTexWrapMode wrap_s, GXTexWrapMode wrap_t, GXTexFilter min_filt, GXTexFilter mag_filt, f32 min_lod,
                        f32 max_lod, f32 lod_bias, GXBool bias_clamp, GXBool do_edge_lod, GXAnisotropy max_aniso);
void GDSetTexImgAttr(GXTexMapID id, u16 width, u16 height, GXTexFmt format);
void GDSetTexImgPtr(GXTexMapID id, void* image_ptr);
void GDSetTexCached(GXTexMapID id, u32 tmem_even, GXTexCacheSize size_even, u32 tmem_odd, GXTexCacheSize size_odd);
void GDSetTexCoordScale(GXTexCoordID coord, u16 s_scale, u16 t_scale);

#ifdef __cplusplus
}
#endif

#endif
