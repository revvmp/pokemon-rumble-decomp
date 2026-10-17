#include "MSL_C/ansi_files.h"
#include "MSL_C/console_io.h"
#include <stdlib.h>

static console_buff stdin_buff;
static console_buff stdout_buff;
static console_buff stderr_buff;

FILE __files[_STATIC_FILES] = {
    {
        0,
        {__must_exist, __read, console_buff_mode, __console_file, __unoriented, 0},
        {__neutral, 0, 0, 0},
        0,
        0,
        0,
        {0, 0},
        {0, 0},
        0,
        stdin_buff,
        console_buff_size,
        stdin_buff,
        0,
        0,
        0,
        0,
        0,
        __read_console,
        __write_console,
        __close_console,
        0,
        &__files[1],
    },
    {
        1,
        {__must_exist, __write, console_buff_mode, __console_file, __unoriented, 0},
        {__neutral, 0, 0, 0},
        0,
        0,
        0,
        {0, 0},
        {0, 0},
        0,
        stdout_buff,
        console_buff_size,
        stdout_buff,
        0,
        0,
        0,
        0,
        0,
        __read_console,
        __write_console,
        __close_console,
        0,
        &__files[2],
    },
    {
        2,
        {__must_exist, __write, _IONBF, __console_file, __unoriented, 0},
        {__neutral, 0, 0, 0},
        0,
        0,
        0,
        {0, 0},
        {0, 0},
        0,
        stderr_buff,
        console_buff_size,
        stderr_buff,
        0,
        0,
        0,
        0,
        0,
        __read_console,
        __write_console,
        __close_console,
        0,
        &__files[3],
    },
};

void __close_all(void) {
    FILE* file = &__files[0];
    FILE* last;

    while (file) {
        if (file->mode.file_kind != __closed_file) {
            fclose(file);
        }

        last = file;
        file = file->next_file_struct;

        if (last->is_dynamically_allocated) {
            free(last);
        } else {
            last->mode.file_kind = __unavailable_file;

            if (file != NULL && file->is_dynamically_allocated) {
                last->next_file_struct = NULL;
            }
        }
    }
}

unsigned int __flush_all(void) {
    unsigned int result = 0;
    FILE* file = &__files[0];

    while (file) {
        if (file->mode.file_kind && fflush(file)) {
            result = -1;
        }

        file = file->next_file_struct;
    }

    return result;
}
