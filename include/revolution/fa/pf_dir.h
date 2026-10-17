#ifndef FA_PF_DIR_H
#define FA_PF_DIR_H

#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PF_DIRENT {
    s8 long_name[512];
    s8 short_name[13];
} PF_DIRENT;

typedef struct PF_STAT {
    u32 file_size;
    u16 access_date;
    u16 modify_time;
    u16 modify_date;
    u16 create_time;
    u16 create_date;
    u16 create_time_ms;
    u8 attr;
} PF_STAT;

#define PF_DTA_STAT_OEM_PATTERN 0x1
#define PF_DTA_STAT_UNICODE_PATTERN 0x2

typedef struct PF_DTA {
    PF_FILE* p_file;
    PF_DIR* p_dir;
    PF_VOLUME* p_vol;
    u32 start_cluster;
    u32 index;
    u32 status;
    u8 num_entry_LFNs;
    u8 ordinal;
    u8 check_sum;
    u8 attr_required;
    s8 pattern[520];
    u16 modify_time;
    u16 modify_date;
    u32 file_size;
    u8 attr;
    s8 short_name[13];
    s8 long_name[522];
    u16 w_pattern[260];
    u16 w_short_name[13];
    u16 w_long_name[261];
} PF_DTA;

s32 PFDIR_IsEmptyDirectory(PF_ENT_ITER* p_iter, u32* p_is_empty);
PF_SDD* PFDIR_GetSDD(PF_VOLUME* p_vol, PF_DIR_ENT* p_ent);
s32 PFDIR_OpenDirEntry(PF_DIR_ENT* p_ent, PF_ENT_ITER* p_iter, PF_DIR** pp_dir);
s32 PFDIR_p_opendir(PF_VOLUME* p_vol, PF_STR* p_path, PF_DIR** pp_dir);
s32 PFDIR_p_readdir(PF_DIR* p_dir, PF_DIRENT* p_dirent);
s32 PFDIR_p_chdir(PF_VOLUME* p_vol, PF_STR* p_path);
s32 PFDIR_p_chmod(PF_VOLUME* p_vol, PF_STR* p_path, u8 attr);
s32 PFDIR_p_rmdir(PF_VOLUME* p_vol, PF_STR* p_path);
s32 PFDIR_p_fstat(PF_VOLUME* p_vol, PF_STR* p_path, PF_STAT* p_stat);
s32 PFDIR_p_fsnext(PF_DTA* p_dta);
s32 PFDIR_p_fsfirst(PF_VOLUME* p_vol, PF_STR* p_path, u8 attr, PF_DTA* p_dta);
s32 PFDIR_ChmodEntry(PF_DTA* p_dta, PF_DIR_ENT* p_ent, u32 mode, u8 attr);
s32 PFDIR_RemoveEntry(PF_DTA* p_dta, PF_DIR_ENT* p_ent, PF_ENT_ITER* p_iter);
s32 PFDIR_OpenFileEntry(PF_DTA* p_dta, PF_DIR_ENT* p_ent, PF_ENT_ITER* p_iter, u32 mode, u8 param);
s32 PFDIR_OpenDirEntryForDTA(PF_DTA* p_dta, PF_DIR_ENT* p_ent, PF_ENT_ITER* p_iter, u32 mode);
s32 PFDIR_p_fsexec(PF_DTA* p_dta, u32 mode, u8 param);
void PFDIR_FinalizeAllDirs(PF_VOLUME* p_vol);
s32 PFDIR_fsfirst(PF_STR* p_path, u8 attr, PF_DTA* p_dta);
s32 PFDIR_fsnext(PF_DTA* p_dta);
s32 PFDIR_fsexec(PF_DTA* p_dta, u32 mode, u8 param);
s32 PFDIR_fstat(PF_STR* p_path, PF_STAT* p_stat);
s32 PFDIR_rename(PF_STR* p_old_path, PF_STR* p_new_path);
s32 PFDIR_move(PF_STR* p_src_path, PF_STR* p_dst_path);
s32 PFDIR_opendir(PF_STR* p_path, PF_DIR** pp_dir);
s32 PFDIR_closedir(PF_DIR* p_dir);
s32 PFDIR_readdir(PF_DIR* p_dir, PF_DIRENT* p_dirent);
s32 PFDIR_telldir(PF_DIR* p_dir, u32* p_index);
s32 PFDIR_seekdir(PF_DIR* p_dir, u32 index);
s32 PFDIR_rmdir(PF_STR* p_path);
s32 PFDIR_mkdir(PF_STR* p_path);
s32 PFDIR_createdir(PF_STR* p_path, u32 mode, PF_DTA* p_dta);
s32 PFDIR_chdir(PF_STR* p_path);
s32 PFDIR_fchdir(PF_DIR* p_dir);
s32 PFDIR_chmod(PF_STR* p_path, u8 attr);
s32 PFDIR_chdmod(PF_STR* p_path, u8 attr);

s32 PFDIR_fchdir(PF_DIR* p_dir);
s32 PFDIR_fsexec(PF_DTA* p_dta, u32 mode, u8 attributes);
s32 PFDIR_fsfirst(PF_STR* p_path, u8 attributes, PF_DTA* p_dta);
s32 PFDIR_fsnext(PF_DTA* p_dta);
s32 PFDIR_fstat(PF_STR* p_path, PF_STAT* p_stat);
s32 PFDIR_chdir(PF_STR* p_path);
s32 PFDIR_chdmod(PF_STR* p_path, u8 attributes);
s32 PFDIR_chmod(PF_STR* p_path, u8 attributes);
s32 PFDIR_closedir(PF_DIR* p_dir);
s32 PFDIR_createdir(PF_STR* p_path, s32 mode, PF_DTA* p_dta);

s32 PFDIR_mkdir(PF_STR* p_path);
s32 PFDIR_move(PF_STR* p_src, PF_STR* p_dst);
s32 PFDIR_opendir(PF_STR* p_path, PF_DIR** pp_dir);
s32 PFDIR_readdir(PF_DIR* p_dir, PF_DIRENT* p_entry);
s32 PFDIR_rename(PF_STR* p_src, PF_STR* p_dst);
s32 PFDIR_seekdir(PF_DIR* p_dir, u32 offset);
s32 PFDIR_rmdir(PF_STR* p_path);
s32 PFDIR_telldir(PF_DIR* p_dir, u32* p_offset);

#ifdef __cplusplus
}
#endif

#endif
