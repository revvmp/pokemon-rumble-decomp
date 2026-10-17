#ifndef FA_PF_ENTRY_H
#define FA_PF_ENTRY_H

#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 PFENT_UpdateSFNEntry(PF_DIR_ENT* p_ent, u32 flag);
u8 PFENT_getcurrentDateTimeForEnt(u16* p_date, u16* p_time);

#ifdef __cplusplus
}
#endif

#endif
