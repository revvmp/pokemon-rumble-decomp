#include <revolution/fa/pf_w_api_util.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_api_util.h>

u32 PFWAPI_ParseOpenModeString(const u16* mode_str) {
    u32 open_mode;
    u32 i;

    if (mode_str == NULL) {
        return 10;
    }

    i = 1;
    switch (mode_str[0]) {
    case 'r':
        open_mode = 2;
        break;
    case 'w':
        open_mode = 1;
        break;
    case 'a':
        open_mode = 4;
        break;
    default:
        return 0;
    }

    if (mode_str[1] == 'b') {
        i = 2;
    }

    switch (mode_str[i++]) {
    case 0:
        return open_mode;
    default:
        return 0;
    case '+':
        if (mode_str[i] == 0) {
            return open_mode | 8;
        }
        return 0;
    }
}
