#include <revolution/fa/pf_w_fstat.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_w_api_util.h>
#include <revolution/fa/pf_str.h>

s32 pf2_w_fstat(const u16* path, PF_STAT* p_stat) {
s32 err;
PF_STR path_str;
err = PFSTR_InitStr(&path_str, (const s8*)path, PF_CODE_MODE_UNICODE);
if (err == 0) {
    err = PFDIR_fstat(&path_str, p_stat);
} else {
    pf_vol_set.last_error = err;
}
return PFAPI_convertReturnValue(err);
}
