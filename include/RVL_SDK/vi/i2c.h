#ifndef RVL_SDK_VI_I2C_H
#define RVL_SDK_VI_I2C_H

typedef unsigned char u8;
typedef unsigned long u32;
typedef signed long s32;
typedef signed long long OSTime;
typedef int BOOL;

#define TRUE 1
#define OSTicksToMicroseconds(ticks) (((ticks) * 8) / 486)

OSTime __OSGetSystemTime(void);
BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL enabled);
void WaitMicroTime(s32 usec);
s32 __VISendI2CData(u8 slaveAddr, u8* pData, s32 nBytes);

#endif
