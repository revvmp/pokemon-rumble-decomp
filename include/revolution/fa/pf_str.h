#ifndef FA_PF_STR_H
#define FA_PF_STR_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PF_CODE_MODE_OEM 1
#define PF_CODE_MODE_UNICODE 2

#define PF_STR_POS_HEAD 1
#define PF_STR_POS_TAIL 2
#define PF_STR_POS_MARK 3

typedef struct PF_CHARCODE {
    s32 (*oem2unicode)(const s8* oem, u16* unicode);
    s32 (*unicode2oem)(const u16* unicode, s8* oem);
    s32 (*oem_char_width)(const s8* oem);
    u32 (*is_oem_mb_char)(s8 c, u32 mode);
    s32 (*unicode_char_width)(const u16* unicode);
    u32 (*is_unicode_mb_char)(u16 c, u32 mode);
} PF_CHARCODE;

#define PF_SETTING_CODE_MODE_MASK 0x3

typedef struct PF_VOLUME_SET {
    u8 state[0x40];
    s32 last_error;
    s32 last_driver_error;
    PF_CHARCODE codeset;
    u32 setting;
} PF_VOLUME_SET;

extern PF_VOLUME_SET pf_vol_set;

typedef struct PF_STR {
    const s8* p_head;
    const s8* p_tail;
    const s8* p_mark;
    u32 code_mode;
} PF_STR;

void PFSTR_SetCodeMode(PF_STR* p_str, u32 code_mode);
u32 PFSTR_GetCodeMode(PF_STR* p_str);
void PFSTR_SetMarkPos(PF_STR* p_str, const s8* p_mark);
const s8* PFSTR_GetStrPos(PF_STR* p_str, u32 target);
void PFSTR_MoveStrPos(PF_STR* p_str, s16 num_char);
s32 PFSTR_InitStr(PF_STR* p_str, const s8* s, u32 code_mode);
u16 PFSTR_StrLen(PF_STR* p_str);
u16 PFSTR_StrNumChar(PF_STR* p_str, u32 target);
s32 PFSTR_StrCmp(const PF_STR* p_str, const s8* s);
s32 PFSTR_StrNCmp(PF_STR* p_str, const s8* s, u32 target, s32 offset, u16 num);
void PFSTR_ToUpperNStr(PF_STR* p_str, u16 num, s8* dest);

#ifdef __cplusplus
}
#endif

#endif
