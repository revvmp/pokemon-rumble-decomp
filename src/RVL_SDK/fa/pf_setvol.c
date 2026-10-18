#include <revolution/fa/pf_setvol.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_setvol(char drive, const char* name) {
    s32 err;
    PF_STR name_str;

    err = PFSTR_InitStr(&name_str, (const s8*)name, PF_CODE_MODE_OEM);
    if (err == 0) {
        err = PFVOL_setvol(drive, &name_str);
    } else {
        pf_vol_set.last_error = err;
    }

    return PFAPI_convertReturnValue(err);
}
