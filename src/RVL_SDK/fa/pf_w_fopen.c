#include <revolution/fa/pf_w_fopen.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_w_api_util.h>
#include <revolution/fa/pf_str.h>

PF_FILE* pf2_w_fopen(const u16* path, const u16* mode) {
u32 open_mode;
PF_FILE* p_file;
PF_STR path_str;
s32 err;
open_mode = PFWAPI_ParseOpenModeString(mode);
if (open_mode == 0) {
    pf_vol_set.last_error = 10;
    return NULL;
}
err = PFSTR_InitStr(&path_str, (const s8*)path, PF_CODE_MODE_UNICODE);
if (err == 0) {
    err = PFFILE_fopen(&path_str, open_mode, &p_file);
} else {
    pf_vol_set.last_error = err;
}
return PFAPI_convertReturnValue2NULL(err, p_file);
}
