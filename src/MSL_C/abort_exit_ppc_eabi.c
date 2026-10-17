#include <revolution/types.h>
#include <signal.h>
#include <stdlib.h>
#include "MSL_C/misc_io.h"

void (*__stdio_exit)(void);
BOOL __aborting;

void abort(void) {
    raise(SIGABRT);
    __aborting = TRUE;
    exit(1);
}
