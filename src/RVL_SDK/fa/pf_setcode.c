#include <revolution/fa/pf_setcode.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_setcode(PF_CHARCODE* p_codeset) {
    return PFAPI_convertReturnValue(PFVOL_setcode(p_codeset));
}
