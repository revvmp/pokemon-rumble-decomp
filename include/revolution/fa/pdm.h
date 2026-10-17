#ifndef FA_PDM_H
#define FA_PDM_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PDM_OK 0
#define PDM_ERR_NOT_BOOT_SECTOR 2
#define PDM_ERR_NO_PARTITION 3
#define PDM_ERR_INVALID_BPB 4
#define PDM_ERR_PARTITION_NOT_FOUND 7
#define PDM_ERR_TOO_MANY_DISKS 8
#define PDM_ERR_TOO_MANY_PARTITIONS 0xA
#define PDM_ERR_INITIALIZED 0xB
#define PDM_ERR_MOUNTED 0xC
#define PDM_ERR_NOT_OPENED 0xD
#define PDM_ERR_NOT_MOUNTED 0xE
#define PDM_ERR_ACCESS 0xF
#define PDM_ERR_DISK_OPENED 0x10
#define PDM_ERR_DISK_MOUNTED 0x11
#define PDM_ERR_OUT_OF_RANGE 0x12
#define PDM_ERR_DRIVER 0x15

#define PDM_MAX_DISK 26
#define PDM_MAX_PARTITION 26

#define PDM_SWAP16(x) ((u16)((((x) >> 8) & 0xFF) | (((x) & 0xFF) << 8)))
#define PDM_SWAP32(x) ((((x) << 24) | (((x) << 8) & 0xFF0000)) | (((x) >> 24) | (((x) >> 8) & 0xFF00)))
#define PDM_LOAD_LE16(buf, off) (((u32)((buf) + (off)) & 1) ? (u16)((buf)[off] | ((buf)[(off) + 1] << 8)) : PDM_SWAP16(*(u16*)((buf) + (((off) + 1) & ~1))))
#define PDM_LOAD_LE32(p) PDM_SWAP32(*(u32*)(p))

#define PDM_DISK_STATUS_OPEN 0x1
#define PDM_DISK_STATUS_WRITE_PROTECT 0x2
#define PDM_DISK_STATUS_INSERTED 0x4

#define PDM_PART_STATUS_OPEN 0x1
#define PDM_PART_STATUS_MEDIA_CHANGED 0x2
#define PDM_PART_STATUS_INITIALIZED 0x4
#define PDM_PART_STATUS_MOUNTED 0x8

#define PDM_MEDIA_ATTR_WRITE_PROTECT 0x1

typedef enum PDM_FAT_TYPE {
    PDM_FAT_12 = 0,
    PDM_FAT_16 = 1,
    PDM_FAT_32 = 2
} PDM_FAT_TYPE;

typedef struct PDM_BPB {
    u16 bytes_per_sector;
    u16 num_reserved_sectors;
    u16 num_root_dir_entries;
    u8 sectors_per_cluster;
    u8 num_FATs;
    u32 total_sectors;
    u32 sectors_per_FAT;
    u32 root_dir_cluster;
    u16 fs_info_sector;
    u16 backup_boot_sector;
    u16 ext_flags;
    u8 media;
    PDM_FAT_TYPE fat_type;
    u8 log2_bytes_per_sector;
    u8 log2_sectors_per_cluster;
    u8 num_active_FATs;
    u16 num_root_dir_sectors;
    u32 active_FAT_sector;
    u32 first_root_dir_sector;
    u32 first_data_sector;
    u32 num_clusters;
} PDM_BPB;

typedef struct PDM_DISK_INFO {
    u32 total_sectors;
    u16 cylinders;
    u8 heads;
    u8 sectors_per_track;
    u16 bytes_per_sector;
    u32 media_attr;
    void* format_param;
} PDM_DISK_INFO;

struct PDM_DISK;
struct PDM_PARTITION;

typedef struct PDM_FUNCTBL {
    s32 (*init)(struct PDM_DISK* p_disk);
    s32 (*finalize)(struct PDM_DISK* p_disk);
    s32 (*mount)(struct PDM_DISK* p_disk);
    s32 (*unmount)(struct PDM_DISK* p_disk);
    s32 (*format)(struct PDM_DISK* p_disk, const u8* param);
    s32 (*physical_read)(struct PDM_DISK* p_disk, u8* buf, u32 block, u32 num_block, u32* p_num_success);
    s32 (*physical_write)(struct PDM_DISK* p_disk, const u8* buf, u32 block, u32 num_block, u32* p_num_success);
    s32 (*get_disk_info)(struct PDM_DISK* p_disk, PDM_DISK_INFO* p_disk_info);
} PDM_FUNCTBL;

typedef struct PDM_DISK_TBL {
    PDM_FUNCTBL* p_func;
    u32 ui_ext;
} PDM_DISK_TBL;

typedef struct PDM_INIT_DISK {
    s32 (*p_func)(PDM_DISK_TBL* p_disk_tbl, u32 ui_ext);
    u32 ui_ext;
} PDM_INIT_DISK;

typedef struct PDM_DISK {
    u32 status;
    PDM_DISK_TBL disk_tbl;
    s16 open_disk_cnt;
    s16 mount_cnt;
    u32 reserved[2];
    PDM_DISK_INFO disk_info;
    struct PDM_PARTITION* p_cur_part;
} PDM_DISK;

typedef void (*PDM_MEDIA_CALLBACK)(struct PDM_PARTITION* p_part);

typedef struct PDM_PART_CALLBACK {
    PDM_MEDIA_CALLBACK insert;
    PDM_MEDIA_CALLBACK eject;
} PDM_PART_CALLBACK;

typedef struct PDM_PARTITION {
    u32 status;
    PDM_DISK* p_disk;
    u32 part_id;
    u32 start_sector;
    u32 total_sector;
    s32 driver_last_error;
    u32 reserved[2];
    PDM_PART_CALLBACK callback;
} PDM_PARTITION;

typedef struct PDM_DISK_SET {
    u16 num_partition;
    u16 num_allocated_disk;
    PDM_DISK disk[PDM_MAX_DISK];
    PDM_PARTITION partition[PDM_MAX_PARTITION];
} PDM_DISK_SET;

typedef struct PDM_FSINFO {
    u32 free_count;
    u32 next_free;
} PDM_FSINFO;

extern PDM_DISK_SET pdm_disk_set;

void pdm_bpb_calculate_common_bpb_fields(PDM_BPB* p_bpb);
s32 pdm_bpb_get_bpb_information(PDM_BPB* p_bpb, u8* buf);
s32 pdm_bpb_get_fsinfo_information(PDM_BPB* p_bpb, u8* buf, u32* p_free_count, u32* p_next_free);
s32 pdm_bpb_check_boot_sector(u8* buf);
s32 pdm_mbr_check_master_boot_record(u8* buf, u32 total_sectors, u32* is_extended);

s32 pdm_disk_init_disk_set(void);
s32 pdm_disk_open_disk(PDM_INIT_DISK* p_init_disk_tbl, PDM_DISK** pp_disk);
s32 pdm_disk_close_disk(PDM_DISK* p_disk);
s32 pdm_disk_set_disk(PDM_DISK* p_disk, struct PDM_PARTITION* p_part);
s32 pdm_disk_get_media_information(PDM_DISK* p_disk, PDM_DISK_INFO* p_disk_info);
s32 pdm_disk_get_lba_size(PDM_DISK* p_disk, u16* p_lba_size);
s32 pdm_disk_initialize(PDM_DISK* p_disk);
s32 pdm_disk_mount(PDM_DISK* p_disk);
s32 pdm_disk_format(PDM_DISK* p_disk, const u8* param);
s32 pdm_disk_physical_read(PDM_DISK* p_disk, u8* buf, u32 lsector, u32 num_lsector, u16 bps, u32* p_num_success);
s32 pdm_disk_physical_write(PDM_DISK* p_disk, const u8* buf, u32 lsector, u32 num_lsector, u16 bps, u32* p_num_success);
s32 pdm_disk_unmount(PDM_DISK* p_disk, u32 mode);
s32 pdm_disk_finalize(PDM_DISK* p_disk);
void pdm_disk_notify_media_eject(PDM_DISK* p_disk);
void pdm_disk_notify_media_insert(PDM_DISK* p_disk);

s32 pdm_part_get_start_sector(PDM_PARTITION* p_part);
u32 pdm_part_chg_ltop(PDM_PARTITION* p_part, u32 lsector, u16 lbps);
s32 pdm_part_open_partition(PDM_DISK* p_disk, u32 part_id, PDM_PARTITION** pp_part);
s32 pdm_part_close_partition(PDM_PARTITION* p_part);
u32 pdm_part_is_initialized(PDM_PARTITION* p_part);
void pdm_part_set_driver_error_code(PDM_PARTITION* p_part, s32 error_code);
s32 pdm_part_get_driver_error_code(PDM_PARTITION* p_part);
s32 pdm_part_get_media_information(PDM_PARTITION* p_part, PDM_DISK_INFO* p_disk_info);
s32 pdm_part_initialize(PDM_PARTITION* p_part);
s32 pdm_part_mount(PDM_PARTITION* p_part);
s32 pdm_part_format(PDM_PARTITION* p_part, const u8* param);
s32 pdm_part_logical_read(PDM_PARTITION* p_part, u8* buf, u32 lsector, u32 num_sector, u16 bps, u32* p_num_success);
s32 pdm_part_logical_write(PDM_PARTITION* p_part, const u8* buf, u32 lsector, u32 num_sector, u16 bps, u32* p_num_success);
s32 pdm_part_unmount(PDM_PARTITION* p_part, u32 mode);
s32 pdm_part_finalize(PDM_PARTITION* p_part);
s32 pdm_part_set_callback(PDM_PARTITION* p_part, PDM_PART_CALLBACK* p_callback);
s32 pdm_part_clear_callback(PDM_PARTITION* p_part);
void pdm_part_set_change_media_state(PDM_DISK* p_disk, u32 event);

#ifdef __cplusplus
}
#endif

#endif
