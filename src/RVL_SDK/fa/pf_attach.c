#include <revolution/fa/pf_attach.h>
#include <revolution/fa/pf_api_util.h>
#include <revolution/fa/pf_volume.h>
#include <revolution/fa/pf_str.h>
#include <stddef.h>

int pf2_attach(PF_DRV_TBL** pp_driver_table, const PF_NAND_FILE* p_nand_file) {
    s32 err;

    if (pp_driver_table == NULL || *pp_driver_table == NULL) {
        err = 10;
        pf_vol_set.last_error = err;
    } else {
        while (*pp_driver_table != NULL) {
            if (p_nand_file != NULL) {
                err = PFVOL_attach(*pp_driver_table, p_nand_file->path, p_nand_file->size);
            } else {
                err = PFVOL_attach(*pp_driver_table, NULL, 0);
            }
            if (err != 0) {
                return PFAPI_convertReturnValue(err);
            }
            ++pp_driver_table;
        }
    }
    return PFAPI_convertReturnValue(err);
}
