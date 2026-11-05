.extern PrepareControllerReset
.syntax unified
.thumb
.section .text.patch_controller_reset,"ax",%progbits

.equ FirmwareResetStateAddress, 0x00003000

    bl PrepareControllerReset
    ldr r7, [pc, #(FirmwareResetStateAddress - 0x00002f7c)]
    str r3, [r7]
