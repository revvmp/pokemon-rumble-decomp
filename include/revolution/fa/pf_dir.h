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
    u32 reserved[2];
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

#ifdef __cplusplus
}
#endif

#endif
