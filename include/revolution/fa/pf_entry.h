#ifndef FA_PF_ENTRY_H
#define FA_PF_ENTRY_H

#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 PFENT_IsNotMatchedEntry(PF_DIR_ENT* p_ent, const u8* buf, PF_STR* p_pattern, u8 attr, u8 attr_required, u8 attr_excluded,
                            u32* p_num_skipped);
s32 PFENT_LoadMatchedEntry(PF_DIR_ENT* p_ent, PF_ENT_ITER* p_iter, PF_STR* p_pattern, u8 attr_required, u8 attr_excluded,
                           u32* p_num_skipped);
s32 PFENT_searchEmptyTailSFN(PF_FFD* p_ffd, u32 tail_index, const s8* pattern, u32* p_tail_bit);
s32 PFENT_findEmptyTailSFN(PF_DIR_ENT* p_ent_containing_dir, const s8* name, u32* p_tails);
u8 PFENT_CalcCheckSum(PF_DIR_ENT* p_ent);
void PFENT_LoadShortNameFromBuf(PF_DIR_ENT* p_ent, const u8* buf);
void PFENT_loadEntryNumericFieldsFromBuf(PF_DIR_ENT* p_ent, const u8* buf);
void PFENT_StoreEntryNumericFieldsToBuf(u8* buf, const PF_DIR_ENT* p_ent);
s32 PFENT_LoadLFNEntryFieldsFromBuf(PF_DIR_ENT* p_ent, const u8* buf);
void PFENT_storeLFNEntryFieldsToBuf(u8* buf, PF_DIR_ENT* p_ent, u8 ord, u8 sum, u32 is_last);
s32 PFENT_findEntry(PF_FFD* p_ffd, PF_DIR_ENT* p_ent, u32 index_search_from, PF_STR* p_pattern, u8 attr_required, u8 attr_excluded,
                    u32* p_lpos, u32* p_ppos);
s32 PFENT_SearchEntry(PF_FFD* p_ffd, PF_DIR_ENT* p_ent, u32 index_search_from, PF_STR* p_pattern, u8 attr_required, u8 attr_excluded);
s32 PFENT_allocateEntry(PF_DIR_ENT* p_ent, u8 num_entries, PF_FFD* p_ffd, u32* p_prev_chain, PF_STR* p_filename, u32* p_pos);
s32 PFENT_AllocateEntry(PF_DIR_ENT* p_ent, u8 num_entries, PF_FFD* p_ffd, u32* p_prev_chain, PF_STR* p_filename);
s32 PFENT_GetRootDir(PF_VOLUME* p_vol, PF_DIR_ENT* p_ent);
s32 PFENT_MakeRootDir(PF_VOLUME* p_vol);
s32 PFENT_UpdateSFNEntry(PF_DIR_ENT* p_ent, u32 flag);
s32 PFENT_AdjustSFN(PF_DIR_ENT* p_ent, s8* p_short_name);
s32 PFENT_RemoveEntry(PF_DIR_ENT* p_ent, PF_ENT_ITER* p_iter);
u8 PFENT_getcurrentDateTimeForEnt(u16* p_date, u16* p_time);

#ifdef __cplusplus
}
#endif

#endif
