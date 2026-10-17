#ifndef FA_PF_FILE_H
#define FA_PF_FILE_H

#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 PFFILE_OpenEntry(PF_DIR_ENT* p_ent, PF_ENT_ITER* p_iter, u32 mode, PF_FILE** pp_file);
u32 PFFILE_IsOpened(PF_DIR_ENT* p_ent);
void PFFILE_Cursor_MoveToEnd(PF_FILE* p_file);
void PFFILE_Cursor_MoveToClusterEnd(PF_FILE* p_file, u32 size);

#ifdef __cplusplus
}
#endif

#endif
