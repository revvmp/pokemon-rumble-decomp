#include "revolution/gd.h"
#include "revolution/os/OSAddress.h"

u8 GDTexMode0Ids[8] = {0x80, 0x81, 0x82, 0x83, 0xA0, 0xA1, 0xA2, 0xA3};
u8 GDTexMode1Ids[8] = {0x84, 0x85, 0x86, 0x87, 0xA4, 0xA5, 0xA6, 0xA7};
u8 GDTexImage0Ids[8] = {0x88, 0x89, 0x8A, 0x8B, 0xA8, 0xA9, 0xAA, 0xAB};
u8 GDTexImage1Ids[8] = {0x8C, 0x8D, 0x8E, 0x8F, 0xAC, 0xAD, 0xAE, 0xAF};
u8 GDTexImage2Ids[8] = {0x90, 0x91, 0x92, 0x93, 0xB0, 0xB1, 0xB2, 0xB3};
u8 GDTexImage3Ids[8] = {0x94, 0x95, 0x96, 0x97, 0xB4, 0xB5, 0xB6, 0xB7};

u8 GD2HWFiltConv[] = {0, 4, 1, 5, 2, 6};

void GDSetTexLookupMode(GXTexMapID id, GXTexWrapMode wrap_s, GXTexWrapMode wrap_t, GXTexFilter min_filt, GXTexFilter mag_filt, f32 min_lod,
                        f32 max_lod, f32 lod_bias, GXBool bias_clamp, GXBool do_edge_lod, GXAnisotropy max_aniso) {
    GDWriteBPCmd(BP_TEX_MODE0(wrap_s, wrap_t, mag_filt == GX_LINEAR, GD2HWFiltConv[min_filt], !do_edge_lod, (u8)(s8)(lod_bias * 32.0f),
                              max_aniso, bias_clamp, GDTexMode0Ids[id]));
    GDWriteBPCmd(BP_TEX_MODE1((u8)(min_lod * 16.0f), (u8)(max_lod * 16.0f), GDTexMode1Ids[id]));
}

void GDSetTexImgAttr(GXTexMapID id, u16 width, u16 height, GXTexFmt format) {
    GDWriteBPCmd(BP_IMAGE_ATTR(width - 1, height - 1, format, GDTexImage0Ids[id]));
}

void GDSetTexImgPtr(GXTexMapID id, void* image_ptr) {
    GDWriteBPCmd(BP_IMAGE_PTR(OSCachedToPhysical(image_ptr) >> 5, GDTexImage3Ids[id]));
}

void GDSetTexCached(GXTexMapID id, u32 tmem_even, GXTexCacheSize size_even, u32 tmem_odd, GXTexCacheSize size_odd) {
    GDWriteBPCmd(BP_TEX_CACHE_EVEN(tmem_even >> 5, size_even + 3, size_even + 3, 0, GDTexImage1Ids[id]));

    if (size_odd != GX_TEXCACHE_NONE && tmem_odd < GD_TMEM_MAX) {
        GDWriteBPCmd(BP_TEX_CACHE_ODD(tmem_odd >> 5, size_odd + 3, size_odd + 3, GDTexImage2Ids[id]));
    }
}

void GDSetTexCoordScale(GXTexCoordID coord, u16 s_scale, u16 t_scale) {
    GDWriteBPCmd(SS_MASK(0x00FFFF));
    GDWriteBPCmd(BP_TEXCOORD_S_SCALE(s_scale - 1, 0, 0, 0, 0, SU_SSIZE0_ID + coord * 2));
    GDWriteBPCmd(SS_MASK(0x00FFFF));
    GDWriteBPCmd(BP_TEXCOORD_T_SCALE(t_scale - 1, 0, 0, SU_TSIZE0_ID + coord * 2));
}
