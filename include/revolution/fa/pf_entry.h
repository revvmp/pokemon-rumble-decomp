#ifndef FA_PF_ENTRY_H
#define FA_PF_ENTRY_H

#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

u8 PFENT_CalcCheckSum(PF_DIR_ENT* p_ent);
void PFENT_StoreEntryNumericFieldsToBuf(u8* buf, const PF_DIR_ENT* p_ent);
void PFENT_storeLFNEntryFieldsToBuf(u8* buf, PF_DIR_ENT* p_ent, u8 ord, u8 sum, u32 is_last);
s32 PFENT_SearchEntry(PF_FFD* p_ffd, PF_DIR_ENT* p_ent, u32 index_search_from, PF_STR* p_pattern, u8 attr_required, u32 is_short_search);
s32 PFENT_allocateEntry(PF_DIR_ENT* p_ent, u8 num_entries, PF_FFD* p_ffd, u32* p_prev_chain, PF_STR* p_filename, u32* p_pos);
s32 PFENT_GetRootDir(PF_VOLUME* p_vol, PF_DIR_ENT* p_ent);
s32 PFENT_LookupPath(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_VOLUME* p_vol, PF_STR* p_path, u32 is_parent, u32 wildcard);
s32 PFENT_GetEntryOfPath(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_VOLUME* p_vol, PF_STR* p_path, u32 wildcard);
s32 PFENT_GetParentEntryOfPath(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_VOLUME* p_vol, PF_STR* p_path);
s32 PFENT_findEntry(PF_FFD* p_ffd, PF_DIR_ENT* p_ent, u32 index_search_from, PF_STR* p_pattern, u8 attr_required, u32 is_short_search,
                    u32* p_lpos, u32* p_ppos);
s32 PFENT_UpdateSFNEntry(PF_DIR_ENT* p_ent, u32 flag);
s32 PFENT_AdjustSFN(PF_DIR_ENT* p_ent, s8* p_short_name);
s32 PFENT_RemoveEntry(PF_DIR_ENT* p_ent, PF_ENT_ITER* p_iter);
u8 PFENT_getcurrentDateTimeForEnt(u16* p_date, u16* p_time);

#ifdef __cplusplus
}
#endif

#endif
