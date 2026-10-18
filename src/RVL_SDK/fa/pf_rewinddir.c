#include <revolution/fa/pf_rewinddir.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_rewinddir(PF_DIR* p_dir) {
    return PFAPI_convertReturnValue(PFDIR_seekdir(p_dir, 0));
}
