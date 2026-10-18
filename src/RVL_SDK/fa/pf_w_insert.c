#include <revolution/fa/pf_w_insert.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_w_api_util.h>
#include <revolution/fa/pf_str.h>

s32 pf2_w_insert(const u16* path, u32 count) {
s32 err;
PF_STR path_str;
u32 inserted;
u32 removed;
err = PFSTR_InitStr(&path_str, (const s8*)path, PF_CODE_MODE_UNICODE);
if (err == 0) {
    PFFILE_cinsert(&path_str, 0, count, &inserted);
    if (count != inserted || count == 0) {
        if (inserted != 0) {
            PFSTR_InitStr(&path_str, (const s8*)path, PF_CODE_MODE_UNICODE);
            PFFILE_cdelete(&path_str, 0, inserted, &removed);
        }
        err = -1;
    }
} else {
    pf_vol_set.last_error = err;
}
return PFAPI_convertReturnValue(err);
}
