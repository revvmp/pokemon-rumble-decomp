#include <revolution/fa/pf_buffering.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_volume.h>

int pf2_buffering(s8 drive, int mode) {
    return PFAPI_convertReturnValue(PFVOL_buffering(drive, mode));
}
