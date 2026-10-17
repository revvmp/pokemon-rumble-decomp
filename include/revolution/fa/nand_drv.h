#ifndef FA_NAND_DRV_H
#define FA_NAND_DRV_H

#include <revolution/fa/pdm.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 pfd_nanddrv_init_drv_tbl(PDM_DISK_TBL* p_table, u32 extension);
s32 pfd_nanddrv_register_file(const s8* p_path, u32 size);
s32 NandPhysicalRead(u32 count, u8* p_buf, u32 block, u32 sector_size, u32* p_success, const PDM_DISK* p_disk);
s32 NandPhysicalWrite(u32 count, const u8* p_buf, u32 block, u32 sector_size, u32* p_success, const PDM_DISK* p_disk);

#ifdef __cplusplus
}
#endif

#endif
