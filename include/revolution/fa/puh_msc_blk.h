#ifndef FA_PUH_MSC_BLK_H
#define FA_PUH_MSC_BLK_H

#include <revolution/fa/pdm.h>

s32 pfd_mscdrv_init_drv_tbl(PDM_DISK_TBL* p_table, u32 extension);
s32 uhf_msc_is_media_insert(PDM_DISK* p_disk);

#endif
