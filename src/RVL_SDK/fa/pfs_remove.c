#include <private/fa/pf_stub.h>

s32 pfstub_remove(const char* path) {
    PF_STUB_COMMAND command;
    command.command = 7;
    command.primary.path = path;
    command.callback = NULL;
    if (pfstub_com_massage(&command) == -1) {
        return -1;
    }
    return command.result;
}

s32 pfstub_remove_async(FACommandBlock* p_request) {
    PF_STUB_COMMAND* p = (PF_STUB_COMMAND*)p_request->asyncData;
    p->command = 7;
    return pfstub_com_massage(p) == -1 ? -1 : 0;
}
