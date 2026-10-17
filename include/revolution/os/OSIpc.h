#ifndef REVOLUTION_OS_OSIPC_H
#define REVOLUTION_OS_OSIPC_H

#ifdef __cplusplus
extern "C" {
#endif

void* __OSGetIPCBufferHi(void);
void* __OSGetIPCBufferLo(void);
void __OSInitIPCBuffer(void);

#ifdef __cplusplus
}
#endif

#endif
