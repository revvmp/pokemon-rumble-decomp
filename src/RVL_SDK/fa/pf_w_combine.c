#include <revolution/fa/pf_w_combine.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_w_api_util.h>
#include <revolution/fa/pf_str.h>

int pf2_w_combine(const u16* path, const u16* path_to_combine) {
    s32 err;
    PF_STR path_str;
    PF_STR path_to_combine_str;

    err = PFSTR_InitStr(&path_str, (const s8*)path, PF_CODE_MODE_UNICODE);
    err |= PFSTR_InitStr(&path_to_combine_str, (const s8*)path_to_combine, PF_CODE_MODE_UNICODE);
    if (err == 0) {
        err = PFFILE_combine(&path_str, &path_to_combine_str);
    } else {
        pf_vol_set.last_error = err;
    }
    return PFAPI_convertReturnValue(err);
}
