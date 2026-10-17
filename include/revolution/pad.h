#ifndef REVOLUTION_PAD_H
#define REVOLUTION_PAD_H

#include "revolution/os.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PAD_SPEC_0 0
#define PAD_SPEC_1 1
#define PAD_SPEC_2 2
#define PAD_SPEC_3 3
#define PAD_SPEC_4 4
#define PAD_SPEC_5 5

#define PAD_MOTOR_STOP 0
#define PAD_MOTOR_RUMBLE 1
#define PAD_MOTOR_STOP_HARD 2

#define PAD_CHAN0_BIT 0x80000000
#define PAD_CHAN1_BIT 0x40000000
#define PAD_CHAN2_BIT 0x20000000
#define PAD_CHAN3_BIT 0x10000000

#define PAD_MAX_CONTROLLERS 4

#define PAD_BUTTON_LEFT 0x0001
#define PAD_BUTTON_RIGHT 0x0002
#define PAD_BUTTON_DOWN 0x0004
#define PAD_BUTTON_UP 0x0008
#define PAD_TRIGGER_Z 0x0010
#define PAD_TRIGGER_R 0x0020
#define PAD_TRIGGER_L 0x0040
#define PAD_BUTTON_A 0x0100
#define PAD_BUTTON_B 0x0200
#define PAD_BUTTON_X 0x0400
#define PAD_BUTTON_Y 0x0800
#define PAD_BUTTON_MENU 0x1000
#define PAD_BUTTON_START 0x1000

#define PAD_ERR_NONE 0
#define PAD_ERR_NO_CONTROLLER -1
#define PAD_ERR_NOT_READY -2
#define PAD_ERR_TRANSFER -3

#define PAD_STICK_CLAMP_OCTA_WITH_MARGIN 0
#define PAD_STICK_CLAMP_OCTA_WITHOUT_MARGIN 1
#define PAD_STICK_CLAMP_CIRCLE_WITH_MARGIN 2
#define PAD_STICK_CLAMP_CIRCLE_WITHOUT_MARGIN 3

#define PAD_TRIGGER_FIXED_BASE 0
#define PAD_TRIGGER_INDIVIDUAL_BASE 1

typedef struct PADStatus {
    u16 button;
    s8 stickX;
    s8 stickY;
    s8 substickX;
    s8 substickY;
    u8 triggerLeft;
    u8 triggerRight;
    u8 analogA;
    u8 analogB;
    s8 err;
} PADStatus;

typedef struct PADClampRegion {
    u8 minTrigger;
    u8 maxTrigger;
    s8 minStick;
    s8 maxStick;
    s8 xyStick;
    s8 minSubstick;
    s8 maxSubstick;
    s8 xySubstick;
    s8 radStick;
    s8 radSubstick;
} PADClampRegion;

typedef void (*PADSamplingCallback)(void);

BOOL PADReset(u32 mask);
BOOL PADRecalibrate(u32 mask);
BOOL PADInit(void);
u32 PADRead(PADStatus* status);
void PADSetSamplingRate(u32 msec);
void PADControlAllMotors(const u32* commandArray);
void PADControlMotor(s32 chan, u32 command);
void PADSetSpec(u32 spec);
u32 PADGetSpec(void);
BOOL PADGetType(s32 chan, u32* type);
BOOL PADSync(void);
void PADSetAnalogMode(u32 mode);
BOOL __PADDisableRecalibration(BOOL disable);
BOOL __PADDisableRumble(BOOL disable);
BOOL PADIsBarrel(s32 chan);
void __PADDisableXPatch(void);
PADSamplingCallback PADSetSamplingCallback(PADSamplingCallback callback);

void PADClamp(PADStatus* status);
void PADClampCircle(PADStatus* status);
void PADClamp2(PADStatus* status, u32 type);
void PADClampCircle2(PADStatus* status, u32 type);
void PADClampTrigger(PADStatus* status, u32 type);

extern u32 __PADSpec;
extern u32 __PADFixBits;

#ifdef __cplusplus
}
#endif

#endif
