#include <revolution/fa/pf_detach.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_detach(char drive) {
    return PFAPI_convertReturnValue(PFVOL_detach(drive));
}
