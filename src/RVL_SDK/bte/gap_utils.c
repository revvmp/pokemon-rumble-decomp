#include <revolution/bte/gap_int.h>

static void btm_cback(UINT16 index, void* p_data);

tGAP_CB gap_cb;

static void btm_cback(UINT16 index, void* p_data) {
    tGAP_INFO* p_cb;
    tGAP_INQ_CMPL inq_cmpl;

    if (index >= GAP_MAX_BLOCKS)
        return;

    if (!gap_cb.blk[index].in_use)
        return;

    p_cb = &gap_cb.blk[index];

    switch (p_cb->event) {
    case GAP_EVT_INQUIRY_COMPLETE:
        inq_cmpl.num_results = ((tBTM_INQUIRY_CMPL*)p_data)->num_resp;
        inq_cmpl.status = (((tBTM_INQUIRY_CMPL*)p_data)->status == BTM_SUCCESS) ? BT_PASS : GAP_ERR_PROCESSING;

        p_data = &inq_cmpl;

        GAP_TRACE_EVENT2("   GAP Inquiry Complete Event (Status 0x%04x, Result(s) %d)", inq_cmpl.status,
                         inq_cmpl.num_results);
        break;

    case GAP_EVT_DISCOVERY_COMPLETE:
        if (*(UINT16*)p_data) {
            GAP_TRACE_EVENT1("   GAP Discovery Complete Event(SDP Result: 0x%04x)", *(UINT16*)p_data);
        } else {
            GAP_TRACE_EVENT0("   GAP Discovery Successfully Completed");
        }
        break;

    case GAP_EVT_REM_NAME_COMPLETE:
        ((tGAP_REMOTE_DEV_NAME*)p_data)->status =
            gap_convert_btm_status(((tBTM_REMOTE_DEV_NAME*)p_data)->status);

        GAP_TRACE_EVENT1("   GAP Remote Name Complete Event (status 0x%04x)", ((tGAP_REMOTE_DEV_NAME*)p_data)->status);
        break;
    }

    if (p_cb->gap_cback)
        (*p_cb->gap_cback)(p_cb->event, p_data);

    gap_free_cb(p_cb);
}

void gap_btm_cback0(void* p1) {
    btm_cback(0, p1);
}

void gap_btm_cback1(void* p1) {
    btm_cback(1, p1);
}

void gap_inq_results_cb(tBTM_INQ_RESULTS* p_results) {
    tGAP_INFO* p_cb;
    UINT8 index;

    GAP_TRACE_EVENT6("GAP Inquiry Results Callback (bdaddr [%02x%02x%02x%02x%02x%02x])", p_results->remote_bd_addr[0],
                     p_results->remote_bd_addr[1], p_results->remote_bd_addr[2], p_results->remote_bd_addr[3],
                     p_results->remote_bd_addr[4], p_results->remote_bd_addr[5]);
    GAP_TRACE_EVENT4("                             (COD [%02x%02x%02x], clkoff 0x%04x)", p_results->dev_class[0],
                     p_results->dev_class[1], p_results->dev_class[2], p_results->clock_offset);

    for (index = 0, p_cb = &gap_cb.blk[0]; index < GAP_MAX_BLOCKS; index++, p_cb++) {
        if (p_cb->in_use && p_cb->event == GAP_EVT_INQUIRY_COMPLETE) {
            if (p_cb->gap_inq_rslt_cback)
                (*p_cb->gap_inq_rslt_cback)(GAP_EVT_INQUIRY_RESULTS, p_results);
        }
    }
}

void gap_find_addr_name_cb(tBTM_REMOTE_DEV_NAME* p) {
    tGAP_FINDADDR_CB* p_cb = &gap_cb.findaddr_cb;
    tGAP_FINDADDR_RESULTS* p_result = &p_cb->results;

    if (!p_cb->in_use)
        return;

    if (p->status == BTM_SUCCESS) {
        GAP_TRACE_EVENT2("   GAP: FindAddrByName Rem Name Cmpl Evt (Status 0x%04x, Name [%s])", p->status,
                         p->remote_bd_name);

        if (!strncmp((char*)p_result->devname, (char*)p->remote_bd_name, strlen((char*)p_result->devname))) {
            memcpy(p_result->bd_addr, p_cb->p_cur_inq->results.remote_bd_addr, BD_ADDR_LEN);
            p_result->status = BT_PASS;
        } else {
            if ((p_cb->p_cur_inq = BTM_InqDbNext(p_cb->p_cur_inq)) != NULL) {
                if (BTM_ReadRemoteDeviceName(p_cb->p_cur_inq->results.remote_bd_addr,
                                             (tBTM_CMPL_CB*)&gap_find_addr_name_cb) == BTM_CMD_STARTED) {
                    return;
                } else {
                    p_result->status = gap_convert_btm_status((tBTM_STATUS)p->status);
                }
            } else {
                p_result->status = GAP_EOINQDB;
            }
        }
    } else {
        GAP_TRACE_EVENT1("   GAP: FindAddrByName Rem Name Cmpl Evt (Status 0x%04x)", p->status);
        p_result->status = gap_convert_btm_status((tBTM_STATUS)p->status);
    }

    if (p_cb->p_cback)
        (*p_cb->p_cback)(GAP_EVT_FIND_ADDR_COMPLETE, p_result);

    p_cb->in_use = FALSE;
    p_cb->p_cback = NULL;
}

void gap_find_addr_inq_cb(tBTM_INQUIRY_CMPL* p) {
    tGAP_FINDADDR_CB* p_cb = &gap_cb.findaddr_cb;
    tGAP_FINDADDR_RESULTS* p_result = &p_cb->results;

    if (!p_cb->in_use)
        return;

    GAP_TRACE_EVENT2("   GAP: FindAddrByName Inq Cmpl Evt (Status 0x%04x, Result(s) %d)", p->status, p->num_resp);

    if (p->status == BTM_SUCCESS) {
        if ((p_result->status = gap_find_local_addr_by_name(p_result->devname, p_result->bd_addr)) ==
            GAP_NO_DATA_AVAIL) {
            if ((p_cb->p_cur_inq = BTM_InqDbFirst()) != NULL) {
                if (BTM_ReadRemoteDeviceName(p_cb->p_cur_inq->results.remote_bd_addr,
                                             (tBTM_CMPL_CB*)&gap_find_addr_name_cb) == BTM_CMD_STARTED) {
                    return;
                } else {
                    p_result->status = gap_convert_btm_status(p->status);
                }
            } else {
                p_result->status = GAP_EOINQDB;
            }
        }
    } else {
        p_result->status = gap_convert_btm_status(p->status);
    }

    if (p_cb->p_cback)
        (*p_cb->p_cback)(GAP_EVT_FIND_ADDR_COMPLETE, p_result);

    p_cb->in_use = FALSE;
    p_cb->p_cback = NULL;
}

UINT16 gap_find_local_addr_by_name(const tBTM_BD_NAME devname, BD_ADDR bd_addr) {
    return GAP_NO_DATA_AVAIL;
}

void gap_free_cb(tGAP_INFO* p_cb) {
    if (p_cb) {
        p_cb->gap_cback = NULL;
        p_cb->in_use = FALSE;
    }
}

UINT16 gap_convert_btm_status(tBTM_STATUS btm_status) {
    switch (btm_status) {
    case BTM_SUCCESS:
        return BT_PASS;
    case BTM_CMD_STARTED:
        return GAP_CMD_INITIATED;
    case BTM_BUSY:
        return GAP_ERR_BUSY;
    case BTM_MODE_UNSUPPORTED:
    case BTM_ILLEGAL_VALUE:
        return GAP_ERR_ILL_PARM;
    case BTM_WRONG_MODE:
        return GAP_DEVICE_NOT_UP;
    case BTM_UNKNOWN_ADDR:
        return GAP_BAD_BD_ADDR;
    case BTM_DEVICE_TIMEOUT:
        return GAP_ERR_TIMEOUT;
    default:
        return GAP_ERR_PROCESSING;
    }
}
