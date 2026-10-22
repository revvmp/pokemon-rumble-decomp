#include <private/fa/pf_stub.h>

s32 pfstub_mount_async(FACommandBlock* p_request) {
    PF_STUB_COMMAND* p = (PF_STUB_COMMAND*)p_request->asyncData;
    p->command = 14;
    return pfstub_com_massage(p) == -1 ? -1 : 0;
}
