#ifndef FA_PF_ATTACH_H
#define FA_PF_ATTACH_H

#include <revolution/fa/pf_volume.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PF_NAND_FILE {
    char path[78];
    u32 size;
} PF_NAND_FILE;

int pf2_attach(PF_DRV_TBL** pp_driver_table, const PF_NAND_FILE* p_nand_file);

#ifdef __cplusplus
}
#endif

#endif
