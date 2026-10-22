#include <string.h>

char* strcpy(char* dst, const char* src) {
    register unsigned char* dest = (unsigned char*)dst;
    register unsigned char* from = (unsigned char*)src;
    register unsigned long word;
    register unsigned long test;
    register unsigned long align;
    register unsigned long high_bits = 0x80808080;
    register unsigned long low_mask = 0xFEFEFEFF;

    if ((align = ((int)from & 3)) != ((int)dest & 3)) {
        goto copy_bytes;
    }

    if (align) {
        if ((*dest = *from) == 0) {
            return dst;
        }

        for (align = 3 - align; align; align--) {
            if ((*(++dest) = *(++from)) == 0) {
                return dst;
            }
        }

        ++dest;
        ++from;
    }

    word = *((unsigned long*)from);
    test = word + low_mask;
    test &= ~word;
    test &= high_bits;

    if (test) {
        goto copy_bytes;
    }

    dest -= 4;

    do {
        dest += 4;
        *(unsigned long*)dest = word;
        from += 4;
        word = *(unsigned long*)from;
        test = word + low_mask;
        test &= ~word;
        test &= high_bits;
    } while (!test);

    dest += 4;

copy_bytes:
    if ((*dest = *from) == 0) {
        return dst;
    }

    do {
        *(++dest) = *(++from);
    } while (*dest);

    return dst;
}

char* strncpy(char* dst, const char* src, size_t count) {
    const unsigned char* from = (const unsigned char*)src - 1;
    unsigned char* dest = (unsigned char*)dst - 1;

    count++;

    while (--count) {
        if (!(*++dest = *++from)) {
            while (--count) {
                *++dest = 0;
            }

            break;
        }
    }

    return dst;
}

char* strcat(char* dst, const char* src) {
    const unsigned char* from = (const unsigned char*)src - 1;
    unsigned char* dest = (unsigned char*)dst - 1;

    while (*++dest) {
    }

    dest--;

    while (*++dest = *++from) {
    }

    return dst;
}

char* strncat(char* dst, const char* src, size_t count) {
    const unsigned char* from = (const unsigned char*)src - 1;
    unsigned char* dest = (unsigned char*)dst - 1;

    while (*++dest) {
    }

    dest--;
    count++;

    while (--count) {
        if (!(*++dest = *++from)) {
            dest--;
            break;
        }
    }

    *++dest = 0;
    return dst;
}

int strcmp(const char* lhs, const char* rhs) {
    register unsigned char* left = (unsigned char*)lhs;
    register unsigned char* right = (unsigned char*)rhs;
    register unsigned long left_word;
    register unsigned long right_word;
    register unsigned long test;
    register unsigned long align;
    register unsigned long c1;
    register unsigned long c2;
    register unsigned long high_bits = 0x80808080;
    register unsigned long low_mask = 0xFEFEFEFF;

    c1 = *left;
    c2 = *right;

    if (c1 - c2) {
        return c1 - c2;
    }

    if ((align = ((int)left & 3)) != ((int)right & 3)) {
        goto compare_bytes;
    }

    if (align) {
        if (c1 == 0) {
            return 0;
        }

        for (align = 3 - align; align; align--) {
            c1 = *(++left);
            c2 = *(++right);

            if (c1 - c2) {
                return c1 - c2;
            }

            if (!c1) {
                return 0;
            }
        }

        left++;
        right++;
    }

    left_word = *(unsigned long*)left;
    right_word = *(unsigned long*)right;
    test = left_word + low_mask;
    test &= ~left_word;

    if (test & high_bits) {
        goto compare_first;
    }

    while (left_word == right_word) {
        left += 4;
        right += 4;
        left_word = *(unsigned long*)left;
        right_word = *(unsigned long*)right;

        if ((left_word + low_mask) & high_bits) {
            goto compare_first;
        }
    }

compare_first:
    c1 = *left;
    c2 = *right;

    if (c1 - c2) {
        return c1 - c2;
    }

compare_bytes:
    if (!c1) {
        return 0;
    }

    do {
        c1 = *(++left);
        c2 = *(++right);

        if (c1 - c2) {
            return c1 - c2;
        }
    } while (c1);

    return 0;
}

int strncmp(const char* lhs, const char* rhs, size_t count) {
    const unsigned char* p1 = (const unsigned char*)lhs - 1;
    const unsigned char* p2 = (const unsigned char*)rhs - 1;
    unsigned long c1;
    unsigned long c2;

    count++;

    while (--count) {
        c1 = *++p1;
        c2 = *++p2;

        if (c1 != c2) {
            return c1 - c2;
        }

        if (!c1) {
            break;
        }
    }

    return 0;
}

char* strchr(const char* str, int c) {
    const unsigned char* p = (const unsigned char*)str - 1;
    unsigned long target = (c & 0xFF);
    unsigned long ch;

    while ((ch = *++p) != 0) {
        if (ch == target) {
            return (char*)p;
        }
    }

    return target ? NULL : (char*)p;
}

char* strstr(const char* str, const char* pat) {
    unsigned char* s1;
    unsigned char* p1;
    unsigned long c1;
    unsigned long c2;
    unsigned long first;
    unsigned char* t1;
    unsigned char* t2;

    s1 = (unsigned char*)str - 1;
    p1 = (unsigned char*)pat;

    if (pat == NULL) {
        return (char*)str;
    }

    first = *p1;

    if (first == 0) {
        return (char*)str;
    }

    while ((c1 = *++s1) != 0) {
        if (c1 == first) {
            t1 = s1 - 1;
            t2 = p1 - 1;

            while ((c1 = *++t1) == (c2 = *++t2) && c1) {
            }

            if (!c2) {
                return (char*)s1;
            }
        }
    }

    return NULL;
}
