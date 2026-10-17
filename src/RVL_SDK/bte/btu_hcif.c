#include <revolution/bte/bt_trace.h>
#include <revolution/bte/btm_api.h>
#include <revolution/bte/btm_int.h>
#include <revolution/bte/btu.h>
#include <revolution/bte/gki.h>
#include <revolution/bte/hcidefs.h>
#include <revolution/bte/hcimsgs.h>
#include <revolution/bte/hidd_int.h>
#include <revolution/bte/l2c_int.h>
#include <revolution/bte/transport.h>

static void btu_hcif_store_cmd(BT_HDR* p_buf);
static void btu_hcif_inquiry_comp_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_inquiry_result_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_inquiry_rssi_result_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_connection_comp_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_connection_request_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_disconnection_comp_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_authentication_comp_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_rmt_name_request_comp_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_encryption_change_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_change_conn_link_key_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_master_link_key_comp_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_read_rmt_features_comp_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_read_rmt_version_comp_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_qos_setup_comp_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_esco_connection_comp_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_esco_connection_chg_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_hdl_command_complete(UINT16 opcode, UINT8* p, UINT16 evt_len);
static void btu_hcif_command_complete_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_hdl_command_status(UINT16 opcode, UINT8 status, UINT8* p_cmd);
static void btu_hcif_command_status_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_hardware_error_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_flush_occured_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_role_change_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_num_compl_data_pkts_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_mode_change_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_return_link_keys_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_pin_code_request_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_link_key_request_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_link_key_notification_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_loopback_command_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_data_buf_overflow_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_max_slots_changed_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_read_clock_off_comp_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_conn_pkt_type_change_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_qos_violation_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_page_scan_mode_change_evt(UINT8* p, UINT16 evt_len);
static void btu_hcif_page_scan_rep_mode_chng_evt(UINT8* p, UINT16 evt_len);

static void btu_hcif_store_cmd(BT_HDR* p_buf) {
    UINT16 opcode;
    BT_HDR* p_cmd;
    UINT8* p = (UINT8*)(p_buf + 1) + p_buf->offset;

    STREAM_TO_UINT16(p, &opcode);

    if (opcode == HCI_RESET || opcode == HCI_HOST_NUM_PACKETS_DONE)
        return;

    if ((p_cmd = HCI_GET_CMD_BUF(p_buf->len + p_buf->offset - HCIC_PREAMBLE_SIZE)) == NULL)
        return;

    memcpy(p_cmd, p_buf, sizeof(BT_HDR));
    memcpy((UINT8*)(p_cmd + 1) + p_cmd->offset, (UINT8*)(p_buf + 1) + p_buf->offset, p_buf->len);

    GKI_enqueue(&btu_cb.cmd_cmpl_q, p_cmd);
    btu_start_timer(&btu_cb.cmd_cmpl_timer, BTU_TTYPE_BTU_CMD_CMPL, BTU_CMD_CMPL_TIMEOUT);
}

void btu_hcif_process_event(BT_HDR* p_msg) {
    UINT8* p = (UINT8*)(p_msg + 1) + p_msg->offset;
    UINT8 hci_evt_code;
    UINT8 hci_evt_len;

    STREAM_TO_UINT8(p, &hci_evt_code);
    STREAM_TO_UINT8(p, &hci_evt_len);

    if (btm_cb.devcb.state == BTM_DEV_STATE_WAIT_RESET_CMPLT && hci_evt_code != HCI_COMMAND_COMPLETE_EVT &&
        hci_evt_code != HCI_COMMAND_STATUS_EVT)
        return;

    switch (hci_evt_code) {
    case HCI_INQUIRY_COMP_EVT:
        btu_hcif_inquiry_comp_evt(p, hci_evt_len);
        break;
    case HCI_INQUIRY_RESULT_EVT:
        btu_hcif_inquiry_result_evt(p, hci_evt_len);
        break;
    case HCI_INQUIRY_RSSI_RESULT_EVT:
        btu_hcif_inquiry_rssi_result_evt(p, hci_evt_len);
        break;
    case HCI_CONNECTION_COMP_EVT:
        btu_hcif_connection_comp_evt(p, hci_evt_len);
        break;
    case HCI_CONNECTION_REQUEST_EVT:
        btu_hcif_connection_request_evt(p, hci_evt_len);
        break;
    case HCI_DISCONNECTION_COMP_EVT:
        btu_hcif_disconnection_comp_evt(p, hci_evt_len);
        break;
    case HCI_AUTHENTICATION_COMP_EVT:
        btu_hcif_authentication_comp_evt(p, hci_evt_len);
        break;
    case HCI_RMT_NAME_REQUEST_COMP_EVT:
        btu_hcif_rmt_name_request_comp_evt(p, hci_evt_len);
        break;
    case HCI_ENCRYPTION_CHANGE_EVT:
        btu_hcif_encryption_change_evt(p, hci_evt_len);
        break;
    case HCI_CHANGE_CONN_LINK_KEY_EVT:
        btu_hcif_change_conn_link_key_evt(p, hci_evt_len);
        break;
    case HCI_MASTER_LINK_KEY_COMP_EVT:
        btu_hcif_master_link_key_comp_evt(p, hci_evt_len);
        break;
    case HCI_READ_RMT_FEATURES_COMP_EVT:
        btu_hcif_read_rmt_features_comp_evt(p, hci_evt_len);
        break;
    case HCI_READ_RMT_VERSION_COMP_EVT:
        btu_hcif_read_rmt_version_comp_evt(p, hci_evt_len);
        break;
    case HCI_QOS_SETUP_COMP_EVT:
        btu_hcif_qos_setup_comp_evt(p, hci_evt_len);
        break;
    case HCI_COMMAND_COMPLETE_EVT:
        btu_hcif_command_complete_evt(p, hci_evt_len);
        break;
    case HCI_COMMAND_STATUS_EVT:
        btu_hcif_command_status_evt(p, hci_evt_len);
        break;
    case HCI_HARDWARE_ERROR_EVT:
        btu_hcif_hardware_error_evt(p, hci_evt_len);
        break;
    case HCI_FLUSH_OCCURED_EVT:
        btu_hcif_flush_occured_evt(p, hci_evt_len);
        break;
    case HCI_ROLE_CHANGE_EVT:
        btu_hcif_role_change_evt(p, hci_evt_len);
        break;
    case HCI_NUM_COMPL_DATA_PKTS_EVT:
        btu_hcif_num_compl_data_pkts_evt(p, hci_evt_len);
        break;
    case HCI_MODE_CHANGE_EVT:
        btu_hcif_mode_change_evt(p, hci_evt_len);
        break;
    case HCI_RETURN_LINK_KEYS_EVT:
        btu_hcif_return_link_keys_evt(p, hci_evt_len);
        break;
    case HCI_PIN_CODE_REQUEST_EVT:
        btu_hcif_pin_code_request_evt(p, hci_evt_len);
        break;
    case HCI_LINK_KEY_REQUEST_EVT:
        btu_hcif_link_key_request_evt(p, hci_evt_len);
        break;
    case HCI_LINK_KEY_NOTIFICATION_EVT:
        btu_hcif_link_key_notification_evt(p, hci_evt_len);
        break;
    case HCI_LOOPBACK_COMMAND_EVT:
        btu_hcif_loopback_command_evt(p, hci_evt_len);
        break;
    case HCI_DATA_BUF_OVERFLOW_EVT:
        btu_hcif_data_buf_overflow_evt(p, hci_evt_len);
        break;
    case HCI_MAX_SLOTS_CHANGED_EVT:
        btu_hcif_max_slots_changed_evt(p, hci_evt_len);
        break;
    case HCI_READ_CLOCK_OFF_COMP_EVT:
        btu_hcif_read_clock_off_comp_evt(p, hci_evt_len);
        break;
    case HCI_CONN_PKT_TYPE_CHANGE_EVT:
        btu_hcif_conn_pkt_type_change_evt(p, hci_evt_len);
        break;
    case HCI_QOS_VIOLATION_EVT:
        btu_hcif_qos_violation_evt(p, hci_evt_len);
        break;
    case HCI_PAGE_SCAN_MODE_CHANGE_EVT:
        btu_hcif_page_scan_mode_change_evt(p, hci_evt_len);
        break;
    case HCI_PAGE_SCAN_REP_MODE_CHNG_EVT:
        btu_hcif_page_scan_rep_mode_chng_evt(p, hci_evt_len);
        break;
    case HCI_ESCO_CONNECTION_COMP_EVT:
        btu_hcif_esco_connection_comp_evt(p, hci_evt_len);
        break;
    case HCI_ESCO_CONNECTION_CHANGED_EVT:
        btu_hcif_esco_connection_chg_evt(p, hci_evt_len);
        break;
    case HCI_BRCM_VENDOR_SPECIFIC_EVT:
    case HCI_VENDOR_SPECIFIC_EVT:
        btm_vendor_specific_evt(p, hci_evt_len);
        break;
    }
}

void btu_hcif_send_cmd(BT_HDR* p_buf) {
    if (p_buf != NULL && btu_cb.cmd_xmit_q.count != 0) {
        GKI_enqueue(&btu_cb.cmd_xmit_q, p_buf);
        p_buf = NULL;
    }

    if (btu_cb.controller_cmd_window == 0 && btm_cb.devcb.state == BTM_DEV_STATE_WAIT_RESET_CMPLT)
        btu_cb.controller_cmd_window = btu_cb.cmd_xmit_q.count + 1;

    while (btu_cb.controller_cmd_window != 0) {
        if (p_buf == NULL)
            p_buf = (BT_HDR*)GKI_dequeue(&btu_cb.cmd_xmit_q);

        if (p_buf == NULL)
            break;

        btu_hcif_store_cmd(p_buf);

        btu_cb.controller_cmd_window--;

        bte_hcisu_send((HC_BT_HDR*)p_buf, BT_EVT_TO_LM_HCI_CMD);

        p_buf = NULL;
    }

    if (p_buf != NULL)
        GKI_enqueue(&btu_cb.cmd_xmit_q, p_buf);
}

static void btu_hcif_inquiry_comp_evt(UINT8* p, UINT16 evt_len) {
    UINT8 status;

    STREAM_TO_UINT8(p, &status);

    btm_process_inq_complete(status);
}

static void btu_hcif_inquiry_result_evt(UINT8* p, UINT16 evt_len) {
    btm_process_inq_results(p, BTM_INQ_RESULT_STANDARD);
}

static void btu_hcif_inquiry_rssi_result_evt(UINT8* p, UINT16 evt_len) {
    btm_process_inq_results(p, BTM_INQ_RESULT_WITH_RSSI);
}

static void btu_hcif_connection_comp_evt(UINT8* p, UINT16 evt_len) {
    UINT8 status;
    UINT16 handle;
    BD_ADDR bda;
    UINT8 link_type;
    UINT8 enc_mode;
    tBTM_ESCO_DATA esco_data;

    STREAM_TO_UINT8(p, &status);
    STREAM_TO_UINT16(p, &handle);
    STREAM_TO_BDADDR(p, bda);
    STREAM_TO_UINT8(p, &link_type);
    STREAM_TO_UINT8(p, &enc_mode);

    handle = HCID_GET_HANDLE(handle);

    if (link_type == HCI_LINK_TYPE_ACL) {
        btm_sec_connected(bda, handle, status, enc_mode);
        l2c_link_hci_conn_comp(status, handle, bda);
    } else {
        memset(&esco_data, 0, sizeof(tBTM_ESCO_DATA));
        memcpy(esco_data.bd_addr, bda, BD_ADDR_LEN);
        btm_sco_connected(status, bda, handle, &esco_data);
    }
}

static void btu_hcif_connection_request_evt(UINT8* p, UINT16 evt_len) {
    BD_ADDR bda;
    DEV_CLASS dc;
    UINT8 link_type;

    STREAM_TO_BDADDR(p, bda);
    STREAM_TO_DEVCLASS(p, dc);
    STREAM_TO_UINT8(p, &link_type);

    if (link_type == HCI_LINK_TYPE_ACL)
        btm_sec_conn_req(bda, dc);
    else
        btm_sco_conn_req(bda, dc, link_type);
}

static void btu_hcif_disconnection_comp_evt(UINT8* p, UINT16 evt_len) {
    UINT8 status;
    UINT16 handle;
    UINT8 reason;

    STREAM_TO_UINT8(p, &status);
    STREAM_TO_UINT16(p, &handle);
    STREAM_TO_UINT8(p, &reason);

    handle = HCID_GET_HANDLE(handle);

    if (!l2c_link_hci_disc_comp(handle, reason))
        btm_sco_removed(handle, reason);

    btm_sec_disconnected(handle, reason);
}

static void btu_hcif_authentication_comp_evt(UINT8* p, UINT16 evt_len) {
    UINT8 status;
    UINT16 handle;

    STREAM_TO_UINT8(p, &status);
    STREAM_TO_UINT16(p, &handle);

    btm_sec_auth_complete(handle, status);
}

static void btu_hcif_rmt_name_request_comp_evt(UINT8* p, UINT16 evt_len) {
    UINT8 status;
    BD_ADDR bd_addr;

    STREAM_TO_UINT8(p, &status);
    STREAM_TO_BDADDR(p, bd_addr);

    evt_len -= BD_ADDR_LEN + 1;

    btm_process_remote_name(bd_addr, p, evt_len, status);
    btm_sec_rmt_name_request_complete(bd_addr, p, status);
}

static void btu_hcif_encryption_change_evt(UINT8* p, UINT16 evt_len) {
    UINT8 status;
    UINT16 handle;
    UINT8 encr_enable;

    STREAM_TO_UINT8(p, &status);
    STREAM_TO_UINT16(p, &handle);
    STREAM_TO_UINT8(p, &encr_enable);

    btm_acl_encrypt_change(handle, status, encr_enable);
    btm_sec_encrypt_change(handle, status, encr_enable);
}

static void btu_hcif_change_conn_link_key_evt(UINT8* p, UINT16 evt_len) {}

static void btu_hcif_master_link_key_comp_evt(UINT8* p, UINT16 evt_len) {
    UINT8 status;
    UINT16 handle;
    UINT8 key_flg;

    STREAM_TO_UINT8(p, &status);
    STREAM_TO_UINT16(p, &handle);
    STREAM_TO_UINT8(p, &key_flg);

    btm_sec_mkey_comp_event(handle, status, key_flg);
}

static void btu_hcif_read_rmt_features_comp_evt(UINT8* p, UINT16 evt_len) {
    btm_read_remote_features_complete(p);
}

static void btu_hcif_read_rmt_version_comp_evt(UINT8* p, UINT16 evt_len) {
    btm_read_remote_version_complete(p);
}

static void btu_hcif_qos_setup_comp_evt(UINT8* p, UINT16 evt_len) {
    UINT8 status;
    UINT16 handle;
    FLOW_SPEC flow;

    STREAM_TO_UINT8(p, &status);
    STREAM_TO_UINT16(p, &handle);
    STREAM_TO_UINT8(p, &flow.qos_flags);
    STREAM_TO_UINT8(p, &flow.service_type);
    STREAM_TO_UINT32(p, &flow.token_rate);
    STREAM_TO_UINT32(p, &flow.peak_bandwidth);
    STREAM_TO_UINT32(p, &flow.latency);
    STREAM_TO_UINT32(p, &flow.delay_variation);

    btm_qos_setup_complete(status, handle, &flow);
}

static void btu_hcif_esco_connection_comp_evt(UINT8* p, UINT16 evt_len) {
    tBTM_ESCO_DATA data;
    UINT16 handle;
    BD_ADDR bda;
    UINT8 status;

    STREAM_TO_UINT8(p, &status);
    STREAM_TO_UINT16(p, &handle);
    STREAM_TO_BDADDR(p, bda);
    STREAM_TO_UINT8(p, &data.link_type);
    STREAM_TO_UINT8(p, &data.tx_interval);
    STREAM_TO_UINT8(p, &data.retrans_window);
    STREAM_TO_UINT16(p, &data.rx_pkt_len);
    STREAM_TO_UINT16(p, &data.tx_pkt_len);
    STREAM_TO_UINT8(p, &data.air_mode);

    memcpy(data.bd_addr, bda, BD_ADDR_LEN);
    btm_sco_connected(status, bda, handle, &data);
}

static void btu_hcif_esco_connection_chg_evt(UINT8* p, UINT16 evt_len) {
    UINT16 handle;
    UINT16 tx_pkt_len;
    UINT16 rx_pkt_len;
    UINT8 status;
    UINT8 tx_interval;
    UINT8 retrans_window;

    STREAM_TO_UINT8(p, &status);
    STREAM_TO_UINT16(p, &handle);
    STREAM_TO_UINT8(p, &tx_interval);
    STREAM_TO_UINT8(p, &retrans_window);
    STREAM_TO_UINT16(p, &rx_pkt_len);
    STREAM_TO_UINT16(p, &tx_pkt_len);

    btm_esco_proc_conn_chg(status, handle, tx_interval, retrans_window, rx_pkt_len, tx_pkt_len);
}

static void btu_hcif_hdl_command_complete(UINT16 opcode, UINT8* p, UINT16 evt_len) {
    switch (opcode) {
    case HCI_RESET:
        btm_reset_complete();
        break;
    case HCI_INQUIRY_CANCEL:
        btm_process_inq_complete(HCI_SUCCESS);
        break;
    case HCI_SET_EVENT_FILTER:
        btm_event_filter_complete(p);
        break;
    case HCI_READ_STORED_LINK_KEY:
        btm_read_stored_link_key_complete(p);
        break;
    case HCI_WRITE_STORED_LINK_KEY:
        btm_write_stored_link_key_complete(p);
        break;
    case HCI_DELETE_STORED_LINK_KEY:
        btm_delete_stored_link_key_complete(p);
        break;
    case HCI_READ_LOCAL_VERSION_INFO:
        btm_read_local_version_complete(p, evt_len);
        break;
    case HCI_READ_POLICY_SETTINGS:
        btm_read_link_policy_complete(p);
        break;
    case HCI_READ_BUFFER_SIZE:
        btm_read_hci_buf_size_complete(p, evt_len);
        break;
    case HCI_READ_LOCAL_FEATURES:
        btm_read_local_features_complete(p, evt_len);
        break;
    case HCI_READ_LOCAL_NAME:
        btm_read_local_name_complete(p, evt_len);
        break;
    case HCI_READ_BD_ADDR:
        btm_read_local_addr_complete(p, evt_len);
        break;
    case HCI_GET_LINK_QUALITY:
        btm_read_link_quality_complete(p);
        break;
    case HCI_READ_RSSI:
        btm_read_rssi_complete(p);
        break;
    default:
        if ((opcode & HCI_GROUP_VENDOR_SPECIFIC) == HCI_GROUP_VENDOR_SPECIFIC)
            btm_vsc_complete(p, opcode, evt_len);
        break;
    }
}

static void btu_hcif_command_complete_evt(UINT8* p, UINT16 evt_len) {
    UINT16 cc_opcode;
    BT_HDR* p_cmd;
    UINT8* pp;
    UINT16 queued_opcode;

    STREAM_TO_UINT8(p, &btu_cb.controller_cmd_window);
    STREAM_TO_UINT16(p, &cc_opcode);

    evt_len -= 3;

    if (cc_opcode != HCI_RESET && cc_opcode != HCI_HOST_NUM_PACKETS_DONE && cc_opcode != HCI_COMMAND_NONE) {
        p_cmd = (BT_HDR*)GKI_dequeue(&btu_cb.cmd_cmpl_q);

        if (p_cmd != NULL) {
            pp = (UINT8*)(p_cmd + 1);
            STREAM_TO_UINT16(pp, &queued_opcode);

            if (cc_opcode == HCI_BRCM_WRITE_RAM && queued_opcode != cc_opcode) {
                GKI_enqueue_head(&btu_cb.cmd_cmpl_q, p_cmd);
                return;
            }

            GKI_freebuf(p_cmd);
        }

        if (!GKI_queue_is_empty(&btu_cb.cmd_cmpl_q))
            btu_start_timer(&btu_cb.cmd_cmpl_timer, BTU_TTYPE_BTU_CMD_CMPL, BTU_CMD_CMPL_TIMEOUT);
        else
            btu_stop_timer(&btu_cb.cmd_cmpl_timer);
    }

    btu_hcif_hdl_command_complete(cc_opcode, p, evt_len);
    btu_hcif_send_cmd(NULL);
}

static void btu_hcif_hdl_command_status(UINT16 opcode, UINT8 status, UINT8* p_cmd) {
    BD_ADDR bd_addr;
    UINT16 handle;
    tBTM_ESCO_DATA esco_data;

    switch (opcode) {
    case HCI_EXIT_SNIFF_MODE:
    case HCI_EXIT_PARK_MODE:
    case HCI_HOLD_MODE:
    case HCI_SNIFF_MODE:
    case HCI_PARK_MODE:
        btm_pm_proc_cmd_status(status);
        return;
    }

    if (status == HCI_SUCCESS)
        return;

    switch (opcode) {
    case HCI_INQUIRY:
        btm_process_inq_complete(status);
        break;
    case HCI_RMT_NAME_REQUEST:
        btm_process_remote_name(NULL, NULL, 0, status);
        btm_sec_rmt_name_request_complete(NULL, NULL, status);
        break;
    case HCI_QOS_SETUP_COMP_EVT:
        btm_qos_setup_complete(status, 0, NULL);
        break;
    case HCI_SWITCH_ROLE:
        btm_acl_role_changed(status, NULL, BTM_ROLE_UNDEFINED);
        l2c_link_role_change_failed();
        break;
    case HCI_CREATE_CONNECTION:
        if (p_cmd != NULL) {
            p_cmd++;
            STREAM_TO_BDADDR(p_cmd, bd_addr);
            btm_sec_connected(bd_addr, HCI_INVALID_HANDLE, status, 0);
            l2c_link_hci_conn_comp(status, HCI_INVALID_HANDLE, bd_addr);
        }
        break;
    case HCI_SETUP_ESCO_CONNECTION:
        if (p_cmd != NULL) {
            p_cmd++;
            STREAM_TO_UINT16(p_cmd, &handle);

            if (btm_is_sco_active(handle))
                btm_esco_proc_conn_chg(status, handle, 0, 0, 0, 0);
            else
                btm_sco_connected(status, NULL, handle, &esco_data);
        }
        break;
    default:
        if ((opcode & HCI_GROUP_VENDOR_SPECIFIC) == HCI_GROUP_VENDOR_SPECIFIC)
            btm_vsc_complete(p_cmd, opcode, 1);
        break;
    }
}

static void btu_hcif_command_status_evt(UINT8* p, UINT16 evt_len) {
    UINT8 status;
    UINT16 opcode;
    BT_HDR* p_cmd = NULL;
    UINT16 cmd_opcode;
    UINT8* p_data = NULL;

    STREAM_TO_UINT8(p, &status);
    STREAM_TO_UINT8(p, &btu_cb.controller_cmd_window);
    STREAM_TO_UINT16(p, &opcode);

    if (opcode != HCI_RESET && opcode != HCI_HOST_NUM_PACKETS_DONE && opcode != HCI_COMMAND_NONE) {
        if ((p_cmd = (BT_HDR*)GKI_dequeue(&btu_cb.cmd_cmpl_q)) != NULL) {
            p_data = (UINT8*)(p_cmd + 1) + p_cmd->offset;
            STREAM_TO_UINT16(p_data, &cmd_opcode);

            if (cmd_opcode != opcode) {
                p_data = NULL;
                HCI_TRACE_WARNING2("Event mismatch opcode=%X cmd opcode=%X", opcode, cmd_opcode);
            }
        }

        if (!GKI_queue_is_empty(&btu_cb.cmd_cmpl_q))
            btu_start_timer(&btu_cb.cmd_cmpl_timer, BTU_TTYPE_BTU_CMD_CMPL, BTU_CMD_CMPL_TIMEOUT);
        else
            btu_stop_timer(&btu_cb.cmd_cmpl_timer);
    }

    btu_hcif_hdl_command_status(opcode, status, p_data);

    if (p_cmd != NULL)
        GKI_freebuf(p_cmd);

    btu_hcif_send_cmd(NULL);
}

void btu_hcif_cmd_timeout(void) {
    BT_HDR* p_cmd;
    UINT8* p;
    UINT16 opcode;
    UINT8 event[1];

    btu_cb.controller_cmd_window = 1;

    if ((p_cmd = (BT_HDR*)GKI_dequeue(&btu_cb.cmd_cmpl_q)) == NULL) {
        HCI_TRACE_WARNING0("Cmd timeout; no cmd in queue");
        return;
    }

    if (!GKI_queue_is_empty(&btu_cb.cmd_cmpl_q))
        btu_start_timer(&btu_cb.cmd_cmpl_timer, BTU_TTYPE_BTU_CMD_CMPL, BTU_CMD_CMPL_TIMEOUT);

    p = (UINT8*)(p_cmd + 1) + p_cmd->offset;

    STREAM_TO_UINT16(p, &opcode);

    HCI_TRACE_WARNING1("BTU HCI command timeout - cmd opcode = 0x%02x", opcode);

    switch (opcode) {
    case HCI_HOLD_MODE:
    case HCI_SNIFF_MODE:
    case HCI_EXIT_SNIFF_MODE:
    case HCI_PARK_MODE:
    case HCI_EXIT_PARK_MODE:
    case HCI_INQUIRY:
    case HCI_RMT_NAME_REQUEST:
    case HCI_QOS_SETUP_COMP_EVT:
    case HCI_CREATE_CONNECTION:
        btu_hcif_hdl_command_status(opcode, HCI_ERR_UNSPECIFIED, p);
        break;
    default:
        event[0] = HCI_ERR_UNSPECIFIED;
        btu_hcif_hdl_command_complete(opcode, event, 1);
        break;
    }

    GKI_freebuf(p_cmd);

    btm_report_device_status(BTM_DEV_STATUS_CMD_TOUT);
    btu_hcif_send_cmd(NULL);
}

static void btu_hcif_hardware_error_evt(UINT8* p, UINT16 evt_len) {
    HCI_TRACE_ERROR0("Ctlr H/w error event");

    if (BTM_IsDeviceUp())
        BTM_DeviceReset(NULL);
}

static void btu_hcif_flush_occured_evt(UINT8* p, UINT16 evt_len) {}

static void btu_hcif_role_change_evt(UINT8* p, UINT16 evt_len) {
    UINT8 status;
    BD_ADDR bda;
    UINT8 role;

    STREAM_TO_UINT8(p, &status);
    STREAM_TO_BDADDR(p, bda);
    STREAM_TO_UINT8(p, &role);

    if (status == HCI_SUCCESS)
        l2c_link_role_changed(bda, role);

    btm_acl_role_changed(status, bda, role);
}

static void btu_hcif_num_compl_data_pkts_evt(UINT8* p, UINT16 evt_len) {
    l2c_link_process_num_completed_pkts(p);
}

static void btu_hcif_mode_change_evt(UINT8* p, UINT16 evt_len) {
    UINT8 status;
    UINT16 handle;
    UINT8 current_mode;
    UINT16 interval;

    STREAM_TO_UINT8(p, &status);
    STREAM_TO_UINT16(p, &handle);
    STREAM_TO_UINT8(p, &current_mode);
    STREAM_TO_UINT16(p, &interval);

    btm_sco_chk_pend_unpark(status, handle, current_mode);
    btm_pm_proc_mode_change(status, handle, current_mode, interval);
    hidd_pm_proc_mode_change(status, current_mode, interval);
}

static void btu_hcif_return_link_keys_evt(UINT8* p, UINT16 evt_len) {
    UINT8 num_keys;
    tBTM_RETURN_LINK_KEYS_EVT* result;

    num_keys = *p;

    if (num_keys == 0)
        return;

    result = (tBTM_RETURN_LINK_KEYS_EVT*)(p - 1);
    result->event = BTM_CB_EVT_RETURN_LINK_KEYS;

    btm_return_link_keys_evt(result);
}

static void btu_hcif_pin_code_request_evt(UINT8* p, UINT16 evt_len) {
    BD_ADDR bda;

    STREAM_TO_BDADDR(p, bda);

    l2c_pin_code_request(bda);
    btm_sec_pin_code_request(bda);
}

static void btu_hcif_link_key_request_evt(UINT8* p, UINT16 evt_len) {
    BD_ADDR bda;

    STREAM_TO_BDADDR(p, bda);

    btm_sec_link_key_request(bda);
}

static void btu_hcif_link_key_notification_evt(UINT8* p, UINT16 evt_len) {
    BD_ADDR bda;
    LINK_KEY key;
    UINT8 key_type;

    STREAM_TO_BDADDR(p, bda);
    STREAM_TO_ARRAY16(p, key);
    STREAM_TO_UINT8(p, &key_type);

    btm_sec_link_key_notification(bda, key, key_type);
}

static void btu_hcif_loopback_command_evt(UINT8* p, UINT16 evt_len) {}

static void btu_hcif_data_buf_overflow_evt(UINT8* p, UINT16 evt_len) {}

static void btu_hcif_max_slots_changed_evt(UINT8* p, UINT16 evt_len) {}

static void btu_hcif_read_clock_off_comp_evt(UINT8* p, UINT16 evt_len) {
    UINT8 status;
    UINT16 handle;
    UINT16 clock_offset;

    STREAM_TO_UINT8(p, &status);

    if (status != HCI_SUCCESS)
        return;

    STREAM_TO_UINT16(p, &handle);
    STREAM_TO_UINT16(p, &clock_offset);

    handle = HCID_GET_HANDLE(handle);

    btm_process_clk_off_comp_evt(handle, clock_offset);
    btm_sec_update_clock_offset(handle, clock_offset);
}

static void btu_hcif_conn_pkt_type_change_evt(UINT8* p, UINT16 evt_len) {}

static void btu_hcif_qos_violation_evt(UINT8* p, UINT16 evt_len) {
    UINT16 handle;

    STREAM_TO_UINT16(p, &handle);

    handle = HCID_GET_HANDLE(handle);

    l2c_link_hci_qos_violation(handle);
}

static void btu_hcif_page_scan_mode_change_evt(UINT8* p, UINT16 evt_len) {}

static void btu_hcif_page_scan_rep_mode_chng_evt(UINT8* p, UINT16 evt_len) {}
