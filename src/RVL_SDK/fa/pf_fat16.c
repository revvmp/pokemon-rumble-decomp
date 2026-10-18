#include <revolution/fa/pf_fat16.h>
#include <revolution/fa/pf_sector.h>

s32 PFFAT16_ReadFATEntry(PF_VOLUME* p_vol, u32 cluster, u32* p_value) {
    u32 fat_offset;
    u32 fat_sector;
    u16 offset_in_sector;
    u8 buf[2];
    s32 err;
    u32 current_fat;
    s32 result;

    fat_offset = cluster * 2;
    fat_sector = (u16)(p_vol->bpb.active_FAT_sector + ((cluster * 2) >> p_vol->bpb.log2_bytes_per_sector));
    offset_in_sector = fat_offset & (p_vol->bpb.bytes_per_sector - 1);
    if (p_vol->bpb.ext_flags & 0x80) {
        current_fat = p_vol->bpb.ext_flags & 7;
    } else {
        current_fat = 1;
    }
    do {
        err = PFSEC_ReadFAT(p_vol, buf, fat_sector, offset_in_sector, 2);
        if (err == 0x1000 && p_vol->p_callback != NULL) {
            result = p_vol->p_callback(p_vol->last_driver_error);
            if (result == 0) {
                continue;
            }
            if (result == 1 && p_vol->bpb.num_active_FATs >= 2 && current_fat < p_vol->bpb.num_active_FATs) {
                current_fat++;
                fat_sector += p_vol->bpb.sectors_per_FAT;
                continue;
            }
        }
        if (err != 0) {
            *p_value = -1;
            return err;
        }
    } while (err != 0);
    *p_value = PDM_SWAP16(*(u16*)buf);
    return 0;
}

s32 PFFAT16_WriteFATEntry(PF_VOLUME* p_vol, u32 cluster, u32 value) {
    s32 err;
    u32 fat_offset;
    u16 fat_sector;
    u16 offset_in_sector;
    u8 buf[2];
    u32 current_fat;
    s32 first_error;

    fat_offset = cluster * 2;
    fat_sector = p_vol->bpb.active_FAT_sector + ((cluster * 2) >> p_vol->bpb.log2_bytes_per_sector);
    offset_in_sector = fat_offset & (p_vol->bpb.bytes_per_sector - 1);
    *(u16*)buf = PDM_SWAP16((u16)value);
    first_error = 0;
    for (current_fat = 0; current_fat < p_vol->bpb.num_active_FATs; ++current_fat) {
        err = PFSEC_WriteFAT(p_vol, buf, fat_sector, offset_in_sector, 2);
        if (err != 0 && first_error == 0) {
            first_error = err;
        }
        fat_sector += p_vol->bpb.sectors_per_FAT;
    }
    return first_error;
}


s32 PFFAT16_WriteFATEntryWithBuf(PF_VOLUME* p_vol, u32 cluster, u32 value, PF_CACHE_PAGE* p_page) {
    s32 err = 0;
    u32 fat_offset;
    u16 fat_sector;
    u16 offset_in_sector;
    u32 current_fat;
    s32 first_error = 0;

    fat_offset = cluster * 2;
    fat_sector = p_vol->bpb.active_FAT_sector + (fat_offset >> p_vol->bpb.log2_bytes_per_sector);
    offset_in_sector = fat_offset & (p_vol->bpb.bytes_per_sector - 1);
    if (p_page->sector != fat_sector) {
        for (current_fat = 0; current_fat < p_vol->bpb.num_active_FATs; ++current_fat) {
            err = PFSEC_WriteFAT(p_vol, p_page->p_buf,
                p_page->sector + current_fat * p_vol->bpb.sectors_per_FAT, 0, p_vol->bpb.bytes_per_sector);
            if (err != 0 && first_error == 0) {
                first_error = err;
            }
        }
        p_page->sector = fat_sector;
        err = PFSEC_ReadFAT(p_vol, p_page->p_buf, fat_sector, 0, p_vol->bpb.bytes_per_sector);
    }
    p_page->p_buf[offset_in_sector] = value;
    p_page->p_buf[offset_in_sector + 1] = value >> 8;
    if (err != 0) {
        first_error = err;
    }
    return first_error;
}

s32 PFFAT16_ReadFATEntryWithBuf(PF_VOLUME* p_vol, u32 cluster, u32* p_value, PF_CACHE_PAGE* p_page) {
    u32 fat_offset;
    u32 fat_sector;
    u32 current_fat;
    u16 index;
    s32 err;
    s32 result;

    fat_offset = cluster * 2;
    fat_sector = (u16)(p_vol->bpb.active_FAT_sector + (fat_offset >> p_vol->bpb.log2_bytes_per_sector));
    if (p_vol->bpb.ext_flags & 0x80) {
        current_fat = p_vol->bpb.ext_flags & 7;
    } else {
        current_fat = 1;
    }
    if (p_page->sector != fat_sector) {
        if (p_page->option == 1) {
            for (index = 0; index < p_vol->bpb.num_active_FATs; ++index) {
                PFSEC_WriteFAT(p_vol, p_page->p_buf,
                    p_page->sector + index * p_vol->bpb.sectors_per_FAT, 0, p_vol->bpb.bytes_per_sector);
            }
        }
        do {
            err = PFSEC_ReadFAT(p_vol, p_page->p_buf, fat_sector, 0, p_vol->bpb.bytes_per_sector);
            if (err == 0x1000 && p_vol->p_callback != NULL) {
                result = p_vol->p_callback(p_vol->last_driver_error);
                if (result == 0) {
                    continue;
                }
                if (result == 1 && p_vol->bpb.num_active_FATs >= 2 && current_fat < p_vol->bpb.num_active_FATs) {
                    current_fat++;
                    fat_sector += p_vol->bpb.sectors_per_FAT;
                    continue;
                }
            }
            if (err != 0) {
                return err;
            }
            p_page->sector = fat_sector;
        } while (err != 0);
    }
    fat_offset &= p_vol->bpb.bytes_per_sector - 1;
    *p_value = PDM_SWAP16(*(u16*)(p_page->p_buf + fat_offset));
    return 0;
}
