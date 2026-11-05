.extern ContinueConnectionUpdate
.syntax unified
.thumb
.section .text.patch_connection_branch,"ax",%progbits

    adds r0, r4, #0
    adds r0, r4, #0
    b ContinueConnectionUpdate
    movs r1, #176
