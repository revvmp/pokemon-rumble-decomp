#ifndef MSL_C_STDIO_API_H
#define MSL_C_STDIO_API_H

#include <stddef.h>
#include <wchar_t.h>

#ifdef __cplusplus
extern "C" {
#endif

enum __ReadProcActions {
    __GetAChar,
    __UngetAChar,
    __TestForError
};

enum __WReadProcActions {
    __GetAwChar,
    __UngetAwChar,
    __TestForwcsError
};

typedef struct {
    char* next_char;
    int null_char_detected;
} __InStrCtrl;

typedef struct {
    wchar_t* wNextChar;
    int wNullCharDetected;
} __wInStrCtrl;

int __StringRead(void* isc, int ch, int action);
int __StringRead(void* isc, int ch, int action);
wint_t __wStringRead(void* isc, wint_t ch, int action);

#ifdef __cplusplus
}
#endif

#endif
