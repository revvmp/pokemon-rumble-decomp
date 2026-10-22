#include <private/fa/pf_stub.h>

s32 pfstub_unmount(s8 drive, u32 mode) {
    PF_STUB_COMMAND command;
    command.command = 39;
    command.drive = drive;
    command.first.mode = mode;
    command.callback = NULL;
    if (pfstub_com_massage(&command) == -1) {
        return -1;
    }
    return command.result;
}

s32 pfstub_unmount_async(FACommandBlock* p_request) {
    PF_STUB_COMMAND* p = (PF_STUB_COMMAND*)p_request->asyncData;
    p->command = 39;
    return pfstub_com_massage(p) == -1 ? -1 : 0;
}
