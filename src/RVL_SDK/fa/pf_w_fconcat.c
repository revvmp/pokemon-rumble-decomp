#include <revolution/fa/pf_w_fconcat.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_w_api_util.h>
#include <revolution/fa/pf_str.h>

s32 pf2_w_fconcat(const u16* path, const u16* source) {
PF_STR path_str;
PF_STR source_str;
s32 err;
err = PFSTR_InitStr(&path_str, (const s8*)path, PF_CODE_MODE_UNICODE);
err |= PFSTR_InitStr(&source_str, (const s8*)source, PF_CODE_MODE_UNICODE);
if (err == 0) {
    err = PFFILE_combine(&path_str, &source_str);
} else {
    pf_vol_set.last_error = err;
}
return PFAPI_convertReturnValue(err);
}
