#include <revolution/fa/pf_setupfsi.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_setupfsi(char drive, u32 flags) {
    return PFAPI_convertReturnValue(PFVOL_setupfsi(drive, flags));
}
