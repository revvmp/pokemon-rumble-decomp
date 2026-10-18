#include <revolution/fa/pf_mount.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_mount(char drive) {
    return PFAPI_convertReturnValue(PFVOL_mount(drive));
}
