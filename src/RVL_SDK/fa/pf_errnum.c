#include <revolution/fa/pf_errnum.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_volume.h>

int pf2_errnum(void) {
    return PFAPI_convertError(PFVOL_errnum());
}
