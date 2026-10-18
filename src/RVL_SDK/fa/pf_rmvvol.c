#include <revolution/fa/pf_rmvvol.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_rmvvol(char drive) {
    return PFAPI_convertReturnValue(PFVOL_rmvvol(drive));
}
