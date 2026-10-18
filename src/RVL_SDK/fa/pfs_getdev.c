#include <private/fa/pf_stub.h>

s32 pfstub_devinf(s8 drive, struct PF_DEV_INF* p_info) {
    PF_STUB_COMMAND command;
    command.command = 20;
    command.drive = drive;
    command.primary.device_info = p_info;
    command.callback = NULL;
    if (pfstub_com_massage(&command) == -1) {
        return -1;
    }
    return command.result;
}
