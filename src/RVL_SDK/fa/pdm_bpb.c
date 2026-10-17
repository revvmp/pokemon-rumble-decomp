#include <revolution/fa/pdm.h>

void pdm_bpb_calculate_common_bpb_fields(PDM_BPB* p_bpb)
{
    u16 val;
    u32 num_data_sectors;

    p_bpb->log2_bytes_per_sector = 0;
    val = p_bpb->bytes_per_sector;
    while ((val >>= 1) != 0) {
        p_bpb->log2_bytes_per_sector++;
    }

    p_bpb->log2_sectors_per_cluster = 0;
    val = p_bpb->sectors_per_cluster;
    while ((val >>= 1) != 0) {
        p_bpb->log2_sectors_per_cluster++;
    }

    p_bpb->num_root_dir_sectors =
        ((p_bpb->bytes_per_sector + p_bpb->num_root_dir_entries * 32) - 1) >> p_bpb->log2_bytes_per_sector;
    p_bpb->first_data_sector = p_bpb->num_FATs * p_bpb->sectors_per_FAT +
                               (p_bpb->num_root_dir_sectors + p_bpb->num_reserved_sectors);
    num_data_sectors = p_bpb->total_sectors - p_bpb->first_data_sector;
    p_bpb->num_clusters = num_data_sectors >> p_bpb->log2_sectors_per_cluster;

    if (p_bpb->num_clusters < 0xFF5) {
        p_bpb->fat_type = PDM_FAT_12;
        p_bpb->first_root_dir_sector = p_bpb->num_reserved_sectors + p_bpb->num_FATs * p_bpb->sectors_per_FAT;
    } else if (p_bpb->num_clusters < 0xFFF5) {
        p_bpb->fat_type = PDM_FAT_16;
        p_bpb->first_root_dir_sector = p_bpb->num_reserved_sectors + p_bpb->num_FATs * p_bpb->sectors_per_FAT;
    } else {
        p_bpb->fat_type = PDM_FAT_32;
        p_bpb->first_root_dir_sector = 0;
    }
}

s32 pdm_bpb_get_bpb_information(PDM_BPB* p_bpb, u8* buf)
{
    u16 total_sectors16;
    s32 result = PDM_OK;
    u16 sectors_per_FAT16;
    u16 fs_version;

    if (!pdm_bpb_check_boot_sector(buf)) {
        result = PDM_ERR_NO_PARTITION;
    } else {
        p_bpb->bytes_per_sector = PDM_LOAD_LE16(buf, 0xB);
        p_bpb->sectors_per_cluster = buf[0xD];
        p_bpb->num_reserved_sectors = PDM_LOAD_LE16(buf, 0xE);
        p_bpb->num_FATs = buf[0x10];
        p_bpb->num_root_dir_entries = PDM_LOAD_LE16(buf, 0x11);
        p_bpb->media = buf[0x15];

        total_sectors16 = PDM_LOAD_LE16(buf, 0x13);
        p_bpb->total_sectors = total_sectors16 == 0 ? PDM_LOAD_LE32(buf + 0x20) : total_sectors16;

        sectors_per_FAT16 = PDM_LOAD_LE16(buf, 0x16);
        p_bpb->sectors_per_FAT = sectors_per_FAT16 == 0 ? PDM_LOAD_LE32(buf + 0x24) : sectors_per_FAT16;

        pdm_bpb_calculate_common_bpb_fields(p_bpb);

        switch (p_bpb->fat_type) {
        case PDM_FAT_12:
        case PDM_FAT_16:
            if (sectors_per_FAT16 == 0) {
                result = PDM_ERR_INVALID_BPB;
            }
            break;
        case PDM_FAT_32:
            if (total_sectors16 != 0 || sectors_per_FAT16 != 0) {
                result = PDM_ERR_INVALID_BPB;
            }
            fs_version = PDM_LOAD_LE16(buf, 0x2A);
            p_bpb->ext_flags = PDM_LOAD_LE16(buf, 0x28);
            p_bpb->root_dir_cluster = PDM_LOAD_LE32(buf + 0x2C);
            p_bpb->fs_info_sector = PDM_LOAD_LE16(buf, 0x30);
            p_bpb->backup_boot_sector = PDM_LOAD_LE16(buf, 0x32);
            p_bpb->first_root_dir_sector =
                p_bpb->first_data_sector + ((p_bpb->root_dir_cluster - 2) << p_bpb->log2_sectors_per_cluster);
            if (fs_version != 0) {
                result = PDM_ERR_INVALID_BPB;
            }
            break;
        default:
            result = PDM_ERR_INVALID_BPB;
            break;
        }

        if (p_bpb->ext_flags & 0x80) {
            p_bpb->num_active_FATs = 1;
            p_bpb->active_FAT_sector = p_bpb->num_reserved_sectors + (p_bpb->ext_flags & 0x7) * p_bpb->sectors_per_FAT;
        } else {
            p_bpb->num_active_FATs = p_bpb->num_FATs;
            p_bpb->active_FAT_sector = p_bpb->num_reserved_sectors;
        }
    }
    return result;
}

s32 pdm_bpb_get_fsinfo_information(PDM_BPB* p_bpb, u8* buf, u32* p_free_count, u32* p_next_free)
{
    u32 free_count;
    u32 next_free;

    *p_free_count = 0xFFFFFFFF;
    *p_next_free = 0xFFFFFFFF;

    if (PDM_LOAD_LE32(buf) == 0x41615252 && PDM_LOAD_LE32(buf + 0x1E4) == 0x61417272 &&
        PDM_LOAD_LE32(buf + 0x1FC) == 0xAA550000) {
        free_count = PDM_LOAD_LE32(buf + 0x1E8);
        next_free = PDM_LOAD_LE32(buf + 0x1EC);
        if (free_count >= 2 && free_count < p_bpb->num_clusters + 2) {
            *p_free_count = free_count;
        }
        if (next_free >= 2 && next_free < p_bpb->num_clusters + 2) {
            *p_next_free = next_free;
        }
    }
    return PDM_OK;
}
