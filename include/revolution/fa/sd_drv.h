#ifndef FA_SD_DRV_H
#define FA_SD_DRV_H

#include <revolution/fa/pdm.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*PFD_SD_MEDIA_CALLBACK)(char drive);

s32 pfd_sddrv_init(PDM_DISK* p_disk);
s32 pfd_sddrv_finalize(PDM_DISK* p_disk);
s32 pfd_sddrv_mount(PDM_DISK* p_disk);
s32 pfd_sddrv_unmount(PDM_DISK* p_disk);
s32 pfd_sddrv_format(PDM_DISK* p_disk, const u8* p_param);
s32 pfd_sddrv_pread(PDM_DISK* p_disk, u8* p_buf, u32 block, u32 count, u32* p_success);
s32 pfd_sddrv_pwrite(PDM_DISK* p_disk, const u8* p_buf, u32 block, u32 count, u32* p_success);
s32 pfd_sddrv_get_disk_info(PDM_DISK* p_disk, PDM_DISK_INFO* p_info);
s32 pfd_sddrv_init_drv_tbl(PDM_DISK_TBL* p_table, u32 extension);
s32 pfd_sddrv_registar_callback(PFD_SD_MEDIA_CALLBACK insert, PFD_SD_MEDIA_CALLBACK eject);

#ifdef __cplusplus
}
#endif

#endif
