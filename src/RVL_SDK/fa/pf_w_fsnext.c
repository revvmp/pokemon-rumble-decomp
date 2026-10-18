#include <revolution/fa/pf_w_fsnext.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_w_api_util.h>
#include <revolution/fa/pf_str.h>

s32 pf2_w_fsnext(PF_DTA* p_dta) {
return PFAPI_convertReturnValue(PFDIR_fsnext(p_dta));
}
