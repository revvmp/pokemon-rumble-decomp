#include <revolution/fa/pf_unregctx.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_unregctx(void) {
    return PFAPI_convertReturnValue(PFVOL_unregctx());
}
