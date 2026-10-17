#include <revolution/fa/pf_getvolcfg.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_volume.h>

s32 pf2_getvolcfg(s8 drive, PF_VOLUME_CONFIG* config) {
    return PFAPI_convertReturnValue(PFVOL_getvolcfg(drive, config));
}
