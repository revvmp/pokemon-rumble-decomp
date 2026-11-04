#include <revolution/fa/pdm.h>

s32 pdm_mbr_check_master_boot_record(u8* buf, u32 total_sectors, u32* is_master_boot)
{
    u32 start_sector[4];
    u32 num_sectors[4];
    s16 i;

    *is_master_boot = FALSE;
    if (buf[0x1FE] != 0x55 || buf[0x1FF] != 0xAA) {
        return PDM_ERR_NOT_BOOT_SECTOR;
    }

    for (i = 0; i < 4; i++, buf += 0x10) {
        start_sector[i] = 0;
        start_sector[i] = ((volatile u8*)buf)[0x1C6] + (((volatile u8*)buf)[0x1C7] << 8) + (buf[0x1C8] << 16) + (buf[0x1C9] << 24);
        num_sectors[i] = ((volatile u8*)buf)[0x1CA] + (((volatile u8*)buf)[0x1CB] << 8) + (((volatile u8*)buf)[0x1CC] << 16) + (((volatile u8*)buf)[0x1CD] << 24);
        if (start_sector[i] != 0 && num_sectors[i] != 0) {
            if (i == 0) {
                *is_master_boot = TRUE;
            }
        } else if (i == 0) {
            return PDM_ERR_NOT_BOOT_SECTOR;
        }
    }

    for (i = 0; i < 4; i++) {
        if (start_sector[i] + num_sectors[i] > total_sectors) {
            if (i == 0) {
                *is_master_boot = FALSE;
            }
            return PDM_ERR_NOT_BOOT_SECTOR;
        }
    }
    return PDM_OK;
}

s32 pdm_bpb_check_boot_sector(u8* buf)
{
    s32 is_boot = TRUE;
    u16 bytes_per_sector;
    u8 sectors_per_cluster;
    u8 media;

    if (buf[0] != 0xEB || buf[2] != 0x90) {
        if (buf[0] != 0xE9) {
            is_boot = FALSE;
        }
    }

    if (buf[0x1FE] != 0x55 || buf[0x1FF] != 0xAA) {
        is_boot = FALSE;
    }

    bytes_per_sector = PDM_LOAD_LE16(buf, 0xB);
    if (bytes_per_sector != 0x200 && bytes_per_sector != 0x400 && bytes_per_sector != 0x800 &&
        bytes_per_sector != 0x1000) {
        is_boot = FALSE;
    }

    sectors_per_cluster = buf[0xD];
    if (sectors_per_cluster != 1 && sectors_per_cluster != 2 && sectors_per_cluster != 4 && sectors_per_cluster != 8 &&
        sectors_per_cluster != 16 && sectors_per_cluster != 32 && sectors_per_cluster != 64 &&
        sectors_per_cluster != 128) {
        is_boot = FALSE;
    }

    media = buf[0x15];
    if (media != 0xF0 && media != 0xF8 && media != 0xF9 && media != 0xFA && media != 0xFB && media != 0xFC &&
        media != 0xFD && media != 0xFE && media != 0xFF) {
        is_boot = FALSE;
    }
    return is_boot;
}
