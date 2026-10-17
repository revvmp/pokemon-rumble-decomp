#ifndef RVL_SDK_IPC_MAIN_H
#define RVL_SDK_IPC_MAIN_H

typedef unsigned char u8;
typedef unsigned long u32;

void IPCInit(void);
void IPCReInit(void);
u32 IPCReadReg(u32 regIdx);
void IPCWriteReg(u32 regIdx, u32 data);
void* IPCGetBufferHi(void);
void* IPCGetBufferLo(void);
void IPCSetBufferLo(void* lo);

#endif
