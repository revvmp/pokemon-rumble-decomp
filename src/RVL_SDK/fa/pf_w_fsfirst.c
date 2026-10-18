#include <revolution/fa/pf_w_fsfirst.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_w_api_util.h>
#include <revolution/fa/pf_str.h>

s32 pf2_w_fsfirst(const u16* path, u8 attributes, PF_DTA* p_dta) {
s32 err;
PF_STR path_str;
err = PFSTR_InitStr(&path_str, (const s8*)path, PF_CODE_MODE_UNICODE);
if (err == 0) {
    err = PFDIR_fsfirst(&path_str, attributes, p_dta);
} else {
    pf_vol_set.last_error = err;
}
return PFAPI_convertReturnValue(err);
}
