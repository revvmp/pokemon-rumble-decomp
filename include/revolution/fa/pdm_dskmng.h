#ifndef FA_PDM_DSKMNG_H
#define FA_PDM_DSKMNG_H

#include <revolution/fa/pdm.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 pdm_init_diskmanager(u32 config, void* param);
s32 pdm_open_disk(PDM_INIT_DISK* p_init_disk_tbl, PDM_DISK** pp_disk);
s32 pdm_close_disk(PDM_DISK* p_disk);
s32 pdm_open_partition(PDM_DISK* p_disk, s32 part_id, PDM_PARTITION** pp_part);
s32 pdm_close_partition(PDM_PARTITION* p_part);

#ifdef __cplusplus
}
#endif

#endif
