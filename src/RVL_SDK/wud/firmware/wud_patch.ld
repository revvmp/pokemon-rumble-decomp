OUTPUT_FORMAT(elf32-littlearm)
OUTPUT_ARCH(arm)
ENTRY(UpdateConnectionControl)
ContinueConnectionUpdate = 0x00011255;
FirmwareGreaterOrEqual = 0x000165d5;
FirmwareNotEqual = 0x000165d9;
FirmwareStateRejected = 0x0001a0d1;
ReadDeviceState = 0x0000a7b9;
ResumeConnectionControl = 0x00005a95;
ResumeControllerReset = 0x00002f79;
ResumeDeviceState = 0x00006111;
ResumeFirmwareByteState = 0x0001a0d1;
SetFirmwareOption = 0x00020905;
SECTIONS
{
    .text.patch_controller_reset 0x00002f74 : { *(.text.patch_controller_reset) }
    .text.patch_connection_control 0x00005a84 : { *(.text.patch_connection_control) }
    .text.patch_device_state 0x00006100 : { *(.text.patch_device_state) }
    .text.patch_connection_branch 0x00011204 : { *(.text.patch_connection_branch) }
    .text.patch_connection_limit 0x0001623c : { *(.text.patch_connection_limit) }
    .text.patch_compare 0x000165bc : { *(.text.patch_compare) }
    .text.patch_firmware_byte_state 0x00019fcc : { *(.text.patch_firmware_byte_state) }
    .text.controller_patch 0x00089970 : { *(.text.controller_patch) }
}
