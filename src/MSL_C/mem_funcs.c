#include "MSL_C/mem_funcs.h"

#define cps ((unsigned char*)src)
#define cpd ((unsigned char*)dst)
#define lps ((unsigned long*)src)
#define lpd ((unsigned long*)dst)
#define deref_auto_inc(p) *++(p)
#define deref_auto_dec(p) *--(p)

void __copy_longs_aligned(void* dst, const void* src, size_t n) {
    unsigned long i;

    i = (-(unsigned long)dst) & 3;

    cps = ((unsigned char*)src) - 1;
    cpd = ((unsigned char*)dst) - 1;

    if (i) {
        n -= i;
        do
            deref_auto_inc(cpd) = deref_auto_inc(cps);
        while (--i);
    }

    lps = ((unsigned long*)(cps + 1)) - 1;
    lpd = ((unsigned long*)(cpd + 1)) - 1;

    i = n >> 5;

    if (i)
        do {
            lpd[1] = lps[1];
            lpd[2] = lps[2];
            lpd[3] = lps[3];
            lpd[4] = lps[4];
            lpd[5] = lps[5];
            lpd[6] = lps[6];
            lpd[7] = lps[7];
            *(lpd += 8) = *(lps += 8);
        } while (--i);

    i = (n & 31) >> 2;

    if (i)
        do
            deref_auto_inc(lpd) = deref_auto_inc(lps);
        while (--i);

    cps = ((unsigned char*)(lps + 1)) - 1;
    cpd = ((unsigned char*)(lpd + 1)) - 1;

    n &= 3;

    if (n)
        do
            deref_auto_inc(cpd) = deref_auto_inc(cps);
        while (--n);

    return;
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

void __copy_longs_unaligned(void* dst, const void* src, size_t n) {
    unsigned long i, v1, v2;
    unsigned int src_offset, left_shift, right_shift;

    i = (-(unsigned long)dst) & 3;

    cps = ((unsigned char*)src) - 1;
    cpd = ((unsigned char*)dst) - 1;

    if (i) {
        n -= i;
        do
            deref_auto_inc(cpd) = deref_auto_inc(cps);
        while (--i);
    }

    src_offset = ((unsigned int)(cps + 1)) & 3;

    left_shift = src_offset << 3;
    right_shift = 32 - left_shift;

    cps -= src_offset;

    lps = ((unsigned long*)(cps + 1)) - 1;
    lpd = ((unsigned long*)(cpd + 1)) - 1;

    i = n >> 3;

    v1 = deref_auto_inc(lps);

    do {
        v2 = deref_auto_inc(lps);
        deref_auto_inc(lpd) = (v1 << left_shift) | (v2 >> right_shift);
        v1 = deref_auto_inc(lps);
        deref_auto_inc(lpd) = (v2 << left_shift) | (v1 >> right_shift);
    } while (--i);

    if (n & 4) {
        v2 = deref_auto_inc(lps);
        deref_auto_inc(lpd) = (v1 << left_shift) | (v2 >> right_shift);
    }

    cps = ((unsigned char*)(lps + 1)) - 1;
    cpd = ((unsigned char*)(lpd + 1)) - 1;

    n &= 3;

    if (n) {
        cps -= 4 - src_offset;
        do
            deref_auto_inc(cpd) = deref_auto_inc(cps);
        while (--n);
    }

    return;
}

void __copy_longs_rev_unaligned(void* dst, const void* src, size_t n) {
    unsigned long i, v1, v2;
    unsigned int src_offset, left_shift, right_shift;

    cps = ((unsigned char*)src) + n;
    cpd = ((unsigned char*)dst) + n;

    i = ((unsigned long)cpd) & 3;

    if (i) {
        n -= i;
        do
            deref_auto_dec(cpd) = deref_auto_dec(cps);
        while (--i);
    }

    src_offset = ((unsigned int)cps) & 3;

    left_shift = src_offset << 3;
    right_shift = 32 - left_shift;

    cps = cps + (4 - src_offset);

    i = n >> 3;

    v1 = deref_auto_dec(lps);

    do {
        v2 = deref_auto_dec(lps);
        deref_auto_dec(lpd) = (v2 << left_shift) | (v1 >> right_shift);
        v1 = deref_auto_dec(lps);
        deref_auto_dec(lpd) = (v1 << left_shift) | (v2 >> right_shift);
    } while (--i);

    if (n & 4) {
        v2 = deref_auto_dec(lps);
        deref_auto_dec(lpd) = (v2 << left_shift) | (v1 >> right_shift);
    }

    n &= 3;

    if (n) {
        cps = ((unsigned char*)lps) + src_offset;
        do
            deref_auto_dec(cpd) = deref_auto_dec(cps);
        while (--n);
    }

    return;
}
