#include <private/fa/pf_stub.h>

s32 pfstub_fclose(PF_FILE* p_file) {
    PF_STUB_COMMAND command;
    command.command = 2;
    command.primary.file = p_file;
    command.callback = NULL;
    if (pfstub_com_massage(&command) == -1) {
        return -1;
    }
    return command.result;
}
