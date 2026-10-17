#include <revolution/fa/pf_clib.h>

s32 pf_toupper(s32 c) {
    if ('a' <= c && c <= 'z') {
        c -= 'a' - 'A';
    }
    return c;
}

void* pf_memcpy(void* dst, const void* src, u32 length) {
    u32* ld = (u32*)dst;
    const u32* ls = (const u32*)src;
    s8* d;
    const s8* s;

    if (((u32)ld & 3) == 0 && ((u32)ls & 3) == 0) {
        for (; length > 3; length -= 4) {
            *ld++ = *ls++;
        }
    }

    d = (s8*)ld;
    s = (const s8*)ls;
    while (length-- != 0) {
        *d++ = *s++;
    }
    return dst;
}

void* pf_memset(void* dst, s32 c, u32 length) {
    s8* d = (s8*)dst;
    u32* ld;
    u32 lc;

    while (((u32)d & 3) != 0) {
        *d++ = c;
        length--;
    }

    ld = (u32*)d;
    lc = (c | (c << 8)) | ((c << 24) | (c << 16));
    for (; length > 3; length -= 4) {
        *ld++ = lc;
    }

    d = (s8*)ld;
    while (length-- != 0) {
        *d++ = c;
    }
    return dst;
}

s32 pf_memcmp(const void* s1, const void* s2, u32 length) {
    const u32* l1 = (const u32*)s1;
    const u32* l2 = (const u32*)s2;
    const u8* p1;
    const u8* p2;

    if (((u32)l1 & 3) == 0 && ((u32)l2 & 3) == 0) {
        while (length > 3) {
            if (*l1 != *l2) {
                break;
            }
            length -= 4;
            l1++;
            l2++;
        }
    }

    p1 = (const u8*)l1;
    p2 = (const u8*)l2;
    while (length-- != 0) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

u32 pf_strlen(const s8* s) {
    const s8* t;

    for (t = s; *t != 0; t++) {
    }
    return t - s;
}

s8* pf_strcpy(s8* dst, const s8* src) {
    s8* d = dst;

    while ((*d = *src) != 0) {
        src++;
        d++;
    }
    return dst;
}

s32 pf_strcmp(const s8* s1, const s8* s2) {
    const u8* p1 = (const u8*)s1;
    const u8* p2 = (const u8*)s2;

    while (*p1 != 0 && *p2 != 0 && *p1 == *p2) {
        p1++;
        p2++;
    }
    return *p1 - *p2;
}

s32 pf_strncmp(const s8* s1, const s8* s2, u32 length) {
    const u8* p1 = (const u8*)s1;
    const u8* p2 = (const u8*)s2;

    while (length-- != 0) {
        if (*p1 == 0 || *p2 == 0 || *p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

u32 pf_w_strlen(const u16* s) {
    const u16* t;

    for (t = s; *t != 0; t++) {
    }
    return ((const s8*)t - (const s8*)s) >> 1;
}

u16* pf_w_strcpy(u16* dst, const u16* src) {
    u16* d = dst;

    while ((*d = *src) != 0) {
        src++;
        d++;
    }
    return dst;
}

s32 pf_w_strcmp(const u16* s1, const u16* s2) {
    while (*s1 != 0 && *s2 != 0 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

s32 pf_w_strncmp(const u16* s1, const u16* s2, u32 length) {
    while (length-- != 0) {
        if (*s1 == 0 || *s2 == 0 || *s1 != *s2) {
            return *s1 - *s2;
        }
        s1++;
        s2++;
    }
    return 0;
}
