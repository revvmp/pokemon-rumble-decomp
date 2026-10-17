#include <revolution/fa/pf_setclstlink.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_setclstlink(char drive, u32 mode, PF_CLUSTER* p_info) {
    return PFAPI_convertReturnValue(PFVOL_setclstlink(drive, mode, p_info));
}
