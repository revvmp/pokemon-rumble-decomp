#include <revolution/fa/pf_unmount.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_unmount(char drive, u32 mode) {
    return PFAPI_convertReturnValue4unmount(PFVOL_unmount(drive, mode));
}
