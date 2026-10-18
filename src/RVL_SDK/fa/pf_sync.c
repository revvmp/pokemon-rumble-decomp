#include <revolution/fa/pf_sync.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_sync(char drive, u32 mode) {
    return PFAPI_convertReturnValue(PFVOL_sync(drive, mode));
}
