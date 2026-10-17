#ifndef FA_PF_PATH_H
#define FA_PF_PATH_H

#include <revolution/fa/pf_str.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 PFPATH_SplitPath(PF_STR* p_path, PF_STR* p_dir_path, PF_STR* p_filename);
s32 PFPATH_SplitPathWildcard(PF_STR* p_path, PF_STR* p_dir_path, PF_STR* p_filename);
struct PF_VOLUME* PFPATH_GetVolumeFromPath(PF_STR* p_path);
void PFPATH_CopyPattern(s8* p_pattern, u16* p_w_pattern, PF_STR* p_filename);
u32 VFiPFPATH_parseShortName(s8* pDest, PF_STR* p_pattern);
s32 VFiPFPATH_cmpTailSFN(const s8* sfn_name, const s8* pattern);
s32 VFiPFPATH_parseShortNameNumeric(s8* p_char, u32 count);
s32 PFPATH_ParseEntryIndex(PF_STR* p_pattern, u32* p_index);
u32 PFPATH_CheckExtShortNameSignature(PF_STR* p_filename);
void VFiPFPATH_InitTokenOfPath(PF_STR* p_token, const s8* p_path, u32 code_mode);
s32 VFiPFPATH_GetNextTokenOfPath(PF_STR* p_token, u32 wildcard);
u32 PFPATH_MatchFileNameWithPattern(const s8* p_name, PF_STR* p_pattern, u32 is_long_name);
u32 VFiPFPATH_GetLengthFromShortname(const s8* p_short_name);
u32 VFiPFPATH_GetLengthFromUnicode(const u16* p_long_name);
s32 PFPATH_AdjustExtShortName(s8* pName, u32 position);
s32 VFiPFPATH_transformFromUnicodeToNormal(s8* sDest, const u16* sSrc);
s32 VFiPFPATH_transformInUnicode(u16* sDestStr, const s8* sSrcStr);
void VFiPFPATH_putShortName(u8* p_buf, const s8* p_short_name, u8 attr);
void VFiPFPATH_getShortName(s8* p_short_name, const u8* p_buf, u8 attr);
void VFiPFPATH_getLongNameformShortName(const s8* p_short_name, s8* p_long_name, u8 small_letter_flag);
s32 PFPATH_CmpLongName(const u16* p_name, PF_STR* p_pattern);
s32 PFPATH_CmpShortName(const s8* p_name, PF_STR* p_pattern, u32 is_wildcard);

#ifdef __cplusplus
}
#endif

#endif
