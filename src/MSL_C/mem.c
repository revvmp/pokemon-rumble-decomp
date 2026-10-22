#include <string.h>
#include "MSL_C/mem_funcs.h"

void* memmove(void* dst, const void* src, size_t count) {
    const unsigned char* source;
    unsigned char* dest;
    int reverse = (unsigned long)dst > (unsigned long)src;

    if (count >= 32) {
        if (((int)dst ^ (int)src) & 3) {
            if (!reverse) {
                __copy_longs_unaligned(dst, src, count);
            } else {
                __copy_longs_rev_unaligned(dst, src, count);
            }
        } else {
            if (!reverse) {
                __copy_longs_aligned(dst, src, count);
            } else {
                __copy_longs_rev_aligned(dst, src, count);
            }
        }

        return dst;
    }

    if (!reverse) {
        source = (const unsigned char*)src - 1;
        dest = (unsigned char*)dst - 1;
        count++;

        while (--count) {
            *++dest = *++source;
        }
    } else {
        source = (const unsigned char*)src + count;
        dest = (unsigned char*)dst + count;
        count++;

        while (--count) {
            *--dest = *--source;
        }
    }

    return dst;
}

void* memchr(const void* src, int value, size_t count) {
    unsigned char target = (unsigned char)value;
    const unsigned char* p = (const unsigned char*)src - 1;

    count++;

    while (--count) {
        if (*++p == target) {
            return (void*)p;
        }
    }

    return NULL;
}

void* __memrchr(const void* src, int value, size_t count) {
    const unsigned char* p = (const unsigned char*)src + count;
    unsigned char target = (unsigned char)value;

    count++;

    while (--count) {
        if (*--p == target) {
            return (void*)p;
        }
    }

    return NULL;
}

int memcmp(const void* lhs, const void* rhs, size_t count) {
    const unsigned char* p1 = (const unsigned char*)lhs - 1;
    const unsigned char* p2 = (const unsigned char*)rhs - 1;

    count++;

    while (--count) {
        if (*++p1 != *++p2) {
            return (*p1 < *p2) ? -1 : 1;
        }
    }

    return 0;
}
