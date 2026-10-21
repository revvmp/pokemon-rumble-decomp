#include <revolution/fa/pfd_cmn.h>
#include <revolution/fa/pf_volume.h>

u8 pfd_get_media_drv_char(PDM_DISK* p_disk, char* p_chars, u8 maximum) {
    u32 i;
    u8 count;
    char* p_output;
    PDM_PARTITION* p_part;
    u8 j;
    PF_VOLUME* p_vol;
    if (p_chars == NULL || maximum == 0) {
        return 0;
    }
    p_part = pdm_disk_set.partition;
    p_output = p_chars;
    count = 0;
    for (i = 0; i < 26; ++i, ++p_part) {
        if (pdm_part_is_initialized(p_part) && p_part->p_disk == p_disk) {
            p_vol = pf_vol_set.volumes;
            for (j = 0; j < 26; ++j, ++p_vol) {
                if (p_vol->p_part == p_part) {
                    *p_output++ = p_vol->drv_char;
                    if (++count == maximum) {
                        break;
                    }
                }
            }
        }
        if (count == maximum) {
            break;
        }
    }
    return count;
}
