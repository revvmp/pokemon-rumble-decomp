.syntax unified
.thumb
.section .text.patch_connection_limit,"ax",%progbits

    movs r0, #40
    b LoadConnectionControl
    ldrh r0, [r4, #42]
LoadConnectionControl:
    ldr r3, [r4]
