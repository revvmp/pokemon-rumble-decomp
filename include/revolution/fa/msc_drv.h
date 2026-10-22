#ifndef FA_MSC_DRV_H
#define FA_MSC_DRV_H

#include <revolution/fa/pdm.h>

typedef void (*PFD_MSC_MEDIA_CALLBACK)(s8 drive);

s32 pfd_mscdrv_registar_callback(PFD_MSC_MEDIA_CALLBACK attach, PFD_MSC_MEDIA_CALLBACK detach);
s32 pfd_mscdrv_is_media_insert(PDM_DISK* p_disk);
s32 pfd_mscdrv_init_drv_tbl(PDM_DISK_TBL* p_table, u32 extension);

#endif
