.extern ReadDeviceState, ResumeConnectionControl, ResumeControllerReset, ResumeDeviceState, ResumeFirmwareByteState, SetFirmwareOption
.syntax unified
.thumb
.section .text.controller_patch,"ax",%progbits

.equ ConnectionTimeout, 0x0c80
.equ FirmwareByteState, 0x00081dd8
.equ ControllerResetWord, 0x000e8024
.equ ControllerResetControl, 0x000e8000
.equ ControllerResetCommand, 0xfe0f0381
.equ ControllerResetPort, 0x000f005c
.equ ControllerControlBit0, 0x000efc60
.equ ControllerControlBits13And14, 0x000efcfc
.equ ControllerControlBit7, 0x000efc30

.global UpdateConnectionControl
.type UpdateConnectionControl, %function
.thumb_func
UpdateConnectionControl:
    bics r0, r1
    lsls r1, r2, #31
    lsrs r1, r1, #16
    orrs r0, r1
    str r0, [r4, #40]
    movs r3, #(ConnectionTimeout >> 7)
    lsls r3, r3, #7
    strh r3, [r6]
    bl ResumeConnectionControl

.global UpdateDeviceState
.type UpdateDeviceState, %function
.thumb_func
UpdateDeviceState:
    strb r0, [r5, #24]
    bl ReadDeviceState
    ldrb r3, [r0, #1]
    strb r3, [r5, #3]
    movs r3, #(ConnectionTimeout >> 7)
    lsls r3, r3, #7
    strh r3, [r6, #56]
    bl ResumeDeviceState

.global UpdateFirmwareByteState
.type UpdateFirmwareByteState, %function
.thumb_func
UpdateFirmwareByteState:
    str r3, [r1]
    ldrb r3, [r4, #14]
    ldr r1, FirmwareByteStateAddress
    str r3, [r1]
    bl ResumeFirmwareByteState

FirmwareByteStateAddress:
    .word FirmwareByteState

.global PrepareControllerReset
.type PrepareControllerReset, %function
.thumb_func
PrepareControllerReset:
    bl ResetControllerRegisters
    movs r3, #0
    bl ResumeControllerReset
    movs r0, r0

.global ResetControllerRegisters
.type ResetControllerRegisters, %function
.thumb_func
ResetControllerRegisters:
    push {lr}
    movs r3, #0
    ldr r1, ControllerResetWordAddress
    str r3, [r1]
    movs r1, #(ControllerResetControl >> 15)
    lsls r1, r1, #15
    str r3, [r1]
    movs r0, #125
    lsls r0, r0, #6

WaitResetDelay:
    subs r0, #1
    bne WaitResetDelay
    ldr r3, ControllerResetCommandValue
    ldr r2, ControllerResetPortAddress
    str r3, [r2]
    movs r0, #71
    movs r1, #0
    bl SetFirmwareOption
    movs r0, #70
    movs r1, #0
    bl SetFirmwareOption
    ldr r2, ControllerControlBit0Address
    ldr r3, [r2]
    ldr r0, KeepLow16ExceptBit0
    ands r3, r0
    str r3, [r2]
    ldr r2, ControllerControlBits13And14Address
    ldr r3, [r2]
    ldr r0, KeepLow16ExceptBits13And14
    ands r3, r0
    str r3, [r2]
    ldr r2, ControllerControlBit7Address
    ldr r3, [r2]
    ldr r0, KeepLow16ExceptBit7
    ands r3, r0
    str r3, [r2]
    pop {pc}

ControllerResetWordAddress:
    .word ControllerResetWord
ControllerResetCommandValue:
    .word ControllerResetCommand
ControllerResetPortAddress:
    .word ControllerResetPort
ControllerControlBit0Address:
    .word ControllerControlBit0
KeepLow16ExceptBit0:
    .word 0x0000ffff & ~(1 << 0)
ControllerControlBits13And14Address:
    .word ControllerControlBits13And14
KeepLow16ExceptBits13And14:
    .word 0x0000ffff & ~((1 << 13) | (1 << 14))
ControllerControlBit7Address:
    .word ControllerControlBit7
KeepLow16ExceptBit7:
    .word 0x0000ffff & ~(1 << 7)
