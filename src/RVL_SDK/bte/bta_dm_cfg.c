#include <revolution/bte/bta_api.h>
#include <revolution/bte/bta_dm_int.h>
#include <revolution/bte/bta_sys.h>
#include <revolution/bte/btm_api.h>
#include <revolution/bte/hcidefs.h>

const tBTA_DM_CFG bta_dm_cfg = {
    {0x40, 0x02, 0x04},
    HCI_ENABLE_MASTER_SLAVE_SWITCH | HCI_ENABLE_HOLD_MODE | HCI_ENABLE_SNIFF_MODE | HCI_ENABLE_PARK_MODE,
    8192,
    5000
};

const tBTA_DM_RM bta_dm_rm_cfg[] = {
    {BTA_ID_SYS, 2, BTA_DM_PARTIAL_SCATTERNET},
    {14, 1, BTA_MASTER_ROLE_ONLY},
    {14, 2, BTA_MASTER_ROLE_ONLY},
    {18, 255, BTA_MASTER_ROLE_PREF}
};

const tBTA_DM_COMPRESS bta_dm_compress_cfg[] = {
    {0, 5, 0},
    {10, 255, 1},
    {8, 255, 1},
    {6, 255, 1},
    {7, 255, 1},
    {18, 255, 2}
};

const tBTA_DM_PM_CFG bta_dm_pm_cfg[] = {
    {BTA_ID_SYS, 0, 0}
};

const tBTA_DM_PM_SPEC bta_dm_pm_spec[] = {
    {3, {
        {{2, 5000}, {BTA_DM_PM_NO_ACTION, 0}},
        {{16, 0}, {BTA_DM_PM_NO_ACTION, 0}},
        {{BTA_DM_PM_NO_ACTION, 0}, {BTA_DM_PM_NO_ACTION, 0}},
        {{BTA_DM_PM_NO_ACTION, 0}, {BTA_DM_PM_NO_ACTION, 0}},
        {{BTA_DM_PM_NO_ACTION, 0}, {BTA_DM_PM_NO_ACTION, 0}},
        {{2, 5000}, {BTA_DM_PM_NO_ACTION, 0}},
        {{BTA_DM_PM_NO_ACTION, 0}, {BTA_DM_PM_NO_ACTION, 0}},
        {{BTA_DM_PM_NO_ACTION, 0}, {BTA_DM_PM_NO_ACTION, 0}},
        {{8, 5000}, {BTA_DM_PM_NO_ACTION, 0}}
    }}
};

const tBTM_PM_PWR_MD bta_dm_pm_md[] = {
    {400, 200, 4, 4, BTM_PM_MD_SNIFF},
    {800, 400, 0, 0, BTM_PM_MD_PARK}
};

tBTA_DM_RM* p_bta_dm_rm_cfg = (tBTA_DM_RM*)bta_dm_rm_cfg;
tBTA_DM_COMPRESS* p_bta_dm_compress_cfg = (tBTA_DM_COMPRESS*)bta_dm_compress_cfg;
tBTA_DM_PM_CFG* p_bta_dm_pm_cfg = (tBTA_DM_PM_CFG*)bta_dm_pm_cfg;
tBTA_DM_PM_SPEC* p_bta_dm_pm_spec = (tBTA_DM_PM_SPEC*)bta_dm_pm_spec;
tBTM_PM_PWR_MD* p_bta_dm_pm_md = (tBTM_PM_PWR_MD*)bta_dm_pm_md;
