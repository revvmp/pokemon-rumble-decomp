#include <revolution/fa/pf_telldir.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_telldir(PF_DIR* p_dir, u32* p_offset) {
    return PFAPI_convertReturnValue(PFDIR_telldir(p_dir, p_offset));
}
