#include <revolution/fa/pf_getdev.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_volume.h>

s32 pf2_devinf(s8 drive, PF_DEV_INF* p_info) {
    return PFAPI_convertReturnValue(PFVOL_getdev(drive, p_info));
}
