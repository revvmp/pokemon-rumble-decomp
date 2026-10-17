#include <revolution/fa/pf_api_util.h>
#include <stddef.h>

int pf_error_to_api_error[40] = {
    0, 22, 22, 2, 16, 90, 28, 8, 17, 8,
    22, 201, 88, 8, 8, 8, 13, 5, 8, 13,
    2, 23, 24, 21, 13, 1, 8, 8, 8, 8,
    12, 22, 22, 8, 2, 8, 8, 27, 9, 46,
};


u32 PFAPI_ParseOpenModeString(const char* mode_str) {
    u32 open_mode_util;
    u32 i;

    if (mode_str == 0) {
        return 10;
    }

    i = 1;
    switch (mode_str[0]) {
    case 'r':
        open_mode_util = 2;
        break;
    case 'w':
        open_mode_util = 1;
        break;
    case 'a':
        open_mode_util = 4;
        break;
    default:
        return 0;
    }

    if (mode_str[1] == 'b') {
        i = 2;
    }

    switch (mode_str[i++]) {
    case 0:
        return open_mode_util;
    case 't':
    default:
        return 0;
    case '+':
        if (mode_str[i] == 0) {
            return open_mode_util | 8;
        }
        return 0;
    }
}

int PFAPI_convertError(int err) {
    if (err == 0) {
        return 0;
    } else if (err == -1) {
        return -1;
    } else if (0 < err && err < 160) {
        return pf_error_to_api_error[err];
    } else if (err == 4096) {
        return 5;
    } else {
        return err;
    }
}

int PFAPI_convertDriverError(int err) {
    if (err == 0) {
        return 0;
    }
    if (err > 0 && (u32)err < 160) {
        return 1;
    }
    return err;
}

int PFAPI_convertReturnValue(int err) {
    if (err == 0) {
        return 0;
    }
    return -1;
}

int PFAPI_convertReturnValue4feof(int err) {
    if (err == 1) {
        return 1;
    }
    if (err != 0) {
        return -1;
    }
    return 0;
}

void* PFAPI_convertReturnValue2NULL(int err, void* p_stream) {
    if (err) {
        return NULL;
    } else {
        return p_stream;
    }
}

int PFAPI_convertReturnValue4unmount(int err) {
    if (err == 0) {
        return 0;
    }

    if (err == 1) {
        return 1;
    }

    return -1;
}
