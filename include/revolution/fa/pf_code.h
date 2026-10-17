#ifndef FA_PF_CODE_H
#define FA_PF_CODE_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const u8 pf_valid_fn_char[96];

s32 PFCODE_Combine_Width(s16 oem_width, s16 uni_width);
void PFCODE_Divide_Width(s32 width, s16* oem_width, s16* uni_width);

u16 PF_GET_LE_U16(const u8* buf);
void PF_LE16_TO_U16_STR(u8* str, u32 num);

#ifdef __cplusplus
}
#endif

#endif
