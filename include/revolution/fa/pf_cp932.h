#ifndef FA_PF_CP932_H
#define FA_PF_CP932_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 PFCODE_CP932_OEM2Unicode(const s8* cp932_src, u16* unicode_dst);
s32 PFCODE_CP932_Unicode2OEM(const u16* unicode_src, s8* cp932_dst);
s32 PFCODE_CP932_OEMCharWidth(const s8* buf);
u32 PFCODE_CP932_isOEMMBchar(s8 cp932, u32 num);
s32 PFCODE_CP932_UnicodeCharWidth(const u16* buf);
u32 PFCODE_CP932_isUnicodeMBchar(u16 unicode, u32 num);

#ifdef __cplusplus
}
#endif

#endif
