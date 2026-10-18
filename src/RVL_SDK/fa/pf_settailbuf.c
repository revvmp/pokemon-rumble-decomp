#include <revolution/fa/pf_settailbuf.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

int pf2_settailbuf(char drive, u32 size, PF_TAIL_BUF* p_buffer) {
    return PFAPI_convertReturnValue(PFVOL_settailbuf(drive, size, p_buffer));
}
