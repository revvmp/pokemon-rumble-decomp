#include "MSL_C/ansi_files.h"
#include "MSL_C/misc_io.h"

void __stdio_atexit(void) {
    __stdio_exit = __close_all;
}
