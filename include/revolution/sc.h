#ifndef REVOLUTION_SC_H
#define REVOLUTION_SC_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SC_STATUS_OK 0

#define SC_SENSOR_BAR_BOTTOM 0
#define SC_SENSOR_BAR_TOP 1

s32 SCCheckStatus(void);
u8 SCGetBtDpdSensibility(void);
u8 SCGetWpadMotorMode(void);
u8 SCGetWpadSensorBarPosition(void);
u8 SCGetWpadSpeakerVolume(void);

#ifdef __cplusplus
}
#endif

#endif
