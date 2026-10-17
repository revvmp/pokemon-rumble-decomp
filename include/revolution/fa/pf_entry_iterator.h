#ifndef FA_PF_ENTRY_ITERATOR_H
#define FA_PF_ENTRY_ITERATOR_H

#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 PFENT_ITER_RecalcEntryIterator(PF_ENT_ITER* p_iter, u32 may_allocate);
s32 PFENT_ITER_DoFindEntry(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_STR* p_pattern, u8 attr_required, u8 attr_excluded, u32* p_is_found);
s32 PFENT_LookupPath(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_VOLUME* p_vol, PF_STR* p_path, u32 wildcard, u32 is_parent);
s32 PFENT_ITER_IteratorInitialize(PF_ENT_ITER* p_iter, u32 index_start_from);
u32 PFENT_ITER_IsAtLogicalEnd(PF_ENT_ITER* p_iter);
s32 PFENT_ITER_MoveTo(PF_ENT_ITER* p_iter, u32 index, u32 may_allocate);
s32 PFENT_ITER_Advance(PF_ENT_ITER* p_iter, u32 may_allocate);
s32 PFENT_ITER_IsInDirectory(PF_DIR_ENT* p_ent, u32 cluster, u32* p_is_found);
s32 PFENT_ITER_Retreat(PF_ENT_ITER* p_iter, u32 may_allocate);
s32 PFENT_ITER_FindCluster(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, u32 cluster, u32* p_is_found);
s32 PFENT_ITER_FindEntry(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_STR* p_pattern, u8 attr_required, u32 wildcard, u32* p_is_found);
s32 PFENT_GetEntryOfPath(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_VOLUME* p_vol, PF_STR* p_path, u32 wildcard);
s32 PFENT_GetParentEntryOfPath(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_VOLUME* p_vol, PF_STR* p_path);
s32 PFENT_ITER_GetEntry(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent);
s32 PFENT_ITER_LoadLongName(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent);
s32 PFENT_ITER_FindEntryOfPath(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_STR* p_pattern, u8 attr_required, u32 wildcard, u32* p_is_found);

#ifdef __cplusplus
}
#endif

#endif
