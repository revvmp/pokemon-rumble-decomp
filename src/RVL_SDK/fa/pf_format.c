#include <revolution/fa/pf_format.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_volume.h>

s32 pf2_format(s8 drive, const char* parameter) {
    return PFAPI_convertReturnValue(PFVOL_format(drive, (u8*)parameter));
}
