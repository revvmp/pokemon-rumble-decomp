#include <private/fa/pf_stub.h>

s32 pfstub_errnum(void) {
    PF_STUB_COMMAND command;
    command.command = 19;
    command.callback = NULL;
    if (pfstub_com_massage(&command) == -1) {
        return -1;
    }
    return command.result;
}
