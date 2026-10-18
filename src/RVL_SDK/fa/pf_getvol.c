#include <revolution/fa/pf_getvol.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_volume.h>

s32 pf2_getvol(s8 drive, PF_VOL_INF* p_info) {
    return PFAPI_convertReturnValue(PFVOL_getvol(drive, p_info));
}
