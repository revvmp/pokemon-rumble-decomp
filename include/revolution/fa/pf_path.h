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
