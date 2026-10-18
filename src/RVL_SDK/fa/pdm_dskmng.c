#include <revolution/fa/pdm_dskmng.h>

PDM_DISK_SET pdm_disk_set;

s32 pdm_init_diskmanager(u32 config, void* param)
{
    return pdm_disk_init_disk_set();
}

s32 pdm_open_disk(PDM_INIT_DISK* p_init_disk_tbl, PDM_DISK** pp_disk)
{
    s32 result;

    *pp_disk = NULL;
    result = pdm_disk_open_disk(p_init_disk_tbl, pp_disk);
    if (result != PDM_OK) {
        return result;
    }
    return PDM_OK;
}

s32 pdm_close_disk(PDM_DISK* p_disk)
{
    return pdm_disk_close_disk(p_disk);
}

s32 pdm_open_partition(PDM_DISK* p_disk, s32 part_id, PDM_PARTITION** pp_part)
{
    s32 result;

    *pp_part = NULL;
    result = pdm_part_open_partition(p_disk, part_id, pp_part);
    if (result != PDM_OK) {
        return result;
    }
    return PDM_OK;
}

s32 pdm_close_partition(PDM_PARTITION* p_part)
{
    s32 result;

    result = pdm_part_close_partition(p_part);
    if (result != PDM_OK) {
        return result;
    }
    return PDM_OK;
}
