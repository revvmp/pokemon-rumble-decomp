#include "MSL_C/buffer_io.h"

void __prep_buffer(FILE* file) {
    file->buffer_ptr = file->buffer;
    file->buffer_len = file->buffer_size;
    file->buffer_len = file->buffer_len - (file->position & file->buffer_alignment);
    file->buffer_pos = file->position;
}

int __flush_buffer(FILE* file, size_t* length) {
    size_t bufferLen;
    int result;

    bufferLen = file->buffer_ptr - file->buffer;

    if (bufferLen) {
        file->buffer_len = bufferLen;
        result = file->write_proc(file->handle, file->buffer, &file->buffer_len, file->ref_con);

        if (length) {
            *length = file->buffer_len;
        }

        if (result) {
            return result;
        }

        file->position += file->buffer_len;
    }

    file->buffer_ptr = file->buffer;
    file->buffer_len = file->buffer_size;
    file->buffer_len = file->buffer_len - (file->position & file->buffer_alignment);
    file->buffer_pos = file->position;
    return 0;
}
