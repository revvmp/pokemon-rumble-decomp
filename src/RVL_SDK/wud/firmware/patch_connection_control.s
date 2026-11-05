.extern UpdateConnectionControl
.syntax unified
.thumb
.section .text.patch_connection_control,"ax",%progbits

    bl UpdateConnectionControl
    lsrs r1, r1, #16
    orrs r0, r1
