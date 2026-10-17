#include <revolution/types.h>
#include <revolution/os.h>
#include "MSL_C/UART.h"
#include "MSL_C/console_io.h"

static BOOL initialized;

int __TRK_write_console(__file_handle handle, unsigned char* buffer, size_t* count, __ref_con refCon);

int __write_console(__file_handle handle, unsigned char* buffer, size_t* count, __ref_con refCon) {
    if ((OSGetConsoleType() & 0x20000000) == 0) {
        int error = kUARTNoError;

        if (initialized == FALSE) {
            error = InitializeUART(kBaud57600);

            if (error == kUARTNoError) {
                initialized = TRUE;
            }
        }

        if (error != kUARTNoError) {
            return 1;
        }

        if (WriteUARTN(buffer, *count) != kUARTNoError) {
            *count = 0;
            return 1;
        }
    }

    __TRK_write_console(handle, buffer, count, refCon);
    return 0;
}

int __close_console(__file_handle handle) {
    return 0;
}
