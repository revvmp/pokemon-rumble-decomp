#ifndef FA_PF_VOLUME_H
#define FA_PF_VOLUME_H

#include <revolution/fa/pdm.h>
#include <revolution/fa/pf_cache.h>
#include <revolution/fa/pf_str.h>
#include <revolution/os/OSThread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PF_FSI_FLAG_FREE_COUNT_VALID 0x4

#define PF_ATTR_READONLY 0x01
#define PF_ATTR_HIDDEN 0x02
#define PF_ATTR_SYSTEM 0x04
#define PF_ATTR_VOLUME 0x08
#define PF_ATTR_DIR 0x10
#define PF_ATTR_ARCHIVE 0x20
#define PF_ATTR_LFN 0x0F

#define PF_ENT_DELETED 0xE5
#define PF_ENT_END 0x00

struct PF_VOLUME;

typedef struct PF_FAT_HINT {
    u32 chain_index;
    u32 cluster;
    u32 chain_stamp;
} PF_FAT_HINT;

typedef struct PF_LAST_ACCESS {
    u32 chain_index;
    u32 cluster;
} PF_LAST_ACCESS;

typedef struct PF_LAST_CLUSTER {
    u32 num_last_cluster;
    u32 max_chain_index;
} PF_LAST_CLUSTER;

typedef struct PF_CLUSTER_LINK {
    u32* buffer;
    u16 interval;
    u16 interval_offset;
    u32 position;
    u32 max_count;
    u32 save_index;
} PF_CLUSTER_LINK;

typedef struct PF_FFD {
    u32 chain_stamp;
    u32 start_cluster;
    u32* p_start_cluster;
    PF_LAST_CLUSTER last_cluster;
    PF_LAST_ACCESS last_access;
    PF_CLUSTER_LINK cluster_link;
    PF_FAT_HINT* p_hint;
    struct PF_VOLUME* p_vol;
} PF_FFD;

typedef struct PF_DIR_ENT {
    u16 long_name[261];
    u8 num_entry_LFNs;
    u8 ordinal;
    u8 check_sum;
    u8 align_pad[1];
    s8 short_name[13];
    u8 small_letter_flag;
    u8 attr;
    u8 create_time_ms;
    u16 create_time;
    u16 create_date;
    u16 access_date;
    u16 modify_time;
    u16 modify_date;
    u32 file_size;
    struct PF_VOLUME* p_vol;
    u32 path_len;
    u32 start_cluster;
    u32 entry_sector;
    u16 entry_offset;
} PF_DIR_ENT;

#define PF_SFD_STAT_ALLOCATED 0x1
#define PF_SFD_STAT_OPENED 0x2

typedef struct PF_LOCK {
    u16 mode;
    u16 count;
    u32 wcount;
    struct PF_FILE* owner;
    OSSemaphore* resource;
    OSSemaphore semaphore;
} PF_LOCK;

typedef struct PF_SFD {
    u32 stat;
    PF_FFD ffd;
    PF_DIR_ENT dir_entry;
    PF_LOCK lock;
    u16 num_handlers;
} PF_SFD;

typedef struct PF_CURSOR {
    u32 position;
    u32 sector;
    u32 file_sector_index;
    u16 offset_in_sector;
} PF_CURSOR;

typedef struct PF_FILE {
    u32 stat;
    u32 open_mode;
    PF_SFD* p_sfd;
    PF_FAT_HINT hint;
    s32 last_error;
    PF_CURSOR cursor;
    u16 lock_count;
} PF_FILE;

#define PF_SDD_STAT_ALLOCATED 0x1
#define PF_SDD_STAT_OPENED 0x2

typedef struct PF_SDD {
    u32 stat;
    u16 num_handlers;
    PF_FFD ffd;
    PF_DIR_ENT dir_entry;
} PF_SDD;

typedef struct PF_DIR_CURSOR {
    u32 physical_entry_index;
    u32 logical_entry_index;
    u32 logical_seek_index;
} PF_DIR_CURSOR;

#define PF_DIR_STAT_ALLOCATED 0x1

typedef struct PF_DIR {
    u32 stat;
    PF_SDD* p_sdd;
    PF_FAT_HINT hint;
    PF_DIR_CURSOR cursor;
} PF_DIR;

#define PF_CUR_DIR_STAT_ALLOCATED 0x1

typedef struct PF_CUR_DIR {
    u32 stat;
    s32 context_id;
    PF_DIR_ENT dir_entry;
} PF_CUR_DIR;

#define PF_MAX_FILES 5
#define PF_MAX_CUR_DIRS 4
#define PF_MAX_DIRS 3

typedef struct PF_VOLUME {
    PDM_BPB bpb;
    u32 num_free_clusters;
    u32 last_free_cluster;
    PF_SFD sfds[PF_MAX_FILES];
    PF_FILE ufds[PF_MAX_FILES];
    PF_SDD sdds[PF_MAX_DIRS];
    PF_DIR udds[PF_MAX_DIRS];
    s32 num_opened_files;
    s32 num_opened_directories;
    PF_SECTOR_CACHE cache;
    s8 label[12];
    PF_CUR_DIR current_dir[PF_MAX_CUR_DIRS];
    u8 tail_entry[0xC];
    s32 last_error;
    s32 last_driver_error;
    u32 file_config;
    u16 flags;
    s8 drv_char;
    u16 fsi_flag;
    u8 tail[0x1FC6];
} PF_VOLUME;

typedef struct PF_ENT_ITER {
    u32 index;
    PF_VOLUME* p_vol;
    PF_FFD ffd;
    u32 file_sector_index;
    u32 sector;
    u16 offset;
    u16 offset_mask;
    u8 buf[32];
    u8 log2_entries_per_sector;
} PF_ENT_ITER;

s32 PFVOL_CheckForRead(PF_VOLUME* p_vol);
s32 PFVOL_CheckForWrite(PF_VOLUME* p_vol);
s32 PFVOL_IsCurrentDirCluster(PF_VOLUME* p_vol, u32 cluster);
s32 PFVOL_SetCurrentDir(PF_VOLUME* p_vol, PF_DIR_ENT* p_ent);
s32 PFVOL_GetCurrentDir(PF_VOLUME* p_vol, PF_DIR_ENT* p_current_dir);
void PFVOL_SetCurrentVolume(PF_VOLUME* p_vol);
PF_VOLUME* PFVOL_GetVolumeFromDrvChar(s8 drv_char);

u32 PFDRV_IsInserted(PF_VOLUME* p_vol);
s32 PFDRV_lread(PF_VOLUME* p_vol, u8* buf, u32 sector, u32 num_sector, u32* p_num_success);
s32 PFDRV_lwrite(PF_VOLUME* p_vol, const u8* buf, u32 sector, u32 num_sector, u32* p_num_success);

typedef struct PF_DEV_INF PF_DEV_INF;
typedef struct PF_VOL_INF PF_VOL_INF;
typedef struct PF_VOLUME_CONFIG PF_VOLUME_CONFIG;
typedef struct PF_CLUSTER_LINK_CONFIG PF_CLUSTER_LINK_CONFIG;

s32 PFVOL_format(s8 drive, u8* p_parameter);
s32 PFVOL_getdev(s8 drive, PF_DEV_INF* p_info);
s32 PFVOL_getvol(s8 drive, PF_VOL_INF* p_info);
s32 PFVOL_getvolcfg(s8 drive, PF_VOLUME_CONFIG* p_config);
s32 PFVOL_iswriteprotected(s8 drive, u32* p_is_write_protected);
s32 PFVOL_buffering(s8 drive, u8 mode);
s32 PFVOL_derrnum(s8 drive);
s32 PFVOL_detach(s8 drive, u32 mode);
s32 PFVOL_errnum(void);

typedef struct PF_TAIL_BUF PF_TAIL_BUF;
s32 PFVOL_mount(s8 drive);
s32 PFVOL_regctx(void);
s32 PFVOL_rmvvol(s8 drive);
s32 PFVOL_setclstlink(s8 drive, u32 mode, PF_CLUSTER_LINK_CONFIG* p_info);
s32 PFVOL_setcode(PF_CHARCODE* p_codeset);
s32 PFVOL_setencode(u32 mode);
s32 PFVOL_settailbuf(s8 drive, u32 size, PF_TAIL_BUF* p_buffer);
s32 PFVOL_setupfsi(s8 drive, u16 flags);
s32 PFVOL_setvol(s8 drive, PF_STR* p_name);
s32 PFVOL_setvolcfg(s8 drive, PF_VOLUME_CONFIG* p_config);
s32 PFVOL_sync(s8 drive, u32 mode);
s32 PFVOL_unmount(s8 drive, u32 mode);
s32 PFVOL_unregctx(void);

struct PF_DRV_TBL;
s32 PFVOL_attach(struct PF_DRV_TBL* p_driver, const char* path, u32 size);

#ifdef __cplusplus
}
#endif

#endif
