#ifndef MSL_C_FILE_STRUC_H
#define MSL_C_FILE_STRUC_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long __file_handle;
typedef unsigned long fpos_t;

#define __ungetc_buffer_size 2

enum __file_kinds {
    __closed_file,
    __disk_file,
    __console_file,
    __unavailable_file
};

enum __open_modes {
    __must_exist,
    __create_if_necessary,
    __create_or_truncate
};

enum __file_orientation {
    __unoriented,
    __char_oriented,
    __wide_oriented
};

enum __io_modes {
    __read = 1,
    __write = 2,
    __read_write = 3,
    __append = 4
};

enum __io_states {
    __neutral,
    __writing,
    __reading,
    __rereading
};

typedef struct __file_modes {
    unsigned long open_mode : 2;
    unsigned long io_mode : 3;
    unsigned long buffer_mode : 2;
    unsigned long file_kind : 3;
    unsigned long file_orientation : 2;
    unsigned long binary_io : 1;
} __file_modes;

typedef struct __file_state {
    unsigned long io_state : 3;
    unsigned long free_buffer : 1;
    unsigned char eof;
    unsigned char error;
} __file_state;

typedef void* __ref_con;
typedef int (*__pos_proc)(__file_handle file, fpos_t* position, int mode, __ref_con refCon);
typedef int (*__io_proc)(__file_handle file, unsigned char* buffer, size_t* count, __ref_con refCon);
typedef int (*__close_proc)(__file_handle file);

typedef struct _FILE {
    __file_handle handle;
    __file_modes mode;
    __file_state state;
    unsigned char is_dynamically_allocated;
    unsigned char char_buffer;
    unsigned char char_buffer_overflow;
    unsigned char ungetc_buffer[__ungetc_buffer_size];
    wchar_t ungetwc_buffer[__ungetc_buffer_size];
    unsigned long position;
    unsigned char* buffer;
    unsigned long buffer_size;
    unsigned char* buffer_ptr;
    unsigned long buffer_len;
    unsigned long buffer_alignment;
    unsigned long saved_buffer_len;
    unsigned long buffer_pos;
    __pos_proc position_proc;
    __io_proc read_proc;
    __io_proc write_proc;
    __close_proc close_proc;
    __ref_con ref_con;
    struct _FILE* next_file_struct;
} FILE;

#define _IOFBF 2
#define _IOLBF 1
#define _IONBF 0

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define _STATIC_FILES 4

extern FILE __files[];

#define stdin (&__files[0])
#define stdout (&__files[1])
#define stderr (&__files[2])

#ifdef __cplusplus
}
#endif

#endif
