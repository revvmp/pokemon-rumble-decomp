#include <revolution/fa/pf_derrnum.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_volume.h>

int pf2_derrnum(s8 drive) {
    return PFAPI_convertDriverError(PFVOL_derrnum(drive));
}
