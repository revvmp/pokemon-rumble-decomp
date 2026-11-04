#include <revolution/bte/bta_hh_int.h>

tBTA_HH_SPT_TOD p_devt_list[] = {
    {0x20, 1},
    {0x10, 2},
    {0x01, 3},
    {0x31, 2}
};

const tBTA_HH_CFG bta_hh_cfg = {4, p_devt_list, 0x400};
tBTA_HH_CFG* p_bta_hh_cfg = (tBTA_HH_CFG*)&bta_hh_cfg;
