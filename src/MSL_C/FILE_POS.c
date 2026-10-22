#include <stdio.h>
#include "MSL_C/buffer_io.h"
#include "MSL_C/errno.h"

long _ftell(FILE* file) {
    int chars_in_undo_buffer = 0;
    long position;
    int i;
    unsigned char* chars;

    if ((unsigned char)(file->mode.file_kind + 0xff) > 1 || file->state.error) {
        errno = ESPIPE;
        return -1;
    }

    if (file->state.io_state == __neutral) {
        return file->position;
    }

    position = file->buffer_pos + (file->buffer_ptr - file->buffer);

    if (file->state.io_state >= __rereading) {
        chars_in_undo_buffer = file->state.io_state - __rereading + 1;
        position -= chars_in_undo_buffer;
    }

    if (!file->mode.binary_io) {
        chars = file->buffer;
        i = (file->buffer_ptr - file->buffer) - chars_in_undo_buffer;

        while (i--) {
            if (*chars++ == '\n') {
                position++;
            }
        }
    }

    return position;
}

long ftell(FILE* file) {
    return _ftell(file);
}

int _fseek(FILE* file, fpos_t offset, int mode) {
    if (file->mode.file_kind != __disk_file || file->state.error) {
        errno = ESPIPE;
        return -1;
    }

    if (file->state.io_state == __writing) {
        if (__flush_buffer(file, NULL)) {
            file->state.error = 1;
            file->buffer_len = 0;
            errno = ESPIPE;
            return -1;
        }
    }

    if (mode == SEEK_CUR) {
        mode = SEEK_SET;
        offset += _ftell(file);
    }

    if (mode != SEEK_END && file->mode.io_mode != __read_write &&
        (file->state.io_state == __reading || file->state.io_state == __rereading)) {
        if (offset >= file->position || offset < file->buffer_pos) {
            file->state.io_state = __neutral;
        } else {
            file->buffer_ptr = file->buffer + (offset - file->buffer_pos);
            file->buffer_len = file->position - offset;
            file->state.io_state = __reading;
        }
    } else {
        file->state.io_state = __neutral;
    }

    if (file->state.io_state == __neutral) {
        if (file->position_proc) {
            if ((*file->position_proc)(file->handle, &offset, mode, file->ref_con)) {
                file->state.error = 1;
                file->buffer_len = 0;
                errno = ESPIPE;
                return -1;
            }
        }

        file->state.eof = 0;
        file->position = offset;
        file->buffer_len = 0;
    }

    return 0;
}
