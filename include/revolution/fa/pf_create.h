#ifndef FA_PF_CREATE_H
#define FA_PF_CREATE_H

#include <revolution/types.h>
#include <revolution/fa/pf_file.h>

#ifdef __cplusplus
extern "C" {
#endif

PF_FILE* pf2_create(const char* path, int mode);

#ifdef __cplusplus
}
#endif

#endif
