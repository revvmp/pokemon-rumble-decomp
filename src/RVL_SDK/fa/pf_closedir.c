#include <revolution/fa/pf_closedir.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_dir.h>

int pf2_closedir(PF_DIR* p_dir) {
    return PFAPI_convertReturnValue(PFDIR_closedir(p_dir));
}
