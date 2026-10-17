#ifndef FA_PF_VOLUME_H
#define FA_PF_VOLUME_H

#include <revolution/fa/pdm.h>
#include <revolution/fa/pf_cache.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PF_FSI_FLAG_FREE_COUNT_VALID 0x4

typedef struct PF_VOLUME {
    PDM_BPB bpb;
    u32 num_free_clusters;
    u8 files[0x15E8];
    PF_SECTOR_CACHE cache;
    s8 label[12];
    u8 state[0x93C];
    u16 fsi_flag;
    u8 tail[0x1FC6];
} PF_VOLUME;

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
    u32 reserved;
    u32* p_start_cluster;
    PF_LAST_CLUSTER last_cluster;
    PF_LAST_ACCESS last_access;
    PF_CLUSTER_LINK cluster_link;
    PF_FAT_HINT* p_hint;
    PF_VOLUME* p_vol;
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
    PF_VOLUME* p_vol;
    u32 path_len;
    u32 start_cluster;
    u32 entry_sector;
    u16 entry_offset;
} PF_DIR_ENT;

#define PF_SFD_STAT_ALLOCATED 0x1
#define PF_SFD_STAT_OPENED 0x2

typedef struct PF_SFD {
    u32 stat;
    PF_FFD ffd;
    PF_DIR_ENT dir_entry;
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
} PF_FILE;

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

s32 PFDRV_lread(PF_VOLUME* p_vol, u8* buf, u32 sector, u32 num_sector, u32* p_num_success);
s32 PFDRV_lwrite(PF_VOLUME* p_vol, const u8* buf, u32 sector, u32 num_sector, u32* p_num_success);

#ifdef __cplusplus
}
#endif

#endif
