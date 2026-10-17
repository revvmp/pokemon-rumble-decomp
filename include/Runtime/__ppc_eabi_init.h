#ifndef PPC_EABI_INIT_H
#define PPC_EABI_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

__declspec(section ".init") extern void __init_hardware(void);
__declspec(section ".init") extern void __flush_cache(void* address, unsigned int size);
extern void __init_user(void);
extern void exit(int status);

#ifdef __cplusplus
}
#endif

#endif
