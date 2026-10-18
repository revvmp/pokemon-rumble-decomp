#ifndef FA_PF_CDELETE_H
#define FA_PF_CDELETE_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

u32 pf2_cdelete(const char* path, u32 offset, u32 num_clusters);

#ifdef __cplusplus
}
#endif

#endif
