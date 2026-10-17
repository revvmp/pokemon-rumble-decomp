#include <revolution/fa/pf_setencode.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_setencode(u32 mode) {
    return PFAPI_convertReturnValue(PFVOL_setencode(mode));
}
