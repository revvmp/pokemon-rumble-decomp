#include <private/fa/pf_stub.h>

s32 pfstub_chdir(const char* path) {
    PF_STUB_COMMAND command;
    command.command = 11;
    command.primary.path = path;
    command.callback = NULL;
    if (pfstub_com_massage(&command) == -1) {
        return -1;
    }
    return command.result;
}
