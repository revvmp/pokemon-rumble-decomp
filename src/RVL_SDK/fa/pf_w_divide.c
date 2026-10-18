#include <revolution/fa/pf_w_divide.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_w_api_util.h>
#include <revolution/fa/pf_str.h>

int pf2_w_divide(const u16* path, const u16* new_path, u32 offset) {
    s32 err;
    PF_STR path_str;
    PF_STR new_path_str;

    err = PFSTR_InitStr(&path_str, (const s8*)path, PF_CODE_MODE_UNICODE);
    err |= PFSTR_InitStr(&new_path_str, (const s8*)new_path, PF_CODE_MODE_UNICODE);
    if (err == 0) {
        err = PFFILE_divide(&path_str, &new_path_str, offset);
    } else {
        pf_vol_set.last_error = err;
    }
    return PFAPI_convertReturnValue(err);
}
