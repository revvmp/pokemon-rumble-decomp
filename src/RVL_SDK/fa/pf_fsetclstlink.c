#include <revolution/fa/pf_fsetclstlink.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_file.h>

s32 pf2_fsetclstlink(PF_FILE* p_file, u32 mode, PF_CLUSTER_LINK_CONFIG* config) {
    return PFAPI_convertReturnValue(PFFILE_fsetclstlink(p_file, mode, config));
}
