#include <wstring.h>

size_t wcslen(const wchar_t* str) {
    size_t len = -1;
    wchar_t* p = (wchar_t*)str - 1;

    do {
        len++;
    } while (*++p);

    return len;
}

wchar_t* wcscpy(wchar_t* dest, const wchar_t* src) {
    const wchar_t* p = (wchar_t*)src - 1;
    wchar_t* q = (wchar_t*)dest - 1;

    while (*++q = *++p) {
    }

    return dest;
}

wchar_t* wcsncpy(wchar_t* dest, const wchar_t* src, size_t count) {
    const wchar_t* p = (const wchar_t*)src - 1;
    wchar_t* q = (wchar_t*)dest - 1;

    count++;

    while (--count) {
        if (!(*++q = *++p)) {
            while (--count) {
                *++q = 0;
            }

            break;
        }
    }

    return dest;
}

int wcscmp(const wchar_t* lhs, const wchar_t* rhs) {
    const wchar_t* p1 = (wchar_t*)lhs - 1;
    const wchar_t* p2 = (wchar_t*)rhs - 1;
    wchar_t c1, c2;

    while ((c1 = *++p1) == (c2 = *++p2)) {
        if (!c1) {
            return 0;
        }
    }

    return c1 - c2;
}

int wcsncmp(const wchar_t* lhs, const wchar_t* rhs, size_t count) {
    const wchar_t* p1 = (wchar_t*)lhs - 1;
    const wchar_t* p2 = (wchar_t*)rhs - 1;
    wchar_t c1, c2;

    count++;

    while (--count) {
        if ((c1 = *++p1) != (c2 = *++p2)) {
            return c1 - c2;
        } else if (!c1) {
            break;
        }
    }

    return 0;
}

wchar_t* wcschr(const wchar_t* str, const wchar_t chr) {
    const wchar_t* p = (wchar_t*)str - 1;
    wchar_t c = (chr & 0xFFFF);
    wchar_t ch;

    while (ch = (*++p & 0xFFFF)) {
        if (ch == c) {
            return (wchar_t*)p;
        }
    }

    return c ? NULL : (wchar_t*)p;
}
