.extern FirmwareGreaterOrEqual, FirmwareNotEqual
.syntax unified
.thumb
.section .text.patch_compare,"ax",%progbits

    cmp r4, r0
    bhs FirmwareGreaterOrEqual
    cmp r4, r0
    bne FirmwareNotEqual
