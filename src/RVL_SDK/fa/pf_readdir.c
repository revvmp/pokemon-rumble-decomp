#include <revolution/fa/pf_readdir.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_readdir(PF_DIR* p_dir, PF_DIRENT* p_entry) {
    return PFAPI_convertReturnValue(PFDIR_readdir(p_dir, p_entry));
}
