#include <private/fa/pf_stub.h>

s32 pfstub_fwrite(FACommandBlock* p_request) {
    PF_STUB_COMMAND* p = (PF_STUB_COMMAND*)p_request->asyncData;
    p->command = 4;
    return pfstub_com_massage(p) == -1 ? -1 : 0;
}
