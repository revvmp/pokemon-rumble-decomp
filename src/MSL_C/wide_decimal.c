#include <MSL_C/wide_decimal.h>

long wide_string_to_decimal(const wchar_t* str) {
    return wcstol(str, NULL, 10);
}
