#include <stdio.h>
#include <string.h>
#include "MSL_C/buffer_io.h"
#include "MSL_C/misc_io.h"

size_t __fwrite(const void* buffer, size_t size, size_t count, FILE* file) {
    unsigned char* src;
    size_t chunk;
    size_t remaining;
    size_t written;
    int result;
    int buffered;

    if (fwide(file, 0) == 0) {
        fwide(file, -1);
    }

    remaining = size * count;

    if (remaining == 0 || file->state.error || file->mode.file_kind == __closed_file) {
        return 0;
    }

    if (file->mode.file_kind == __console_file) {
        __stdio_atexit();
    }

    buffered = !file->mode.binary_io || file->mode.buffer_mode == _IOFBF || file->mode.buffer_mode == _IOLBF;

    if (file->state.io_state == __neutral && (file->mode.io_mode & __write)) {
        if (file->mode.io_mode & __append) {
            if (_fseek(file, 0, SEEK_END)) {
                return 0;
            }
        }

        file->state.io_state = __writing;
        __prep_buffer(file);
    }

    if (file->state.io_state != __writing) {
        file->state.error = 1;
        file->buffer_len = 0;
        return 0;
    }

    src = (unsigned char*)buffer;
    written = 0;

    if (remaining && (file->buffer_ptr != file->buffer || buffered)) {
        file->buffer_len = file->buffer_size - (file->buffer_ptr - file->buffer);

        do {
            unsigned char* newline = 0;

            chunk = file->buffer_len;

            if (chunk > remaining) {
                chunk = remaining;
            }

            if (file->mode.buffer_mode == _IOLBF && chunk) {
                if ((newline = (unsigned char*)__memrchr(src, '\n', chunk)) != 0) {
                    chunk = newline + 1 - src;
                }
            }

            if (chunk != 0) {
                memcpy(file->buffer_ptr, src, chunk);
                src += chunk;
                remaining -= chunk;
                file->buffer_ptr += chunk;
                file->buffer_len -= chunk;
            }

            if (file->buffer_len == 0 || newline != 0 || file->mode.buffer_mode == _IONBF) {
                result = __flush_buffer(file, 0);

                if (result != 0) {
                    file->state.error = 1;
                    file->buffer_len = 0;
                    remaining = 0;
                    break;
                }
            }

            written += chunk;
        } while (remaining && buffered);
    }

    if (remaining && buffered == 0) {
        unsigned char* savedBuffer = file->buffer;
        size_t savedSize = file->buffer_size;

        file->buffer = src;
        file->buffer_size = remaining;
        file->buffer_ptr = src + remaining;

        if (__flush_buffer(file, &chunk) != 0) {
            file->state.error = 1;
            file->buffer_len = 0;
        } else {
            written += chunk;
        }

        file->buffer = savedBuffer;
        file->buffer_size = savedSize;
        __prep_buffer(file);
        file->buffer_len = 0;
    }

    if (file->mode.buffer_mode != _IOFBF) {
        file->buffer_len = 0;
    }

    return written / size;
}
