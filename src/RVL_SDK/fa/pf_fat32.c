#include <revolution/fa/pf_fat32.h>
#include <revolution/fa/pf_sector.h>
#include <stddef.h>

s32 PFFAT32_ReadFATEntry(PF_VOLUME* p_vol, u32 cluster, u32* p_value) {
    u32 fat_offset;
    u32 fat_sector;
    u16 offset_in_sector;
    u8 buf[4];
    s32 err;
    u32 current_fat;
    s32 result;

    fat_offset = cluster * 4;
    fat_sector = (u16)(p_vol->bpb.active_FAT_sector + (fat_offset >> p_vol->bpb.log2_bytes_per_sector));
    offset_in_sector = fat_offset & (p_vol->bpb.bytes_per_sector - 1);
    if ((p_vol->bpb.ext_flags & 0x80) != 0) {
        current_fat = p_vol->bpb.ext_flags & 7;
    } else {
        current_fat = 1;
    }
    do {
        err = PFSEC_ReadFAT(p_vol, buf, fat_sector, offset_in_sector, 4);
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
    *p_value = PDM_SWAP32(*(u32*)buf) & 0x0FFFFFFF;
    return 0;
}

s32 PFFAT32_WriteFATEntry(PF_VOLUME* p_vol, u32 cluster, u32 value) {
    s32 err;
    u32 fat_offset;
    u32 fat_sector;
    u16 offset_in_sector;
    u8 buf[4];
    u32 dword;
    u16 i;
    s32 first_error;

    fat_offset = cluster * 4;
    fat_sector = (u16)(p_vol->bpb.active_FAT_sector + (fat_offset >> p_vol->bpb.log2_bytes_per_sector));
    offset_in_sector = fat_offset & (p_vol->bpb.bytes_per_sector - 1);
    err = PFSEC_ReadFAT(p_vol, buf, fat_sector, offset_in_sector, 4);
    if (err != 0) {
        return err;
    }
    dword = (value & 0x0FFFFFFF) | (PDM_SWAP32(*(u32*)buf) & 0xF0000000);
    *(u32*)buf = PDM_SWAP32(dword);
    first_error = 0;
    for (i = 0; i < p_vol->bpb.num_active_FATs; i++) {
        err = PFSEC_WriteFAT(p_vol, buf, fat_sector, offset_in_sector, 4);
        if (err != 0 && first_error == 0) {
            first_error = err;
        }
        fat_sector += p_vol->bpb.sectors_per_FAT;
    }
    return first_error;
}

s32 PFFAT32_WriteFATEntryWithBuf(PF_VOLUME* p_vol, u32 cluster, u32 value, PF_CACHE_PAGE* p_page) {
    s32 err;
    u32 fat_offset;
    u32 fat_sector;
    u16 offset_in_sector;
    u32 dword;
    u16 i;
    s32 first_error;

    err = 0;
    fat_offset = cluster * 4;
    fat_sector = (u16)(p_vol->bpb.active_FAT_sector + (fat_offset >> p_vol->bpb.log2_bytes_per_sector));
    offset_in_sector = fat_offset & (p_vol->bpb.bytes_per_sector - 1);
    first_error = 0;
    if (p_page->sector != fat_sector) {
        for (i = 0; i < p_vol->bpb.num_active_FATs; i++) {
            err = PFSEC_WriteFAT(p_vol, p_page->p_buf, p_page->sector + i * p_vol->bpb.sectors_per_FAT, 0, p_vol->bpb.bytes_per_sector);
            if (err != 0 && first_error == 0) {
                first_error = err;
            }
        }
        err = PFSEC_ReadFAT(p_vol, p_page->p_buf, fat_sector, 0, p_vol->bpb.bytes_per_sector);
        if (err != 0) {
            return err;
        }
        p_page->sector = fat_sector;
    }
    dword = (value & 0x0FFFFFFF) | (PDM_SWAP32(*(u32*)(p_page->p_buf + offset_in_sector)) & 0xF0000000);
    *(u32*)(p_page->p_buf + offset_in_sector) = PDM_SWAP32(dword);
    if (err != 0) {
        first_error = err;
    }
    return first_error;
}

s32 PFFAT32_ReadFATEntryWithBuf(PF_VOLUME* p_vol, u32 cluster, u32* p_value, PF_CACHE_PAGE* p_page) {
    u32 fat_offset;
    u32 fat_sector;
    u32 current_fat;
    u16 i;
    s32 err;
    s32 result;

    fat_offset = cluster * 4;
    fat_sector = (u16)(p_vol->bpb.active_FAT_sector + (fat_offset >> p_vol->bpb.log2_bytes_per_sector));
    if ((p_vol->bpb.ext_flags & 0x80) != 0) {
        current_fat = p_vol->bpb.ext_flags & 7;
    } else {
        current_fat = 1;
    }
    if (p_page->sector != fat_sector) {
        if (p_page->option == 1) {
            for (i = 0; i < p_vol->bpb.num_active_FATs; i++) {
                PFSEC_WriteFAT(p_vol, p_page->p_buf, p_page->sector + i * p_vol->bpb.sectors_per_FAT, 0, p_vol->bpb.bytes_per_sector);
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
    *p_value = PDM_SWAP32(*(u32*)(p_page->p_buf + fat_offset)) & 0x0FFFFFFF;
    return 0;
}
