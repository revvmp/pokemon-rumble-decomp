#include <revolution/fa/pf_path.h>
#include <revolution/fa/pf_clib.h>
#include <revolution/fa/pf_volume.h>
#include <revolution/fa/pf_code.h>
#include <stddef.h>
#define PF_CODE_C_TO_WC(x0, x1) ((u16)(((u8)(x0) << 8) + (u8)(x1)))
#define PF_CODE_C_TO_WC_ARR(x, i) PF_CODE_C_TO_WC(x[i], x[i + 1])

#define VALID_PATH_CHAR(x, b) ((u8)(x) >= 0x80 || ((u8)(x) >= ' ' && (pf_valid_fn_char[(u8)(x) - ' '] & b)))
#define VALID_PATH_WCHAR(x, b)                                                                                                                       \
    ((x) >= 0x80 || (u8)(x) >= ' ' && ((u8)(x) & 0xFF00) == 0 && (pf_valid_fn_char[(u8)((x & 0x00FF) - ' ')] & b))

static s32 PFPATH_DoSplitPath(PF_STR* p_path, PF_STR* p_dir_path, PF_STR* p_filename, u32 wildcard) {
    s8* p;
    s8* p_tail_prev;
    u32 code_mode;
    PF_STR token;
    PF_STR token_prev;
    s32 err;

    code_mode = PFSTR_GetCodeMode(p_path);
    PFSTR_SetCodeMode(p_dir_path, code_mode);
    PFSTR_SetCodeMode(p_filename, code_mode);
    p_dir_path->p_head = NULL;
    if (p_filename != NULL) {
        p_filename->p_head = NULL;
    }
    if (PFSTR_StrNCmp(p_path, (s8*)":", 1, 1, 1) == 0) {
        PFSTR_MoveStrPos(p_path, 2);
    }
    p = (s8*)p_path->p_head;
    p_tail_prev = p;
    VFiPFPATH_InitTokenOfPath(&token, p, code_mode);
    err = VFiPFPATH_GetNextTokenOfPath(&token, wildcard);
    if (err != 0) {
        return err;
    }
    if (token.p_head == NULL || PFSTR_StrLen(&token) == 0) {
        return 2;
    }
    if (p_path->p_tail < token.p_tail) {
        return 2;
    }
    token_prev = token;
    while (1) {
        if (PF_IS_PATH_NULL(&token, 2, 0)) {
            p_tail_prev = (s8*)token_prev.p_tail;
            break;
        }
        err = VFiPFPATH_GetNextTokenOfPath(&token, wildcard);
        if (err != 0) {
            return err;
        }
        if (PFSTR_StrLen(&token) == 0) {
            return 2;
        }
        if (p_path->p_tail < token.p_tail) {
            break;
        }
        if (token.p_head == NULL) {
            break;
        }
        p_tail_prev = (s8*)token_prev.p_tail;
        token_prev = token;
    }
    p_dir_path->p_head = p_path->p_head;
    p_dir_path->p_tail = p_tail_prev;
    if (p_filename != NULL) {
        *p_filename = token_prev;
    }
    return 0;
}

static u32 PFPATH_UNI_ConvertFWchar(s32 src, u16* dst) {
    if (src >= 0xFF41UL && src <= 0xFF5AUL) {
        *dst = src - ' ';
        return 1;
    }
    return 0;
}

static u32 PFPATH_OEM_ConvertFWchar(const s8* src, u16* dst) {
    pf_vol_set.codeset.oem2unicode((s8*)src, dst);
    if (PFPATH_UNI_ConvertFWchar(*dst, dst) == 1) {
        pf_vol_set.codeset.unicode2oem(dst, (s8*)dst);
        *dst = (u16)*dst;
        return 1;
    }
    return 0;
}

static u16 PFPATH_GetNextCharOfShortName(PF_FILE_NAME_ITER* p_name) {
    u32 wc;
    s8 c;

    if (p_name->index >= 12) {
        return 0;
    }
    wc = (u8)p_name->buf[p_name->index++];
    if (pf_vol_set.codeset.is_oem_mb_char(wc, 1) != 0) {
        c = p_name->buf[p_name->index++];
        wc = PF_CODE_C_TO_WC(wc, c);
    }

    return ('a' <= wc) && (wc <= 'z') ? wc - ' ' : wc;
}

static u16 PFPATH_GetNextCharOfLongName(PF_FILE_NAME_ITER* p_name) {
    u16 wc;
    u16 wchar;

    if (p_name->index > 522) {
        return 0;
    }
    wc = *(u16*)&p_name->buf[p_name->index];
    p_name->index += sizeof(u16);

    wc = ('a' <= wc) && (wc <= 'z') ? wc - ' ' : wc;
    if (PFPATH_UNI_ConvertFWchar(wc, &wchar) == 1) {
        wc = wchar;
    }
    return wc;
}

static u16 PFPATH_GetNextCharOfFileName(PF_FILE_NAME_ITER* p_name) {
    if (p_name->is_long_name) {
        return PFPATH_GetNextCharOfLongName(p_name);
    }
    return PFPATH_GetNextCharOfShortName(p_name);
}

static u16 PFPATH_GetNextCharOfPattern(PF_STR* p_pattern, u32 is_long_name) {
    u16 wc;
    u16 tmp_wc;
    s8 pattern[3];
    u16 twc;

    if (p_pattern->code_mode == 1) {
        pattern[0] = *p_pattern->p_head++;
        pattern[1] = 0;
        if (pattern[0] == 0 || p_pattern->p_tail < p_pattern->p_head) {
            return 0;
        }
        if (pf_vol_set.codeset.is_oem_mb_char(pattern[0], 1) != 0) {
            pattern[1] = *p_pattern->p_head++;
            wc = PF_CODE_C_TO_WC_ARR(pattern, 0);
        } else if ((pf_vol_set.config & 0x10000) != 0 || VALID_PATH_CHAR(pattern[0], 2)) {
            wc = (u8)pattern[0];
        } else {
            wc = '_';
        }
        if (is_long_name) {
            pf_vol_set.codeset.oem2unicode(&pattern[0], &wc);
        }
    } else {
        pattern[0] = (u8)(*(u16*)p_pattern->p_head);
        pattern[1] = (u8)(*(u16*)p_pattern->p_head >> 8);
        p_pattern->p_head += 2;
        pattern[2] = 0;
        if (((pattern[0] == 0) && (pattern[1] == 0)) || (p_pattern->p_tail < p_pattern->p_head)) {
            return 0;
        }
        wc = ((u8)pattern[1] << 8) + (u8)pattern[0];
        if (is_long_name == 0) {
            pf_vol_set.codeset.unicode2oem(&wc, (s8*)&tmp_wc);
            tmp_wc = (u16)tmp_wc;
            if (pf_vol_set.codeset.is_oem_mb_char((u8)(tmp_wc >> 8), 1) != 0) {
                wc = tmp_wc;
            } else {
                pattern[0] = (u8)(tmp_wc >> 8);
                if ((pf_vol_set.config & 0x10000) != 0 || VALID_PATH_CHAR(pattern[0], 2)) {
                    wc = (u8)pattern[0];
                } else {
                    wc = '_';
                }
            }
        }
    }
    wc = ('a' <= wc) && (wc <= 'z') ? wc - ' ' : wc;
    if (is_long_name) {
        if (PFPATH_UNI_ConvertFWchar(wc, &twc) == 1) {
            wc = twc;
        }
    } else {
        if ((wc >> 8) == 0) {
            pattern[0] = wc;
            pattern[1] = 0;
        } else {
            pattern[0] = (u8)(wc >> 8);
            pattern[1] = (u8)wc;
        }
        if (PFPATH_OEM_ConvertFWchar(pattern, &twc) == 1) {
            wc = twc;
        }
    }
    return wc;
}

static u32 PFPATH_DoMatchFileNameWithPattern(u16 c_name, PF_FILE_NAME_ITER* p_name, u16 c_pat, PF_STR* p_pattern, u32 is_long_name) {
    PF_FILE_NAME_ITER name;
    PF_STR pattern;

    for (; c_pat != 0; c_pat = PFPATH_GetNextCharOfPattern(p_pattern, is_long_name)) {
        switch (c_pat) {
            case '?': {
                if (c_name == 0) {
                    return 0;
                }
                break;
            }
            case '*': {
                for (c_pat = PFPATH_GetNextCharOfPattern(p_pattern, is_long_name); c_pat == '*' || c_pat == '?';
                     c_pat = PFPATH_GetNextCharOfPattern(p_pattern, is_long_name)) {
                }
                if (c_pat == 0) {
                    return 1;
                }

                for (; c_name != 0; c_name = PFPATH_GetNextCharOfFileName(p_name)) {
                    if (c_name == c_pat) {
                        name = *p_name;
                        pattern = *p_pattern;
                        c_name = PFPATH_GetNextCharOfFileName(&name);
                        c_pat = PFPATH_GetNextCharOfPattern(&pattern, is_long_name);
                        if (PFPATH_DoMatchFileNameWithPattern(c_name, &name, c_pat, &pattern, is_long_name) != 0) {
                            return 1;
                        }
                    }
                }
                return 0;
                break;
            }
            default: {
                if (c_name != c_pat) {
                    return 0;
                }
                break;
            }
        }

        c_name = PFPATH_GetNextCharOfFileName(p_name);
    }
    if (c_name != 0) {
        return 0;
    }
    return 1;
}

static s32 PFPATH_cmpNameImpl(const s8* sName, const s8* sPattern, u32* p_is_end) {
    s32 pw;
    s32 nw;
    s32 ret;
    u16 p;
    u16 n;
    u16 tp;
    u16 tn;

    while (*sPattern != 0) {
        pw = pf_vol_set.codeset.oem_char_width((s8*)sPattern);
        nw = pf_vol_set.codeset.oem_char_width((s8*)sName);
        p = pw == 1 ? pf_toupper(*sPattern) : PF_GET_LE_U16((u8*)sPattern);

        n = nw == 1 ? pf_toupper(*sName) : PF_GET_LE_U16((u8*)sName);
        if (pw == 2 && PFPATH_OEM_ConvertFWchar(sPattern, &tp) == 1) {
            p = tp;
        }
        if (nw == 2 && PFPATH_OEM_ConvertFWchar(sName, &tn) == 1) {
            n = tn;
        }
        sPattern = &sPattern[pw];
        switch (p) {
            case '?':
                if (n == 0) {
                    return 1;
                }
                break;
            case '*': {
                do {
                    pw = pf_vol_set.codeset.oem_char_width((s8*)sPattern);
                    p = pw == 1 ? pf_toupper(*sPattern) : PF_GET_LE_U16((u8*)sPattern);
                    sPattern = &sPattern[pw];
                    if (p == '?') {
                        if (n == 0) {
                            return 1;
                        }
                        sName = &sName[nw];
                        nw = pf_vol_set.codeset.oem_char_width((s8*)sName);

                        n = nw == 1 ? pf_toupper(*sName) : PF_GET_LE_U16((u8*)sName);
                    }
                } while (p == '?' || p == '*');
                if (p == 0) {
                    return 0;
                }
                for (; n != 0; n = nw == 1 ? pf_toupper(*sName) : PF_GET_LE_U16((u8*)sName)) {
                    sName = &sName[nw];
                    if (n == p) {
                        ret = PFPATH_cmpNameImpl(sName, sPattern, p_is_end);
                        if (ret == 0) {
                            return 0;
                        }
                        if (*p_is_end != 0) {
                            return ret;
                        }
                    }
                    nw = pf_vol_set.codeset.oem_char_width((s8*)sName);
                }
                if ((*sName == 0) || (*sPattern == 0)) {
                    *p_is_end = 1;
                }
            }
            default: {
                if (n != p) {
                    return 1;
                }
                break;
            }
        }

        sName = &sName[nw];
    }
    return *sName != 0 ? 1 : 0;
}

s32 PFPATH_CmpLongName(const u16* p_name, PF_STR* sPattern) {
    if (PFPATH_MatchFileNameWithPattern((s8*)p_name, sPattern, 1) != 0) {
        return 0;
    }

    return 1;
}

s32 PFPATH_CmpShortName(const s8* sShort, PF_STR* p_pattern, u32 is_short_search) {
    u32 is_end;
    s8 tmpBuf[13];
    s8* p_tmpBuf;
    const s8* sPattern;

    is_end = 0;
    p_tmpBuf = tmpBuf;
    sPattern = PFSTR_GetStrPos(p_pattern, 3);
    pf_strcpy(tmpBuf, sShort);
    if (((pf_vol_set.setting & 0x02) == 0x02) && (PFSTR_GetCodeMode(p_pattern) == 2) && (pf_strcmp(tmpBuf, (s8*)".") != 0) &&
        (pf_strcmp(tmpBuf, (s8*)"..") != 0) && (PFPATH_CheckExtShortName(p_pattern, 3, 1) == 0) && (is_short_search == 0)) {
        return 1;
    }
    if (pf_strcmp(sPattern, (s8*)"*.") == 0) {
        for (; *p_tmpBuf != 0 && *p_tmpBuf != '.'; p_tmpBuf++) {
        }
        if (*p_tmpBuf == 0) {
            *p_tmpBuf = '.';
            p_tmpBuf++;
            *p_tmpBuf = 0;
        }
    } else {
        if (*sPattern == 0) {
            return 1;
        }
        if (*tmpBuf == 0) {
            return 1;
        }
    }
    return PFPATH_cmpNameImpl(tmpBuf, sPattern, &is_end);
}

s32 VFiPFPATH_cmpTailSFN(const s8* sfn_name, const s8* pattern) {
    if (pf_strcmp(sfn_name, pattern) != 0) {
        return 1;
    }
    return 0;
}

void VFiPFPATH_InitTokenOfPath(PF_STR* p_str, const s8* path, u32 code_mode) {
    p_str->p_head = path;
    p_str->p_tail = path;
    p_str->code_mode = code_mode;
}

s32 VFiPFPATH_GetNextTokenOfPath(PF_STR* p_str, u32 wildcard) {
    u32 extsfn_len;
    u32 code_mode;

    extsfn_len = 0;
    p_str->p_head = p_str->p_tail;
    if (PF_IS_PATH_NULL(p_str, 1, 0)) {
        p_str->p_head = p_str->p_tail = NULL;
        return 0;
    }
    if (PF_IS_PATH_SEPERATOR(p_str, 1, 0)) {
        PFSTR_MoveStrPos(p_str, 1);
    }
    if (PF_IS_PATH_SEPERATOR(p_str, 1, 0)) {
        return 2;
    }
    code_mode = PFSTR_GetCodeMode(p_str);
    p_str->p_tail = p_str->p_head;
    if ((pf_vol_set.setting & 0x02) == 0x02) {
        extsfn_len = PFPATH_CheckExtShortName(p_str, 2, wildcard);
        if (extsfn_len != 0) {
            if (code_mode == 1) {
                p_str->p_tail += extsfn_len;
            } else {
                p_str->p_tail += extsfn_len * sizeof(u16);
            }
        }
    }
    if (extsfn_len == 0) {
        while (PF_IS_PATH_NOT_NULL(p_str, 2, 0)) {
            if (code_mode == 1 && pf_vol_set.codeset.is_oem_mb_char(*p_str->p_tail, 1) != 0) {
                p_str->p_tail++;
                if (pf_vol_set.codeset.is_oem_mb_char(*p_str->p_tail, 2) == 0 || p_str->p_tail[0] == 0) {
                    return 2;
                }
            } else {
                if (PF_IS_PATH_SEPERATOR(p_str, 2, 0)) {
                    break;
                }
                if ((pf_vol_set.config & 0x10000) == 0) {
                    if ((code_mode == 1 && !VALID_PATH_CHAR(*p_str->p_tail, 0x01)) ||
                        (code_mode == 2 && VALID_PATH_WCHAR(PF_CODE_C_TO_WC_ARR(p_str->p_tail, 0), 0x01) == 0)) {
                        if (wildcard == 0 ||
                            !(PFSTR_StrNCmp(p_str, (s8*)"*", 2, 0, 1) == 0 || PFSTR_StrNCmp(p_str, (s8*)"?", 2, 0, 1) == 0)) {
                            return 2;
                        }
                    }
                }
            }
            code_mode == 1 ? p_str->p_tail++ : (p_str->p_tail += sizeof(u16));
        }
    }
    return 0;
}

s32 PFPATH_SplitPath(PF_STR* p_path, PF_STR* p_dir_path, PF_STR* p_filename) {
    return PFPATH_DoSplitPath(p_path, p_dir_path, p_filename, 0);
}

s32 PFPATH_SplitPathWildcard(PF_STR* p_path, PF_STR* p_dir_path, PF_STR* p_pattern) {
    return PFPATH_DoSplitPath(p_path, p_dir_path, p_pattern, 1);
}

struct PF_VOLUME* PFPATH_GetVolumeFromPath(struct PF_STR* p_path) {
    s8 drv_char[2];
    if (PFSTR_StrNumChar(p_path, 1) >= 2 && PFSTR_StrNCmp(p_path, (const s8*)":", 1, 1, 1) == 0) {
        PFSTR_ToUpperNStr(p_path, 1, drv_char);
        return PFVOL_GetVolumeFromDrvChar(drv_char[0]);
    }
    return PFVOL_GetCurrentVolume();
}

u32 PFPATH_MatchFileNameWithPattern(const s8* file_name, PF_STR* p_pattern, u32 is_long_name) {
    u16 c_name;
    u16 c_pat;
    PF_FILE_NAME_ITER name;
    PF_STR pattern;
    u32 is_match = 1;

    name.buf = file_name;
    name.dot_inserted = 0;
    name.is_long_name = is_long_name;
    name.index = 0;

    pattern = *p_pattern;

    if (PFSTR_GetCodeMode(p_pattern) == 1) {
        if (is_long_name == 0 && (pf_vol_set.setting & 0x02) == 0x02 && PFPATH_CheckExtShortNameSignature(&pattern) == 1) {
            is_match = PFPATH_CheckExtShortNameSignature(&pattern);
            if (is_match == 1) {
                name.index += 2;
                pattern.p_head += 2;
            }
        }
    } else if ((pf_vol_set.setting & 0x02) == 0x02 && is_long_name == 0 && PFSTR_StrNCmp(p_pattern, (s8*)".", 1, 0, 1) != 0 &&
               PFSTR_StrNCmp(p_pattern, (s8*)"..", 1, 0, 2) != 0 && PFPATH_CheckExtShortName(p_pattern, 1, 0) == 0) {
        is_match = 0;
    }
    if (is_match == 1) {
        c_name = PFPATH_GetNextCharOfFileName(&name);
        c_pat = PFPATH_GetNextCharOfPattern(&pattern, is_long_name);
        is_match = PFPATH_DoMatchFileNameWithPattern(c_name, &name, c_pat, &pattern, is_long_name);
    }
    return is_match;
}

s32 VFiPFPATH_putShortName(u8* pDirEntry, const s8* short_name, u8 attr) {
    s32 i = 0;

    for (i = 0; (i < 8) && *short_name != 0 && *short_name != '.'; i++) {
        *pDirEntry++ = *short_name++;
    }
    if (i == 0) {
        for (i = 0; i < 2 && *short_name != 0; i++) {
            *pDirEntry++ = *short_name++;
        }
    }
    if ((attr & 0x08) == 0) {
        for (; i < 8; i++) {
            *pDirEntry++ = ' ';
        }
    }
    if (*short_name != 0) {
        if ((attr & 0x08) == 0) {
            short_name++;
        }
        for (; *short_name != 0; i++) {
            *pDirEntry++ = *short_name++;
        }
    }
    for (; i < 11; i++) {
        *pDirEntry++ = ' ';
    }
    return 0;
}

s32 VFiPFPATH_getShortName(s8* short_name, const u8* pDirEntry, u8 attr) {
    s32 i = 0;
    s32 nLen = -1;

    for (i = 0; i < 8; i++) {
        if ((short_name[i] = pDirEntry[i]) != ' ') {
            nLen = i;
        }
    }
    short_name += ++nLen;
    nLen = 7;
    for (i = 8; i < 11; i++) {
        if (pDirEntry[i] != ' ') {
            nLen = i;
        }
    }
    if (nLen > 7) {
        if ((attr & 0x08) == 0) {
            *short_name = '.';
            short_name++;
        }
        for (i = 8; i <= nLen; i++) {
            *short_name++ = pDirEntry[i];
        }
    }
    *short_name = 0;
    return 0;
}

void VFiPFPATH_getLongNameformShortName(const s8* short_name, s8* long_name, u8 flag) {
    s32 i = 0;
    s32 j = 0;

    for (i = 0; i < 8; i++) {
        if (short_name[i] == 0 || short_name[i] == '.') {
            break;
        }
        if ((flag & 0x08) != 0 && short_name[i] >= 'A' && short_name[i] <= 'Z') {
            long_name[i] = short_name[i] + ' ';
        } else {
            long_name[i] = short_name[i];
        }
    }
    if (short_name[i] == '.') {
        long_name[i++] = '.';
    }

    for (j = i + 3; i < j; i++) {
        if (short_name[i] == 0) {
            break;
        }
        if ((flag & 0x10) != 0 && short_name[i] >= 'A' && short_name[i] <= 'Z') {
            long_name[i] = short_name[i] + ' ';
        } else {
            long_name[i] = short_name[i];
        }
    }
    long_name[i] = 0;
}

u32 VFiPFPATH_GetLengthFromShortname(const s8* sSrc) {
    s32 i = 0;
    u32 szStr = 0;

    for (; sSrc[i] != 0; i++) {
        if (i == 8 && (*(sSrc + i) != ' ' || *(sSrc + i + 1) != ' ' || *(sSrc + i + 2) != ' ')) {
            szStr++;
        }
        if (sSrc[i] != ' ') {
            szStr++;
        }
    }
    return szStr;
}

u32 VFiPFPATH_GetLengthFromUnicode(const u16* sSrc) {
    s32 i = 0;
    s32 width;
    u32 szStr = 0;
    s16 oem_width;
    s16 uni_width;
    s8 Dest[2];

    while (sSrc[i] != 0) {
        width = pf_vol_set.codeset.unicode2oem(&sSrc[i], Dest);
        PFCODE_Divide_Width(width, &oem_width, &uni_width);
        szStr += oem_width;
        i += uni_width >> 1;
    }
    return szStr;
}

s32 VFiPFPATH_transformFromUnicodeToNormal(s8* sDest, const u16* sSrc) {
    s32 i;
    s32 width;
    u32 szStr = 0;  
    s16 oem_width;
    s16 uni_width;
    u16 dot_buf[2];
    u16 space_buf[2];

    if ((pf_vol_set.setting & 0x02) == 0x02) {
        dot_buf[0] = '.';
        dot_buf[1] = 0;
        space_buf[0] = ' ';
        space_buf[1] = 0;
        i = 0;
        while (sSrc[i] != 0) {
            if (pf_w_strncmp(&sSrc[i], dot_buf, 1) == 0) {
                *sDest = '.';
            } else if (pf_w_strncmp(&sSrc[i], space_buf, 1) == 0) {
                *sDest = ' ';
            } else {
                *sDest = '_';
            }
            sDest++;
            i++;
        }
    } else {
        i = 0;
        while (sSrc[i] != 0) {
            width = pf_vol_set.codeset.unicode2oem(&sSrc[i], sDest);
            PFCODE_Divide_Width(width, &oem_width, &uni_width);
            sDest = &sDest[oem_width];
            i += uni_width >> 1;
        }
    }
    *sDest = 0;
    return szStr;
}

s32 VFiPFPATH_transformInUnicode(u16* sDestStr, const s8* sSrcStr) {
    s32 i;
    s32 width;
    s32 szStr;
    s16 oem_width;
    s16 uni_width;

    width = 1;
    szStr = 0;
    i = 0;
    while (sSrcStr[i] != 0) {
        width = pf_vol_set.codeset.oem2unicode(&sSrcStr[i], sDestStr);
        PFCODE_Divide_Width(width, &oem_width, &uni_width);
        sDestStr += uni_width >> 1;
        szStr++;
        i += oem_width;
    }
    *sDestStr++ = 0;
    i++;
    i *= 2;
    return szStr;
}

u32 VFiPFPATH_parseShortName(s8* pDest, PF_STR* p_pattern) {
    s32 width;
    const s8* p_cur_src;
    u32 is_create_long = 0;
    u32 is_create_tail = 0;
    u16* p_name_cnt;
    u16 num_base;
    u16 num_ext;
    u16 last_width;
    u16 prev_last_width;
    u16 src_dot;
    u16 prev_src_dot;
    u16 src_pos;
    u16 dst_pos;
    u16 ext_pos;
    u16 move_cnt;
    s16 i;
    u16 wchar;
    u16 t_wchar;

    p_cur_src = PFSTR_GetStrPos(p_pattern, 3);
    if ((pf_vol_set.setting & 0x02) == 0) {
        while (*p_cur_src == ' ' || *p_cur_src == '.') {
            p_cur_src++;
            is_create_tail = 1;
        }
        src_dot = 0;
        prev_src_dot = 0;
        for (src_pos = 0; p_cur_src[src_pos] != '\0'; src_pos++) {
            if (p_cur_src[src_pos] == '.') {
                if (src_dot != 0) {
                    prev_src_dot = src_dot;
                }
                src_dot = src_pos;
            }
        }
        if (src_dot != 0 && p_cur_src[src_dot + 1] == 0) {
            if (prev_src_dot != 0) {
                src_dot = prev_src_dot;
            }
            is_create_tail = 1;
        }
        dst_pos = 0;
        src_pos = 0;
        num_base = 0;
        num_ext = 0;
        p_name_cnt = &num_base;
        last_width = 1;
        prev_last_width = 1;
        for (; num_ext < 3 && p_cur_src[src_pos] != 0; src_pos++) {
            if ((num_base != 8 && (src_dot == 0 || src_pos != src_dot)) || p_name_cnt == &num_ext) {
                if (p_cur_src[src_pos] != ' ' && p_cur_src[src_pos] != '.') {
                    width = pf_vol_set.codeset.oem_char_width((s8*)&p_cur_src[src_pos]);
                    if (width != 1) {
                        if ((src_pos < src_dot || src_dot == 0) && (num_base + width) > 8) {
                            is_create_tail = 1;
                        } else {
                            if (src_dot != 0 && src_pos > src_dot && (num_ext + width) > 3) {
                                is_create_tail = 1;
                                break;
                            }
                            if (p_name_cnt == &num_base) {
                                prev_last_width = last_width;
                                last_width = width;
                            }
                            for (; width != 0; dst_pos += 2, src_pos += 2, width -= 2) {
                                wchar = ((u8)p_cur_src[src_pos] << 8) + (u8)p_cur_src[src_pos + 1];
                                if (PFPATH_OEM_ConvertFWchar(&p_cur_src[src_pos], &t_wchar) != 0) {
                                    wchar = t_wchar;
                                    is_create_long = 1;
                                }
                                pDest[dst_pos] = (u8)(wchar >> 8);
                                pDest[dst_pos + 1] = (u8)wchar;
                                *p_name_cnt = *p_name_cnt + 2;
                            }
                            src_pos--;
                        }
                    } else {
                        if (p_name_cnt == &num_base) {
                            prev_last_width = last_width;
                            last_width = 1;
                        }
                        if ((pf_vol_set.config & 0x10000) != 0 || VALID_PATH_CHAR(p_cur_src[src_pos], 0x02)) {
                            pDest[dst_pos++] = pf_toupper(p_cur_src[src_pos]);
                            (*p_name_cnt)++;
                            if (p_cur_src[src_pos] >= 'a' && p_cur_src[src_pos] <= 'z') {
                                is_create_long = 1;
                            }
                        } else {
                            pDest[dst_pos++] = '_';
                            (*p_name_cnt)++;
                            is_create_tail = 1;
                        }
                    }
                } else {
                    is_create_tail = 1;
                }
            } else {
                p_name_cnt = &num_ext;
                if (p_cur_src[src_pos] != 0 && p_cur_src[src_pos] != '.') {
                    is_create_tail = 1;
                }
                if (src_dot != 0) {
                    pDest[dst_pos++] = '.';
                    src_pos = src_dot;
                } else {
                    break;
                }
            }
        }
        if (num_ext == 3 && p_cur_src[src_pos] != 0) {
            is_create_tail = 1;
        }
        pDest[dst_pos] = 0;
        if (is_create_tail != 0) {
            dst_pos = num_base;
            if (dst_pos == 8) {
                if (last_width != 1) {
                    dst_pos = 8 - last_width;
                } else {
                    dst_pos = 8 - ((prev_last_width == 1) ? 2 : (prev_last_width + 1));
                }
            } else if (dst_pos == 7) {
                dst_pos -= prev_last_width;
            }
            ext_pos = dst_pos + 2;
            if (ext_pos < num_base) {
                move_cnt = num_base - ext_pos;
                for (i = -1; i < (num_ext + 1); i++) {
                    pDest[num_base + i] = pDest[num_base + move_cnt + i];
                }
            } else if (ext_pos > num_base) {
                move_cnt = ext_pos - num_base;
                for (i = num_ext + 1; i >= 0; i--) {
                    pDest[num_base + move_cnt + i] = pDest[num_base + i];
                }
            }
            if (num_base != 0) {
                if ((pf_vol_set.setting & 0x02) != 0x02) {
                    pDest[dst_pos++] = '~';
                    pDest[dst_pos++] = '1';
                } else {
                    pDest[dst_pos++] = '_';
                    pDest[dst_pos++] = '_';
                }
            }
            is_create_long = 1;
        }
        if (PFSTR_GetCodeMode(p_pattern) == 2) {
            is_create_long = 1;
        }
    } else {
        for (; *p_cur_src != 0 && !is_create_tail; p_cur_src++) {
            if (*p_cur_src != '.' && *p_cur_src != ' ') {
                is_create_tail = 1;
            }
        }
        if (is_create_tail) {
            pDest[0] = 1;
            pDest[1] = 2;
            pDest[2] = '0';
            pDest[3] = '0';
            pDest[4] = '0';
            pDest[5] = '0';
            pDest[6] = '0';
            pDest[7] = '0';
            pDest[8] = 0;
        } else {
            *pDest = 0;
        }
        is_create_long = 1;
    }
    return is_create_long;
}

s32 VFiPFPATH_parseShortNameNumeric(s8* p_char, u32 count) {
    u32 numeric_cnt;
    u32 pos_tail;
    u32 pos_dot;
    u32 pos_ext;
    u32 pos_slide;
    u32 pos_end;
    s8 numeric[6];

    if (count == 0) {
        return 0;
    }
    for (pos_tail = 1; p_char[pos_tail] != '~'; pos_tail++) {
    }
    for (pos_dot = pos_tail + 1; p_char[pos_dot] != '.' && p_char[pos_dot] != 0; pos_dot++) {
    }
    pos_ext = pos_dot + 1;
    if (p_char[pos_dot] == '.' && p_char[pos_ext] != 0) {
        for (pos_end = pos_ext + 1; p_char[pos_end] != 0; pos_end++) {
        }
    } else {
        pos_end = pos_ext;
    }
    for (numeric_cnt = 0; count != 0; numeric_cnt++) {
        numeric[numeric_cnt] = (count % 10) + '0';
        count /= 10;
    }
    if ((pos_tail + numeric_cnt) >= pos_ext) {
        pos_slide = pos_tail + numeric_cnt + 1;
        if (pos_slide > 8) {
            pos_slide = 8;
        }
        p_char[pos_slide + 4] = p_char[pos_end];
        p_char[pos_slide + 3] = p_char[pos_ext + 2];
        p_char[pos_slide + 2] = p_char[pos_ext + 1];
        p_char[pos_slide + 1] = p_char[pos_ext];
        p_char[pos_slide] = p_char[pos_dot];
    }
    if ((pos_tail + numeric_cnt) >= 8U) {
        pos_tail = 7 - numeric_cnt;
    }
    p_char[pos_tail++] = '~';
    for (; numeric_cnt != 0; numeric_cnt--, pos_tail++) {
        p_char[pos_tail] = numeric[numeric_cnt - 1];
    }
    return 0;
}

void PFPATH_CopyPattern(s8* p_buf_local, u16* p_buf_unicode, PF_STR* p_pattern) {
    u16 wc[2];

    if (PFSTR_GetCodeMode(p_pattern) == 1) {
        if (pf_strcmp(p_pattern->p_head, (s8*)"*.*") == 0) {
            pf_strcpy(p_buf_local, (s8*)"*");
            return;
        }
        pf_strcpy(p_buf_local, p_pattern->p_head);
        return;
    }
    if (PFSTR_StrCmp(p_pattern, (s8*)"*.*") == 0) {
        wc[0] = '*';
        wc[1] = 0;
        pf_w_strcpy(p_buf_unicode, wc);
    } else {
        pf_w_strcpy(p_buf_unicode, (u16*)p_pattern->p_head);
    }
    if ((pf_vol_set.setting & 0x02) == 0x02) {
        pf_vol_set.setting &= ~(0x01 | 0x02);
        pf_vol_set.setting |= 0x01;
        VFiPFPATH_transformFromUnicodeToNormal(p_buf_local, p_buf_unicode);
        pf_vol_set.setting &= ~(0x01 | 0x02);
        pf_vol_set.setting |= 0x02;
        return;
    }
    VFiPFPATH_transformFromUnicodeToNormal(p_buf_local, p_buf_unicode);
}

u32 PFPATH_CheckExtShortNameSignature(PF_STR* p_str) {
    u32 result = 0;
    s8 sig[2] = {1, 2};

    if (PFSTR_StrNCmp(p_str, sig, 1, 0, 2) == 0) {
        result = 1;
    }
    return result;
}

u32 PFPATH_CheckExtShortName(PF_STR* p_str, u32 target, u32 wildcard) {
    u32 result = 0;
    s16 i;
    s16 num;
    u32 is_wildcard = 0;
    s8 sig[2] = {1, 2};
    const s8* p_c;
    u16* p_wc;

    if (PFSTR_StrNCmp(p_str, sig, target, 0, 2) == 0 ||
        (PFSTR_StrNCmp(p_str, (s8*)"?", target, 0, 1) == 0 &&
         (PFSTR_StrNCmp(p_str, (s8*)"?", target, 1, 1) == 0 || PFSTR_StrNCmp(p_str, (s8*)"*", target, 0, 1) == 0)) ||
        PFSTR_StrNCmp(p_str, (s8*)"*", target, 0, 1) == 0) {
        for (i = 2; i < 8 && PF_IS_PATH_SEPERATOR(p_str, target, i) == 0 && PFSTR_StrNCmp(p_str, (s8*)" ", target, i, 1) != 0 &&
                    PF_IS_PATH_NOT_NULL(p_str, target, i);
             i++) {
            if (PFSTR_GetCodeMode(p_str) == 1) {
                p_c = PFSTR_GetStrPos(p_str, target);
                num = p_c[i] - '0';
            } else {
                p_wc = (u16*)PFSTR_GetStrPos(p_str, target);
                num = p_wc[i] - '0';
            }
            if (num < 0 || num > 9) {
                if (!wildcard) {
                    break;
                }
                if (PFSTR_StrNCmp(p_str, (s8*)"*", target, i, 1) == 0 || PFSTR_StrNCmp(p_str, (s8*)"?", target, i, 1) == 0) {
                    if (wildcard == 1 && PFSTR_StrNCmp(p_str, (s8*)"*", target, i, 1) == 0) {
                        is_wildcard = 1;
                    }
                } else {
                    break;
                }
            }
        }
        if (i == 8 || is_wildcard == 1) {
            if (PFSTR_StrNCmp(p_str, (s8*)" ", target, i, 1) == 0 || PF_IS_PATH_SEPERATOR(p_str, target, i) == 0 ||
                PF_IS_PATH_NULL(p_str, target, i)) {
                result = i;
            }
        }
    }
    return result;
}

u32 PFPATH_ParseEntryIndex(PF_STR* p_str, u32* p_index) {
    u32 result = 0;
    s16 i;
    s16 num;
    u32 index;
    s8 sig[2] = {1, 2};
    const s8* p_c;
    u16* p_wc;

    if (PFSTR_StrNCmp(p_str, sig, 1, 0, 2) == 0) {
        index = 0;

        for (i = 2; i < 8 || PF_IS_PATH_NOT_NULL(p_str, 1, i); i++) {
            if (PFSTR_GetCodeMode(p_str) == 1) {
                p_c = PFSTR_GetStrPos(p_str, 1);
                num = p_c[i] - '0';
            } else {
                p_wc = (u16*)PFSTR_GetStrPos(p_str, 1);
                num = p_wc[i] - '0';
            }
            if ((num >= 0) && (num <= 9)) {
                index *= 10;
                index += num;
            } else {
                break;
            }
        }
        if (i == 8) {
            if (PF_IS_PATH_SEPERATOR(p_str, 1, i) == 0 || PFSTR_StrNCmp(p_str, NULL, 1, i, 1) == 0) {
                *p_index = index;
                result = 1;
            }
        }
    }
    return result;
}

s32 PFPATH_AdjustExtShortName(s8* pName, u32 position) {
    u32 num;
    u32 i;
    u32 div1;
    u32 div2;

    for (i = 7, div1 = 10, div2 = 1; i > 1 && position != 0; i--, div1 *= 10, div2 *= 10) {
        num = position % div1;
        if (num != 0) {
            position -= num;
            num = (num / div2);
            *(pName + i) += (s8)num;
        }
    }
    return 0;
}
