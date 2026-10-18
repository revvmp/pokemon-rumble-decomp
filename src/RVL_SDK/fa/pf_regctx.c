#include <revolution/fa/pf_regctx.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_regctx(void) {
    return PFAPI_convertReturnValue(PFVOL_regctx());
}
