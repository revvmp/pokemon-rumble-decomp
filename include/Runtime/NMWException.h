#ifndef NMWEXCEPTION_H
#define NMWEXCEPTION_H

#include "Runtime/__ppc_eabi_linker.h"

#ifdef __cplusplus
extern "C" {
#endif

int __register_fragment(struct __eti_init_info* info, char* toc);
void __unregister_fragment(int fragment);

#ifdef __cplusplus
}
#endif

#endif
