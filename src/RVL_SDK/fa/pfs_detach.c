#include <private/fa/pf_stub.h>

s32 pfstub_detach(s8 drive) {
    PF_STUB_COMMAND command;
    command.command = 41;
    command.drive = drive;
    command.callback = NULL;
    if (pfstub_com_massage(&command) == -1) {
        return -1;
    }
    return command.result;
}
