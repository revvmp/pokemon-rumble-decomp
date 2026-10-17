#include <revolution/fa/pf_clib.h>
#include <revolution/fa/pf_str.h>

void PFSTR_SetCodeMode(PF_STR* p_str, u32 code_mode) {
    p_str->code_mode = code_mode;
}

u32 PFSTR_GetCodeMode(PF_STR* p_str) {
    return p_str->code_mode;
}

void PFSTR_SetMarkPos(PF_STR* p_str, const s8* p_mark) {
    if (p_str->code_mode == PF_CODE_MODE_OEM || p_mark == 0) {
        p_str->p_mark = p_str->p_head;
    } else {
        p_str->p_mark = p_mark;
    }
}

const s8* PFSTR_GetStrPos(PF_STR* p_str, u32 target) {
    if (target == PF_STR_POS_HEAD) {
        return p_str->p_head;
    }
    if (target == PF_STR_POS_TAIL) {
        return p_str->p_tail;
    }
    return p_str->p_mark;
}

void PFSTR_MoveStrPos(PF_STR* p_str, s16 num_char) {
    s16 cnt;
    s16 offset = 0;
    s32 width;
    const s8* p;
    const u16* wp;

    if (PFSTR_GetCodeMode(p_str) == PF_CODE_MODE_OEM) {
        p = p_str->p_head;
        while (num_char != 0) {
            if (pf_vol_set.codeset.is_oem_mb_char(*p, TRUE)) {
                offset++;
            }
            offset++;
            num_char--;
        }
    } else {
        wp = (const u16*)p_str->p_head;
        for (cnt = 0; cnt < num_char; cnt++) {
            width = pf_vol_set.codeset.unicode_char_width(wp);
            wp += width;
            offset += (s16)width;
        }
    }

    p_str->p_head += offset;
}

s32 PFSTR_InitStr(PF_STR* p_str, const s8* s, u32 code_mode) {
    if (code_mode == PF_CODE_MODE_OEM) {
        p_str->p_head = s;
        p_str->p_tail = s + pf_strlen(s);
    } else if (code_mode == PF_CODE_MODE_UNICODE) {
        p_str->p_head = s;
        p_str->p_tail = s + pf_w_strlen((const u16*)s) * sizeof(u16);
    }

    PFSTR_SetCodeMode(p_str, code_mode);
    return 0;
}

u16 PFSTR_StrLen(PF_STR* p_str) {
    return p_str->p_tail - p_str->p_head;
}

u16 PFSTR_StrNumChar(PF_STR* p_str, u32 target) {
    const s8* p;
    u16 cnt;

    if (target == PF_STR_POS_HEAD) {
        p = p_str->p_head;
    } else {
        p = p_str->p_tail;
    }

    if (p_str->code_mode == PF_CODE_MODE_OEM) {
        for (cnt = 0; *p != 0; cnt++) {
            if (pf_vol_set.codeset.is_oem_mb_char(*p, TRUE)) {
                p++;
            }
            p++;
        }
    } else {
        for (cnt = 0; p[0] != 0 || p[1] != 0; cnt++) {
            p += pf_vol_set.codeset.unicode_char_width((const u16*)p);
        }
    }

    return cnt;
}

s32 PFSTR_StrCmp(const PF_STR* p_str, const s8* s) {
    u16 wc;
    const u16* wp;
    s32 ret;

    if (p_str->code_mode == PF_CODE_MODE_OEM) {
        ret = pf_strcmp(p_str->p_head, s);
    } else {
        wp = (const u16*)p_str->p_head;
        do {
            pf_vol_set.codeset.oem2unicode(s, &wc);
            s++;
            if (*wp++ != wc) {
                break;
            }
        } while (wp[-1] != 0 && wc != 0);

        ret = wp[-1] - wc;
    }

    return ret;
}
