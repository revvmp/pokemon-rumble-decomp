#include <revolution/fa/pf_buffering.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_buffering(char drive, u32 mode) {
    return PFAPI_convertReturnValue(PFVOL_buffering(drive, mode));
}
