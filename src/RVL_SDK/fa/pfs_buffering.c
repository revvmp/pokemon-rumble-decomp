#include <private/fa/pf_stub.h>

s32 pfstub_buffering(s8 drive, u32 mode) {
    PF_STUB_COMMAND command;
    command.command = 16;
    command.drive = drive;
    command.first.mode = mode;
    command.callback = NULL;
    if (pfstub_com_massage(&command) == -1) {
        return -1;
    }
    return command.result;
}
