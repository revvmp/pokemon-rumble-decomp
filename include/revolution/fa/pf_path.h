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
s32 VFiPFPATH_transformFromUnicodeToNormal(s8* sDest, const u16* sSrc);
s32 VFiPFPATH_transformInUnicode(u16* sDestStr, const s8* sSrcStr);

#ifdef __cplusplus
}
#endif

#endif
