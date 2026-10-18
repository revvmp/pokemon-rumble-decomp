#include <revolution/fa/pf_iswriteprotected.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_volume.h>

s32 pf2_iswriteprotected(s8 drive) {
    u32 protected = 0;
    s32 err = PFVOL_iswriteprotected(drive, &protected);
    if (err == 0) {
        return protected;
    }
    return PFAPI_convertReturnValue(err);
}
