#include <private/fa/pf_stub.h>

s32 pfstub_setupfsi(s8 drive, u32 flags) {
    PF_STUB_COMMAND command;
    command.command = 30;
    command.drive = drive;
    command.first.flags = flags;
    command.callback = NULL;
    if (pfstub_com_massage(&command) == -1) {
        return -1;
    }
    return command.result;
}
