#include <revolution/bte/bt_trace.h>
#include <revolution/bte/bta_hh_api.h>

#include "revolution/types.h"
#include <string.h>

#include <revolution/bte/bt_trace.h>
#include <revolution/bte/bt_types.h>

#include <revolution/bte/bd.h>
#include <revolution/bte/bta_api.h>
#include <revolution/bte/bta_hh_int.h>
#include <revolution/bte/bta_sys.h>
#include <revolution/bte/gki.h>
#include <revolution/bte/hiddefs.h>

static void bta_hh_snd_write_dev(UINT8 dev_handle, UINT8 t_type, UINT8 param, UINT16 data, UINT8 rpt_id, BT_HDR* p_data);

static tBTA_SYS_REG const bta_hh_reg = {&bta_hh_hdl_event, &BTA_HhDisable};

void BTA_HhEnable(tBTA_SEC sec_mask, tBTA_HH_CBACK* p_cback) {
    tBTA_HH_API_ENABLE* p_buf;

    GKI_sched_lock();

    bta_sys_register(BTA_ID_HH, &bta_hh_reg);

    GKI_sched_unlock();

    if ((p_buf = GKI_getbuf(sizeof *p_buf)) != NULL) {
        memset(p_buf, 0, sizeof *p_buf);

        p_buf->hdr.event = BTA_HH_API_ENABLE_EVT;
        p_buf->p_cback = p_cback;
        p_buf->sec_mask = sec_mask;

        bta_sys_sendmsg(p_buf);
    }
}

void BTA_HhDisable(void) {
    BT_HDR* p_buf;

    if ((p_buf = GKI_getbuf(sizeof *p_buf)) != NULL) {

        p_buf->event = BTA_HH_API_DISABLE_EVT;

        bta_sys_sendmsg(p_buf);
    }
}

void BTA_HhClose(UINT8 dev_handle) {
    BT_HDR* p_buf;

    if ((p_buf = GKI_getbuf(sizeof *p_buf)) != NULL) {
        memset(p_buf, 0, sizeof *p_buf);

        p_buf->event = BTA_HH_API_CLOSE_EVT;
        p_buf->layer_specific = dev_handle;

        bta_sys_sendmsg(p_buf);
    }
}

void BTA_HhOpen(BD_ADDR dev_bda, tBTA_HH_PROTO_MODE mode, tBTA_SEC sec_mask) {
    tBTA_HH_API_CONN* p_buf;

    if ((p_buf = GKI_getbuf(sizeof *p_buf)) != NULL) {
        memset(p_buf, 0, sizeof *p_buf);

        p_buf->hdr.event = BTA_HH_API_OPEN_EVT;
        p_buf->hdr.layer_specific = BTA_HH_INVALID_HANDLE;
        p_buf->sec_mask = sec_mask;
        p_buf->mode = mode;
        bdcpy(p_buf->bd_addr, dev_bda);

        bta_sys_sendmsg(p_buf);
    } else {
        if (appl_trace_level >= BT_TRACE_LEVEL_ERROR) {
            LogMsg_0(TRACE_ORG_APPL | TRACE_TYPE_ERROR, "No resource to send HID host Connect request.");
        }
    }
}

static void bta_hh_snd_write_dev(UINT8 dev_handle, UINT8 t_type, UINT8 param, UINT16 data, UINT8 rpt_id, BT_HDR* p_data) {
    tBTA_HH_CMD_DATA* p_buf;
    UINT16 len = sizeof *p_buf;

    if ((p_buf = GKI_getbuf(len)) != NULL) {
        memset(p_buf, 0, sizeof *p_buf);

        p_buf->hdr.event = BTA_HH_API_WRITE_DEV_EVT;
        p_buf->hdr.layer_specific = dev_handle;
        p_buf->t_type = t_type;
        p_buf->data = data;
        p_buf->param = param;
        p_buf->p_data = p_data;
        p_buf->rpt_id = rpt_id;

        bta_sys_sendmsg(p_buf);
    }
}

void BTA_HhSetReport(UINT8 dev_handle, tBTA_HH_RPT_TYPE r_type, BT_HDR* p_data) {
    bta_hh_snd_write_dev(dev_handle, HID_TRANS_SET_REPORT, r_type, 0, 0, p_data);
}

void BTA_HhGetReport(UINT8 dev_handle, tBTA_HH_RPT_TYPE r_type, UINT8 rpt_id, UINT16 buf_size) {
    UINT8 param = buf_size ? r_type | 0x08 : r_type;

    bta_hh_snd_write_dev(dev_handle, HID_TRANS_GET_REPORT, param, buf_size, rpt_id, NULL);
}

void BTA_HhSetProtoMode(UINT8 dev_handle, tBTA_HH_PROTO_MODE p_type) {
    bta_hh_snd_write_dev(dev_handle, HID_TRANS_SET_PROTOCOL, (UINT8)p_type, 0, 0, NULL);
}

void BTA_HhGetProtoMode(UINT8 dev_handle) {
    bta_hh_snd_write_dev(dev_handle, HID_TRANS_GET_PROTOCOL, 0, 0, 0, NULL);
}

void BTA_HhSetIdle(UINT8 dev_handle, UINT16 idle_rate) {
    bta_hh_snd_write_dev(dev_handle, HID_TRANS_SET_IDLE, 0, idle_rate, 0, NULL);
}

void BTA_HhGetIdle(UINT8 dev_handle) {
    bta_hh_snd_write_dev(dev_handle, HID_TRANS_GET_IDLE, 0, 0, 0, NULL);
}

void BTA_HhSendCtrl(UINT8 dev_handle, tBTA_HH_TRANS_CTRL_TYPE c_type) {
    bta_hh_snd_write_dev(dev_handle, HID_TRANS_CONTROL, (UINT8)c_type, 0, 0, NULL);
}

void BTA_HhSendData(UINT8 dev_handle, BT_HDR* p_data) {
    bta_hh_snd_write_dev(dev_handle, HID_TRANS_DATA, BTA_HH_RPTT_OUTPUT, 0, 0, p_data);
}

void BTA_HhGetDscpInfo(UINT8 dev_handle) {
    BT_HDR* p_buf;

    if ((p_buf = GKI_getbuf(sizeof *p_buf)) != NULL) {
        memset(p_buf, 0, sizeof *p_buf);

        p_buf->event = BTA_HH_API_GET_DSCP_EVT;
        p_buf->layer_specific = dev_handle;

        bta_sys_sendmsg(p_buf);
    }
}

void BTA_HhAddDev(BD_ADDR bda, tBTA_HH_ATTR_MASK attr_mask, UINT8 sub_class, UINT8 app_id, tHID_DEV_DSCP_INFO dscp_info) {
    tBTA_HH_MAINT_DEV* p_buf;

    if ((p_buf = GKI_getbuf(sizeof *p_buf)) != NULL) {
        memset(p_buf, 0, sizeof *p_buf);

        p_buf->hdr.event = BTA_HH_API_MAINT_DEV_EVT;
        p_buf->sub_event = BTA_HH_ADD_DEV_EVT;
        p_buf->hdr.layer_specific = BTA_HH_INVALID_HANDLE;
        p_buf->attr_mask = (UINT16)attr_mask;
        p_buf->sub_class = sub_class;
        p_buf->app_id = app_id;
        memcpy(&p_buf->dscp_info, &dscp_info, sizeof p_buf->dscp_info);
        bdcpy(p_buf->bda, bda);

        bta_sys_sendmsg(p_buf);
    }
}

void BTA_HhRemoveDev(UINT8 dev_handle) {
    tBTA_HH_MAINT_DEV* p_buf;

    if ((p_buf = GKI_getbuf(sizeof *p_buf)) != NULL) {
        memset(p_buf, 0, sizeof *p_buf);

        p_buf->hdr.event = BTA_HH_API_MAINT_DEV_EVT;
        p_buf->sub_event = BTA_HH_RMV_DEV_EVT;
        p_buf->hdr.layer_specific = dev_handle;

        bta_sys_sendmsg(p_buf);
    }
}

void BTA_HhGetAclQueueInfo(void) {
    BT_HDR* p_buf;

    if ((p_buf = GKI_getbuf(sizeof *p_buf)) != NULL) {
        memset(p_buf, 0, sizeof *p_buf);

        p_buf->event = BTA_HH_API_GET_ACL_Q_EVT;

        bta_sys_sendmsg(p_buf);
    }
}

