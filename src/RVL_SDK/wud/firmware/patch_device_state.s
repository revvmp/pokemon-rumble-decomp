.extern UpdateDeviceState
.syntax unified
.thumb
.section .text.patch_device_state,"ax",%progbits

    bl UpdateDeviceState
    movs r0, r0
    movs r0, r0
