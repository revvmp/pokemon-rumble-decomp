#ifndef FA_PF_FILE_H
#define FA_PF_FILE_H

#include <revolution/fa/pf_volume.h>
#include <revolution/fa/pf_str.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 PFFILE_OpenEntry(PF_DIR_ENT* p_ent, PF_ENT_ITER* p_iter, u32 mode, PF_FILE** pp_file);
u32 PFFILE_IsOpened(PF_DIR_ENT* p_ent);
void PFFILE_Cursor_MoveToEnd(PF_FILE* p_file);
void PFFILE_Cursor_MoveToClusterEnd(PF_FILE* p_file, u32 size);

typedef struct PF_INFO PF_INFO;

s32 PFFILE_fadjust(PF_FILE* p_file);
s32 PFFILE_fappend(PF_FILE* p_file, u32 count, u32* p_count_appended);
s32 PFFILE_fclose(PF_FILE* p_file);
s32 PFFILE_combine(PF_STR* p_path, PF_STR* p_source);
s32 PFFILE_feof(PF_FILE* p_file);
s32 PFFILE_ferror(PF_FILE* p_file);
s32 PFFILE_finfo(PF_FILE* p_file, PF_INFO* p_info);
s32 PFFILE_flock(PF_FILE* p_file, u32 mode);
s32 PFFILE_fopen(PF_STR* p_path, u32 mode, PF_FILE** pp_file);
s32 PFFILE_fread(u8* p_buffer, u32 size, u32 count, PF_FILE* p_file, u32* p_count_read);
s32 PFFILE_fwrite(u8* p_buffer, u32 size, u32 count, PF_FILE* p_file, u32* p_count_written);
s32 PFFILE_fseek(PF_FILE* p_file, s32 offset, int origin);
s32 PFFILE_fsetclstlink(PF_FILE* p_file, u32 mode, PF_CLUSTER_LINK_CONFIG* p_config);
s32 PFFILE_fsync(PF_FILE* p_file);
s32 PFFILE_cinsert(PF_STR* p_path, u32 offset, u32 count, u32* p_count_inserted);
s32 PFFILE_cdelete(PF_STR* p_path, u32 offset, u32 count, u32* p_count_deleted);
s32 PFFILE_divide(PF_STR* p_path, PF_STR* p_new_path, u32 offset);

s32 PFFILE_remove(PF_STR* p_path);

#ifdef __cplusplus
}
#endif

#endif
