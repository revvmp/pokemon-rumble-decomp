#include <string.h>
#include <wchar.h>

wchar_t* wmemcpy(wchar_t* dest, const wchar_t* src, size_t count) {
    return (wchar_t*)memcpy(dest, src, count * sizeof(wchar_t));
}

wchar_t* wmemmove(wchar_t* dest, const wchar_t* src, size_t count) {
    return (wchar_t*)memmove(dest, src, count * sizeof(wchar_t));
}

wchar_t* wmemset(wchar_t* dest, wchar_t value, size_t count) {
    wchar_t* save = dest;

    while (count--) {
        *dest++ = value;
    }

    return save;
}

wchar_t* wmemchr(const wchar_t* src, wchar_t value, size_t count) {
    while (count != 0) {
        if (*src == value) {
            return (wchar_t*)src;
        }

        src++;
        count--;
    }

    return NULL;
}

int wmemcmp(const wchar_t* lhs, const wchar_t* rhs, size_t count) {
    int diff = 0;

    while (count--) {
        diff = *lhs - *rhs;

        if (diff != 0) {
            break;
        }

        lhs++;
        rhs++;
    }

    return diff;
}
