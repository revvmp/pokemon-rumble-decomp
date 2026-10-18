#include <revolution/fa/pf_setvolcfg.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_setvolcfg(char drive, PF_VOLUME_CONFIG* p_config) {
    return PFAPI_convertReturnValue(PFVOL_setvolcfg(drive, p_config));
}
