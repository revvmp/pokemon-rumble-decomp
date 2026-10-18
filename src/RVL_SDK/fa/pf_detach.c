#include <revolution/fa/pf_detach.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_volume.h>

int pf2_detach(s8 drive) {
    return PFAPI_convertReturnValue(PFVOL_detach(drive));
}
