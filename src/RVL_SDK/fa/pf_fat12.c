#include <revolution/fa/pf_fat12.h>
#include <revolution/fa/pf_sector.h>

s32 PFFAT12_ReadFATEntry(PF_VOLUME* p_vol, u16 cluster, u32* p_value) {
    u16 fat_offset;
    u16 fat_sector;
    u16 offset_in_sector;
    u16 word;
    u8 buf[sizeof(u16)];
    s32 err;
    u32 current_fat;
    s32 result;

    fat_offset = cluster + (cluster >> 1);
    fat_sector = p_vol->bpb.active_FAT_sector + (fat_offset >> p_vol->bpb.log2_bytes_per_sector);
    offset_in_sector = fat_offset & (p_vol->bpb.bytes_per_sector - 1);
    if ((p_vol->bpb.ext_flags & 0x80) != 0) {
        current_fat = p_vol->bpb.ext_flags & (0x01 | 0x02 | 0x04);
    } else {
        current_fat = 1;
    }

    do {
        if (offset_in_sector < (p_vol->bpb.bytes_per_sector - 1)) {
            err = PFSEC_ReadFAT(p_vol, &buf[0], (u32)fat_sector, offset_in_sector, 2);
        } else {
            err = PFSEC_ReadFAT(p_vol, &buf[0], (u32)fat_sector, offset_in_sector, 1);
            if (err == 0) {
                err = PFSEC_ReadFAT(p_vol, &buf[1], fat_sector + 1, 0U, 1);
            }
        }
        if (err == 0x1000 && p_vol->p_callback != NULL) {
            result = p_vol->p_callback(p_vol->last_driver_error);
            if (result == 0) {
                continue;
            }
            if (result == 1 && p_vol->bpb.num_active_FATs >= 2 && current_fat < p_vol->bpb.num_active_FATs) {
                current_fat++;
                fat_sector += (u16)p_vol->bpb.sectors_per_FAT;
                continue;
            }
        }
        if (err != 0) {
            *p_value = -1;
            return err;
        }
    } while (err != 0);

    word = PDM_SWAP16(*(u16*)buf);
    if ((cluster & 0x01) != 0) {
        *p_value = (word >> 4);
    } else {
        *p_value = word & 0xFFF;
    }
    return 0;
}


s32 PFFAT12_WriteFATEntry(PF_VOLUME* p_vol, u16 cluster, u16 value) {
    s32 err;
    u16 fat_offset;
    u16 fat_sector;
    u16 offset_in_sector;
    u16 fat_sector2;
    u16 offset_in_sector2;
    u16 word;
    u8 buf[sizeof(u16)];
    u16 current_fat;
    s32 first_error;

    fat_offset = cluster + (cluster >> 1);
    fat_sector = p_vol->bpb.active_FAT_sector + (fat_offset >> p_vol->bpb.log2_bytes_per_sector);
    offset_in_sector = fat_offset & (p_vol->bpb.bytes_per_sector - 1);
    if ((cluster & 0x01) != 0) {
        err = PFSEC_ReadFAT(p_vol, &buf[0], fat_sector, offset_in_sector, 1);
        if (err != 0) {
            return err;
        }
        word = (u16)((value << 4) + (buf[0] & 0xF));
    } else {
        fat_sector2 = p_vol->bpb.active_FAT_sector + ((fat_offset + 1) >> p_vol->bpb.log2_bytes_per_sector);
        offset_in_sector2 = (fat_offset + 1) & (p_vol->bpb.bytes_per_sector - 1);
        err = PFSEC_ReadFAT(p_vol, &buf[0], fat_sector2, offset_in_sector2, 1);
        if (err != 0) {
            return err;
        }
        word = (u16)buf[0] << 8;
        word = (word & 0xF000) + (u16)(value & 0xFFF);
    }
    *(u16*)buf = PDM_SWAP16(word);
    first_error = 0;
    for (current_fat = 0; current_fat < p_vol->bpb.num_active_FATs; ++current_fat) {
        if (offset_in_sector < p_vol->bpb.bytes_per_sector - 1) {
            err = PFSEC_WriteFAT(p_vol, &buf[0], fat_sector, offset_in_sector, 2);
            if (err != 0 && first_error == 0) first_error = err;
        } else {
            err = PFSEC_WriteFAT(p_vol, &buf[0], fat_sector, offset_in_sector, 1);
            if (err != 0 && first_error == 0) first_error = err;
            err = PFSEC_WriteFAT(p_vol, &buf[1], fat_sector + 1, 0, 1);
            if (err != 0 && first_error == 0) first_error = err;
        }
        fat_sector += p_vol->bpb.sectors_per_FAT;
    }
    return first_error;
}



s32 PFFAT12_WriteFATEntryWithBuf(PF_VOLUME* p_vol, u16 cluster, u16 value, PF_CACHE_PAGE* p_page) {
    s32 err = 0;
    u16 fat_offset;
    u16 fat_sector;
    u16 offset_in_sector;
    u16 current_fat;
    s32 first_error = 0;

    fat_offset = cluster + (cluster >> 1);
    fat_sector = p_vol->bpb.active_FAT_sector + (fat_offset >> p_vol->bpb.log2_bytes_per_sector);
    offset_in_sector = fat_offset & (p_vol->bpb.bytes_per_sector - 1);
    if (p_page->sector != fat_sector) {
        for (current_fat = 0; current_fat < p_vol->bpb.num_active_FATs; ++current_fat) {
            err = PFSEC_WriteFAT(p_vol, p_page->p_buf,
                p_page->sector + current_fat * p_vol->bpb.sectors_per_FAT, 0, p_vol->bpb.bytes_per_sector);
            if (err != 0 && first_error == 0) first_error = err;
        }
        err = PFSEC_ReadFAT(p_vol, p_page->p_buf, fat_sector, 0, p_vol->bpb.bytes_per_sector);
        if (err != 0) return err;
        p_page->sector = fat_sector;
    }
    if (cluster & 1) {
        p_page->p_buf[offset_in_sector] = (p_page->p_buf[offset_in_sector] & 0xF) | ((value << 4) & 0xF0);
        if (offset_in_sector == p_vol->bpb.bytes_per_sector - 1u) {
            for (current_fat = 0; current_fat < p_vol->bpb.num_active_FATs; ++current_fat) {
                err = PFSEC_WriteFAT(p_vol, p_page->p_buf,
                    p_page->sector + current_fat * p_vol->bpb.sectors_per_FAT, 0, p_vol->bpb.bytes_per_sector);
                if (err != 0 && first_error == 0) first_error = err;
            }
            err = PFSEC_ReadFAT(p_vol, p_page->p_buf, fat_sector + 1, 0, p_vol->bpb.bytes_per_sector);
            if (err != 0) return err;
            p_page->sector = fat_sector + 1;
            p_page->p_buf[0] = value >> 4;
        } else {
            p_page->p_buf[offset_in_sector + 1] = value >> 4;
        }
    } else {
        p_page->p_buf[offset_in_sector] = value;
        if (offset_in_sector == p_vol->bpb.bytes_per_sector - 1u) {
            for (current_fat = 0; current_fat < p_vol->bpb.num_active_FATs; ++current_fat) {
                err = PFSEC_WriteFAT(p_vol, p_page->p_buf,
                    p_page->sector + current_fat * p_vol->bpb.sectors_per_FAT, 0, p_vol->bpb.bytes_per_sector);
                if (err != 0 && first_error == 0) first_error = err;
            }
            err = PFSEC_ReadFAT(p_vol, p_page->p_buf, fat_sector + 1, 0, p_vol->bpb.bytes_per_sector);
            if (err != 0) return err;
            p_page->sector = fat_sector + 1;
            p_page->p_buf[0] = (p_page->p_buf[0] & 0xF0) | (value >> 8);
        } else {
            p_page->p_buf[offset_in_sector + 1] = (p_page->p_buf[offset_in_sector + 1] & 0xF0) | (value >> 8);
        }
    }
    if (err != 0) first_error = err;
    return first_error;
}

s32 PFFAT12_ReadFATEntryWithBuf(PF_VOLUME* p_vol, u16 cluster, u32* p_value, PF_CACHE_PAGE* p_page) {
    u32 fat_offset;
    u32 fat_sector;
    u32 current_fat;
    u16 index;
    s32 err;
    s32 result;

    fat_offset = (u16)(cluster + (cluster >> 1));
    fat_sector = (u16)(p_vol->bpb.active_FAT_sector + (fat_offset >> p_vol->bpb.log2_bytes_per_sector));
    if (p_vol->bpb.ext_flags & 0x80) current_fat = p_vol->bpb.ext_flags & 7;
    else current_fat = 1;
    err = 0;
    while (1) {
        if (err == 0x1000 && p_vol->p_callback != NULL) {
            result = p_vol->p_callback(p_vol->last_driver_error);
            if (result == 0) {
                err = 0;
                continue;
            }
            if (result == 1 && p_vol->bpb.num_active_FATs >= 2 && current_fat < p_vol->bpb.num_active_FATs) {
                current_fat++;
                fat_sector += p_vol->bpb.sectors_per_FAT;
                err = 0;
                continue;
            }
        } else {
            if (p_page->sector != fat_sector) {
                if (p_page->option == 1) {
                    for (index = 0; index < p_vol->bpb.num_active_FATs; ++index) {
                        PFSEC_WriteFAT(p_vol, p_page->p_buf,
                            p_page->sector + index * p_vol->bpb.sectors_per_FAT, 0, p_vol->bpb.bytes_per_sector);
                    }
                }
                err = PFSEC_ReadFAT(p_vol, p_page->p_buf, fat_sector, 0, p_vol->bpb.bytes_per_sector);
                if (err != 0) continue;
                p_page->sector = fat_sector;
            }
            fat_offset &= p_vol->bpb.bytes_per_sector - 1;
            if (fat_offset == p_vol->bpb.bytes_per_sector - 1) {
                *p_value = p_page->p_buf[fat_offset];
                if (p_page->option == 1) {
                    for (index = 0; index < p_vol->bpb.num_active_FATs; ++index) {
                        PFSEC_WriteFAT(p_vol, p_page->p_buf,
                            p_page->sector + index * p_vol->bpb.sectors_per_FAT, 0, p_vol->bpb.bytes_per_sector);
                    }
                }
                err = PFSEC_ReadFAT(p_vol, p_page->p_buf, fat_sector + 1, 0, p_vol->bpb.bytes_per_sector);
                if (err != 0) continue;
                p_page->sector = fat_sector + 1;
                *p_value += p_page->p_buf[0] << 8;
            } else {
                *p_value = (p_page->p_buf[fat_offset + 1] << 8) + p_page->p_buf[fat_offset];
            }
        }
        break;
    }
    if (err != 0) {
        *p_value = -1;
        return err;
    }
    if (cluster & 1) *p_value >>= 4;
    else *p_value &= 0xFFF;
    return 0;
}
