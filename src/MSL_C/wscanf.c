#include <wchar.h>
#include "MSL_C/stdio_api.h"

wint_t __wStringRead(void* isc, wint_t ch, int action) {
    wchar_t result;
    __wInStrCtrl* control = (__wInStrCtrl*)isc;

    switch (action) {
        case __GetAwChar:
            result = *control->wNextChar;

            if (result == 0) {
                control->wNullCharDetected = 1;
                return 0xFFFF;
            } else {
                control->wNextChar++;
                return result;
            }
        case __UngetAwChar:
            if (!control->wNullCharDetected) {
                control->wNextChar--;
            } else {
                control->wNullCharDetected = 0;
            }

            return ch;
        case __TestForwcsError:
            return control->wNullCharDetected;
    }

    return 0;
}
