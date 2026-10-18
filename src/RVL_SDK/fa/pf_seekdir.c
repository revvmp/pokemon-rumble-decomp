#include <revolution/fa/pf_seekdir.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_seekdir(PF_DIR* p_dir, u32 offset) {
    return PFAPI_convertReturnValue(PFDIR_seekdir(p_dir, offset));
}
