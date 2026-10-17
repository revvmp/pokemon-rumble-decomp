#pragma section code_type ".init"

#include "Runtime/__mem.h"

asm void* memcpy(void* dst, const void* src, size_t count) {
    nofralloc
    cmplwi cr1, r5, 0
    beqlr cr1
    cmplw cr1, r4, r3
    blt cr1, copy_backward
    beqlr cr1

    li r6, 0x80
    cmplw cr5, r5, r6
    blt cr5, forward_test_word_alignment
    clrlwi r9, r4, 29
    clrlwi r10, r3, 29
    subf r8, r10, r3
    dcbt 0, r4
    xor. r11, r10, r9
    bne forward_bytes
    andi. r10, r10, 7
    beq+ forward_doubles
    li r6, 8
    subf r9, r9, r6
    addi r8, r3, 0
    mtctr r9
    subf r5, r9, r5

forward_align_double:
    lbz r9, 0(r4)
    addi r4, r4, 1
    stb r9, 0(r8)
    addi r8, r8, 1
    bdnz forward_align_double

forward_doubles:
    srwi r6, r5, 5
    mtctr r6

forward_double_loop:
    lfd f1, 0(r4)
    lfd f2, 8(r4)
    lfd f3, 0x10(r4)
    lfd f4, 0x18(r4)
    addi r4, r4, 0x20
    stfd f1, 0(r8)
    stfd f2, 8(r8)
    stfd f3, 0x10(r8)
    stfd f4, 0x18(r8)
    addi r8, r8, 0x20
    bdnz forward_double_loop
    andi. r6, r5, 0x1F
    beqlr
    addi r4, r4, -1
    mtctr r6
    addi r8, r8, -1

forward_double_tail:
    lbzu r9, 1(r4)
    stbu r9, 1(r8)
    bdnz forward_double_tail
    blr

forward_test_word_alignment:
    li r6, 0x14
    cmplw cr5, r5, r6
    ble cr5, forward_bytes
    clrlwi r9, r4, 30
    clrlwi r10, r3, 30
    xor. r11, r10, r9
    bne forward_bytes
    li r6, 4
    subf r9, r9, r6
    addi r8, r3, 0
    subf r5, r9, r5
    mtctr r9

forward_align_word:
    lbz r9, 0(r4)
    addi r4, r4, 1
    stb r9, 0(r8)
    addi r8, r8, 1
    bdnz forward_align_word
    srwi r6, r5, 4
    mtctr r6

forward_word_loop:
    lwz r9, 0(r4)
    lwz r10, 4(r4)
    lwz r11, 8(r4)
    lwz r12, 0xC(r4)
    addi r4, r4, 0x10
    stw r9, 0(r8)
    stw r10, 4(r8)
    stw r11, 8(r8)
    stw r12, 0xC(r8)
    addi r8, r8, 0x10
    bdnz forward_word_loop
    andi. r6, r5, 0xF
    beqlr
    addi r4, r4, -1
    mtctr r6
    addi r8, r8, -1

forward_word_tail:
    lbzu r9, 1(r4)
    stbu r9, 1(r8)
    bdnz forward_word_tail
    blr

forward_bytes:
    addi r7, r4, -1
    addi r8, r3, -1
    mtctr r5

forward_byte_loop:
    lbzu r9, 1(r7)
    stbu r9, 1(r8)
    bdnz forward_byte_loop
    blr

copy_backward:
    add r4, r4, r5
    add r12, r3, r5
    li r6, 0x80
    cmplw cr5, r5, r6
    blt cr5, backward_test_word_alignment
    clrlwi r9, r4, 29
    clrlwi r10, r12, 29
    xor. r11, r10, r9
    bne backward_bytes
    andi. r10, r10, 7
    beq+ backward_doubles
    mtctr r10

backward_align_double:
    lbzu r9, -1(r4)
    stbu r9, -1(r12)
    bdnz backward_align_double

backward_doubles:
    subf r5, r10, r5
    srwi r6, r5, 5
    mtctr r6

backward_double_loop:
    lfd f1, -8(r4)
    lfd f2, -0x10(r4)
    lfd f3, -0x18(r4)
    lfd f4, -0x20(r4)
    addi r4, r4, -0x20
    stfd f1, -8(r12)
    stfd f2, -0x10(r12)
    stfd f3, -0x18(r12)
    stfdu f4, -0x20(r12)
    bdnz backward_double_loop
    andi. r6, r5, 0x1F
    beqlr
    mtctr r6

backward_double_tail:
    lbzu r9, -1(r4)
    stbu r9, -1(r12)
    bdnz backward_double_tail
    blr

backward_test_word_alignment:
    li r6, 0x14
    cmplw cr5, r5, r6
    ble cr5, backward_bytes
    clrlwi r9, r4, 30
    clrlwi r10, r12, 30
    xor. r11, r10, r9
    bne backward_bytes
    andi. r10, r10, 7
    beq+ backward_words
    mtctr r10

backward_align_word:
    lbzu r9, -1(r4)
    stbu r9, -1(r12)
    bdnz backward_align_word

backward_words:
    subf r5, r10, r5
    srwi r6, r5, 4
    mtctr r6

backward_word_loop:
    lwz r9, -4(r4)
    lwz r10, -8(r4)
    lwz r11, -0xC(r4)
    lwz r8, -0x10(r4)
    addi r4, r4, -0x10
    stw r9, -4(r12)
    stw r10, -8(r12)
    stw r11, -0xC(r12)
    stwu r8, -0x10(r12)
    bdnz backward_word_loop
    andi. r6, r5, 0xF
    beqlr
    mtctr r6

backward_word_tail:
    lbzu r9, -1(r4)
    stbu r9, -1(r12)
    bdnz backward_word_tail
    blr

backward_bytes:
    mtctr r5

backward_byte_loop:
    lbzu r9, -1(r4)
    stbu r9, -1(r12)
    bdnz backward_byte_loop
    blr
}

void __fill_mem(void* dst, int value, unsigned long count) {
    unsigned long fill = (unsigned char)value;
    unsigned long i;

    ((unsigned char*)dst) = (unsigned char*)dst - 1;

    if (count >= 32) {
        i = ~(unsigned long)dst & 3;

        if (i) {
            count -= i;

            do {
                *++((unsigned char*)dst) = fill;
            } while (--i);
        }

        if (fill != 0) {
            fill |= fill << 24 | fill << 16 | fill << 8;
        }

        ((unsigned long*)dst) = (unsigned long*)((unsigned char*)dst + 1) - 1;
        i = count >> 5;

        if (i != 0) {
            do {
                *++((unsigned long*)dst) = fill;
                *++((unsigned long*)dst) = fill;
                *++((unsigned long*)dst) = fill;
                *++((unsigned long*)dst) = fill;
                *++((unsigned long*)dst) = fill;
                *++((unsigned long*)dst) = fill;
                *++((unsigned long*)dst) = fill;
                *++((unsigned long*)dst) = fill;
            } while (--i);
        }

        i = (count & 31) >> 2;

        if (i) {
            do {
                *++((unsigned long*)dst) = fill;
            } while (--i);
        }

        ((unsigned char*)dst) = (unsigned char*)((unsigned long*)dst + 1) - 1;
        count &= 3;
    }

    if (count) {
        do {
            *++((unsigned char*)dst) = fill;
        } while (--count);
    }
}

void* memset(void* dst, int value, size_t count) {
    __fill_mem(dst, value, count);
    return dst;
}

#pragma section code_type ".text"

__declspec(weak) size_t strlen(const char* str) {
    size_t length = -1;
    const unsigned char* p = (const unsigned char*)str - 1;

    do {
        length++;
    } while (*++p);

    return length;
}
