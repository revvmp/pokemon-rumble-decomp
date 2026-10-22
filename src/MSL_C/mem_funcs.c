#include "MSL_C/mem_funcs.h"

void __copy_longs_aligned(void* dst, const void* src, size_t count) {
    unsigned long i;
    unsigned char* dest;
    const unsigned char* source;
    unsigned long* long_dest;
    const unsigned long* long_source;

    i = (-(unsigned long)dst) & 3;
    source = (const unsigned char*)src - 1;
    dest = (unsigned char*)dst - 1;

    if (i) {
        count -= i;

        do {
            *++dest = *++source;
        } while (--i);
    }

    long_source = (const unsigned long*)(source - 3);
    long_dest = (unsigned long*)(dest - 3);
    i = count >> 5;

    if (i) {
        do {
            long_dest[1] = long_source[1];
            long_dest[2] = long_source[2];
            long_dest[3] = long_source[3];
            long_dest[4] = long_source[4];
            long_dest[5] = long_source[5];
            long_dest[6] = long_source[6];
            long_dest[7] = long_source[7];
            *(long_dest += 8) = *(long_source += 8);
        } while (--i);
    }

    i = (count & 31) >> 2;

    if (i) {
        do {
            *++long_dest = *++long_source;
        } while (--i);
    }

    source = (const unsigned char*)long_source + 3;
    dest = (unsigned char*)long_dest + 3;
    count &= 3;

    if (count) {
        do {
            *++dest = *++source;
        } while (--count);
    }
}

void __copy_longs_rev_aligned(void* dst, const void* src, size_t count) {
    unsigned char* dest = (unsigned char*)dst + count;
    const unsigned char* source = (const unsigned char*)src + count;
    unsigned long* long_dest;
    const unsigned long* long_source;
    unsigned long i;

    i = (unsigned long)dest & 3;

    if (i) {
        count -= i;

        do {
            *--dest = *--source;
        } while (--i);
    }

    long_source = (const unsigned long*)source;
    long_dest = (unsigned long*)dest;
    i = count >> 5;

    if (i) {
        do {
            long_dest[-1] = long_source[-1];
            long_dest[-2] = long_source[-2];
            long_dest[-3] = long_source[-3];
            long_dest[-4] = long_source[-4];
            long_dest[-5] = long_source[-5];
            long_dest[-6] = long_source[-6];
            long_dest[-7] = long_source[-7];
            *(long_dest -= 8) = *(long_source -= 8);
        } while (--i);
    }

    i = (count & 31) >> 2;

    if (i) {
        do {
            *--long_dest = *--long_source;
        } while (--i);
    }

    source = (const unsigned char*)long_source;
    dest = (unsigned char*)long_dest;
    count &= 3;

    if (count) {
        do {
            *--dest = *--source;
        } while (--count);
    }
}

void __copy_longs_unaligned(void* dst, const void* src, size_t count) {
    unsigned long i;
    unsigned char* dest;
    const unsigned char* source;
    unsigned long* long_dest;
    const unsigned long* long_source;
    unsigned long first;
    unsigned long second;
    unsigned int src_offset;
    unsigned int left_shift;
    unsigned int right_shift;

    i = (-(unsigned long)dst) & 3;
    source = (const unsigned char*)src - 1;
    dest = (unsigned char*)dst - 1;

    if (i) {
        count -= i;

        do {
            *++dest = *++source;
        } while (--i);
    }

    long_dest = (unsigned long*)(dest - 3);
    src_offset = ((unsigned int)(source + 1)) & 3;
    left_shift = src_offset << 3;
    right_shift = 32 - left_shift;
    i = count >> 3;
    source -= src_offset;
    long_source = (const unsigned long*)(source + 1);
    first = *long_source;

    do {
        second = long_source[1];
        long_dest[1] = (first << left_shift) | (second >> right_shift);
        first = long_source[2];
        long_source += 2;
        *(long_dest += 2) = (second << left_shift) | (first >> right_shift);
    } while (--i);

    if (count & 4) {
        second = *++long_source;
        *++long_dest = (first << left_shift) | (second >> right_shift);
    }

    count &= 3;
    source = (const unsigned char*)long_source + 3;
    dest = (unsigned char*)long_dest + 3;

    if (count) {
        source -= 4 - src_offset;

        do {
            *++dest = *++source;
        } while (--count);
    }
}

void __copy_longs_rev_unaligned(void* dst, const void* src, size_t count) {
    unsigned char* dest = (unsigned char*)dst + count;
    const unsigned char* source = (const unsigned char*)src + count;
    unsigned long* long_dest;
    const unsigned long* long_source;
    unsigned long i;
    unsigned long first;
    unsigned long second;
    unsigned int src_offset;
    unsigned int left_shift;
    unsigned int right_shift;

    i = (unsigned long)dest & 3;

    if (i) {
        count -= i;

        do {
            *--dest = *--source;
        } while (--i);
    }

    src_offset = ((unsigned int)source) & 3;
    left_shift = src_offset << 3;
    right_shift = 32 - left_shift;
    i = count >> 3;
    long_source = (const unsigned long*)(source + (4 - src_offset));
    long_dest = (unsigned long*)dest;
    first = *--long_source;

    do {
        second = long_source[-1];
        long_dest[-1] = (second << left_shift) | (first >> right_shift);
        first = *(long_source -= 2);
        *(long_dest -= 2) = (first << left_shift) | (second >> right_shift);
    } while (--i);

    if (count & 4) {
        second = *--long_source;
        *--long_dest = (second << left_shift) | (first >> right_shift);
    }

    count &= 3;

    if (count) {
        source = (const unsigned char*)long_source + src_offset;
        dest = (unsigned char*)long_dest;

        do {
            *--dest = *--source;
        } while (--count);
    }
}
