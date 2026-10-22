#include "MSL_C/stdio_api.h"

int __StringRead(void* isc, int ch, int action) {
    char return_char;
    __InStrCtrl* control = (__InStrCtrl*)isc;

    switch (action) {
        case __GetAChar:
            return_char = *(control->next_char);

            if (return_char == '\0') {
                control->null_char_detected = 1;
                return -1;
            } else {
                control->next_char++;
                return (unsigned char)return_char;
            }

        case __UngetAChar:
            if (!control->null_char_detected) {
                control->next_char--;
            } else {
                control->null_char_detected = 0;
            }

            return ch;

        case __TestForError:
            return control->null_char_detected;
    }

    return 0;
}
