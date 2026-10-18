#ifndef FA_PF_SETTAILBUF_H
#define FA_PF_SETTAILBUF_H

#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

int pf2_settailbuf(char drive, u32 size, PF_TAIL_BUF* p_buffer);

#ifdef __cplusplus
}
#endif

#endif
