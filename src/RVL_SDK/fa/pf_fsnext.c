#include <revolution/fa/pf_fsnext.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_dir.h>

s32 pf2_fsnext(PF_DTA* p_dta) {
    return PFAPI_convertReturnValue(PFDIR_fsnext(p_dta));
}
