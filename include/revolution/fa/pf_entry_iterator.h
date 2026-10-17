#ifndef FA_PF_ENTRY_ITERATOR_H
#define FA_PF_ENTRY_ITERATOR_H

#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 PFENT_ITER_IteratorInitialize(PF_ENT_ITER* p_iter, u32 index_start_from);
u32 PFENT_ITER_IsAtLogicalEnd(PF_ENT_ITER* p_iter);
s32 PFENT_ITER_MoveTo(PF_ENT_ITER* p_iter, u32 index, u32 may_allocate);
s32 PFENT_ITER_Advance(PF_ENT_ITER* p_iter, u32 may_allocate);

#ifdef __cplusplus
}
#endif

#endif
