#include <revolution/fa/pf_fsexec.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_dir.h>

s32 pf2_fsexec(PF_DTA* p_dta, u32 mode, u8 attributes) {
    return PFAPI_convertReturnValue(PFDIR_fsexec(p_dta, mode, attributes));
}
