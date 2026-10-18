#include <revolution/fa/pf_w_fsexec.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_w_api_util.h>
#include <revolution/fa/pf_str.h>

s32 pf2_w_fsexec(PF_DTA* p_dta, u32 mode, u8 attributes) {
return PFAPI_convertReturnValue(PFDIR_fsexec(p_dta, mode, attributes));
}
