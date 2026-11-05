.extern FirmwareStateRejected, UpdateFirmwareByteState
.syntax unified
.thumb
.section .text.patch_firmware_byte_state,"ax",%progbits

    bl UpdateFirmwareByteState
    cmp r0, #3
    bne FirmwareStateRejected
