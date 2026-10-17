#ifndef NMWEXCEPTION_H
#define NMWEXCEPTION_H

#include "Runtime/__ppc_eabi_linker.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef short vbase_ctor_arg_type;
typedef char local_cond_type;

typedef struct CatchInfo {
    void* location;
    void* typeinfo;
    void* dtor;
    void* sublocation;
    long pointercopy;
    void* stacktop;
} CatchInfo;

void __throw(char* throwtype, void* location, void* dtor);
void __end__catch(CatchInfo* catchinfo);
void __unexpected(CatchInfo* catchinfo);
char __throw_catch_compare(const char* throwtype, const char* catchtype, long* offset_result);

int __register_fragment(struct __eti_init_info* info, char* toc);
void __unregister_fragment(int fragment);

#ifdef __cplusplus
}
#endif

#endif
