#include <revolution/fa/pf_volume.h>
#include <revolution/fa/pf_str.h>

s32 PFVOL_iswriteprotected(s8 drv_char, u32* p_protected) {
    PF_VOLUME* p_vol = PFVOL_GetVolumeFromDrvChar(drv_char);
    if ((p_vol->flags & 8) == 0) {
        pf_vol_set.last_error = 9;
        p_vol->last_error = 9;
        return 9;
    }
    if ((p_vol->flags & 2) != 0) {
        *p_protected = 1;
    } else {
        *p_protected = 0;
    }
    return 0;
}
