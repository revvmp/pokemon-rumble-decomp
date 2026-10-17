#include <revolution/fa/pf_fchdir.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_dir.h>

s32 pf2_fchdir(PF_DIR* p_dir) {
    return PFAPI_convertReturnValue(PFDIR_fchdir(p_dir));
}
