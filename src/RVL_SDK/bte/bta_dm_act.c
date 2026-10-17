#include <RVL_SDK/bte/bd.h>
#include <RVL_SDK/bte/bt_trace.h>
#include <RVL_SDK/bte/bta_dm_int.h>
#include <RVL_SDK/bte/btm_int.h>
#include <RVL_SDK/bte/btu.h>
#include <RVL_SDK/bte/gki.h>
#include <RVL_SDK/bte/l2c_api.h>
#include <RVL_SDK/bte/sdpdefs.h>
#include <RVL_SDK/bte/wbt_api.h>

static void bta_dm_disable_timer_cback(TIMER_LIST_ENT* p_tle);
static void bta_dm_search_timer_cback(TIMER_LIST_ENT* p_tle);
static void bta_dm_find_services(BD_ADDR bd_addr);
static void bta_dm_discover_next_device(void);
static void bta_dm_inq_results_cb(tBTM_INQ_RESULTS* p_inq);
static void bta_dm_inq_cmpl_cb(void* p_result);
static void bta_dm_service_search_remname_cback(BD_ADDR bd_addr, DEV_CLASS dc, tBTM_BD_NAME bd_name);
static void bta_dm_remname_cback(tBTM_REMOTE_DEV_NAME* p_remote_name);
static void bta_dm_disc_remname_cback(tBTM_REMOTE_DEV_NAME* p_remote_name);
static void bta_dm_start_remote_name_response_timer(void);
static void bta_dm_stop_remote_name_response_timer(void);
static void bta_dm_remote_name_request_cmd_failed(UINT8* bd_addr);
static UINT8 bta_dm_authorize_cback(BD_ADDR bd_addr, DEV_CLASS dev_class, tBTM_BD_NAME bd_name, UINT8* service_name, UINT8 service_id,
                                    BOOLEAN is_originator);
static UINT8 bta_dm_pin_cback(BD_ADDR bd_addr, DEV_CLASS dev_class, tBTM_BD_NAME bd_name);
static UINT8 bta_dm_link_key_request_cback(BD_ADDR bd_addr, LINK_KEY key);
static UINT8 bta_dm_new_link_key_cback(BD_ADDR bd_addr, DEV_CLASS dev_class, tBTM_BD_NAME bd_name, UINT8* key, UINT8 key_type);
static UINT8 bta_dm_authentication_complete_cback(BD_ADDR bd_addr, DEV_CLASS dev_class, tBTM_BD_NAME bd_name, int result);
static void bta_dm_local_addr_cback(void* p1);
static void bta_dm_acl_change_cback(BD_ADDR p_bda, DEV_CLASS p_dc, BD_NAME p_bdn, BD_FEATURES features, BOOLEAN is_new);
static void bta_dm_disable_conn_down_timer_cback(TIMER_LIST_ENT* p_tle);
static BOOLEAN bta_dm_l2cap_server_compress_cback(BD_ADDR peer_addr, int, int, int, int, int, int, UINT8** p_mem_pool,
                                                  UINT32* mem_pool_size);
static void bta_dm_init_compress(void);
static void bta_dm_compress_cback(tBTA_SYS_CONN_STATUS status, UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
static void bta_dm_rm_cback(tBTA_SYS_CONN_STATUS status, UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
static char* bta_dm_get_remname(void);

const UINT16 bta_service_id_to_uuid_lkup_tbl[BTA_MAX_SERVICE_ID] = {
    UUID_SERVCLASS_PNP_INFORMATION,
    UUID_SERVCLASS_SERIAL_PORT,
    UUID_SERVCLASS_DIALUP_NETWORKING,
    UUID_SERVCLASS_FAX,
    UUID_SERVCLASS_LAN_ACCESS_USING_PPP,
    UUID_SERVCLASS_HEADSET,
    UUID_SERVCLASS_HF_HANDSFREE,
    UUID_SERVCLASS_OBEX_OBJECT_PUSH,
    UUID_SERVCLASS_OBEX_FILE_TRANSFER,
    UUID_SERVCLASS_CORDLESS_TELEPHONY,
    UUID_SERVCLASS_INTERCOM,
    UUID_SERVCLASS_IRMC_SYNC,
    UUID_SERVCLASS_DIRECT_PRINTING,
    UUID_SERVCLASS_IMAGING_RESPONDER,
    UUID_SERVCLASS_PANU,
    UUID_SERVCLASS_NAP,
    UUID_SERVCLASS_GN,
    UUID_SERVCLASS_SAP,
    UUID_SERVCLASS_AUDIO_SINK,
    UUID_SERVCLASS_AV_REMOTE_CONTROL,
    UUID_SERVCLASS_HUMAN_INTERFACE,
    UUID_SERVCLASS_VIDEO_SINK,
    UUID_SERVCLASS_PBAP_PSE,
};

const UINT32 bta_service_id_to_btm_srv_id_lkup_tbl[BTA_MAX_SERVICE_ID] = {
    0,
    BTM_SEC_SERVICE_SERIAL_PORT,
    BTM_SEC_SERVICE_DUN,
    BTM_SEC_SERVICE_FAX,
    BTM_SEC_SERVICE_LAN_ACCESS,
    BTM_SEC_SERVICE_HEADSET_AG,
    BTM_SEC_SERVICE_AG_HANDSFREE,
    BTM_SEC_SERVICE_OBEX,
    BTM_SEC_SERVICE_OBEX_FTP,
    BTM_SEC_SERVICE_CORDLESS,
    BTM_SEC_SERVICE_INTERCOM,
    BTM_SEC_SERVICE_IRMC_SYNC,
    BTM_SEC_SERVICE_BPP_JOB,
    BTM_SEC_SERVICE_BIP,
    BTM_SEC_SERVICE_BNEP_PANU,
    BTM_SEC_SERVICE_BNEP_NAP,
    BTM_SEC_SERVICE_BNEP_GN,
    BTM_SEC_SERVICE_SAP,
    BTM_SEC_SERVICE_AVDTP,
    BTM_SEC_SERVICE_AVCTP,
    BTM_SEC_SERVICE_HID_SEC_CTRL,
    BTM_SEC_SERVICE_AVDTP,
    BTM_SEC_SERVICE_PBAP,
};

const tBTM_APPL_INFO bta_security = {
    &bta_dm_authorize_cback,
    &bta_dm_pin_cback,
    &bta_dm_new_link_key_cback,
    &bta_dm_link_key_request_cback,
    &bta_dm_authentication_complete_cback,
    NULL,
};

static tBTA_DM_COMPRESS_SRVCS bta_dm_compress_srvcs[BTA_DM_NUM_COMPRESS_SRVS];

void bta_dm_enable(tBTA_DM_MSG* p_data) {
    DEV_CLASS dev_class;

    memset(&bta_dm_search_cb, 0, sizeof(bta_dm_search_cb));
    memset(&bta_dm_conn_srvcs, 0, sizeof(bta_dm_conn_srvcs));

    memcpy(dev_class, bta_dm_cfg.dev_class, DEV_CLASS_LEN);
    BTM_SetDeviceClass(dev_class);

    bta_dm_cb.p_sec_cback = p_data->enable.p_sec_cback;
    BTM_SecRegister((tBTM_APPL_INFO*)&bta_security);

    BTM_SetDefaultLinkSuperTout(bta_dm_cfg.link_timeout);
    BTM_WritePageTimeout(bta_dm_cfg.page_timeout);
    BTM_SetDefaultLinkPolicy(bta_dm_cfg.policy_settings);

    BTM_AclRegisterForChanges(&bta_dm_acl_change_cback);
    BTM_ReadLocalDeviceAddr(&bta_dm_local_addr_cback);

    bta_sys_rm_register(&bta_dm_rm_cback);
    bta_sys_compress_register(&bta_dm_compress_cback);

    bta_dm_init_pm();
    bta_dm_init_compress();

    WBT_ExtCreateRecord();
}

void bta_dm_disable(tBTA_DM_MSG* p_data) {
    L2CA_SetIdleTimeoutByBdAddr((BD_ADDR_PTR)BT_BD_ANY, 0);

    bta_sys_disable();

    BTM_SetDiscoverability(BTM_NON_DISCOVERABLE, 0, 0);
    BTM_SetConnectability(BTM_NON_CONNECTABLE, 0, 0);

    bta_dm_disable_pm();

    if (BTM_GetNumAclLinks() == 0) {
        bta_dm_cb.disable_timer.p_cback = (TIMER_CBACK*)&bta_dm_disable_conn_down_timer_cback;
        bta_sys_start_timer(&bta_dm_cb.disable_timer, 0, BTA_DISABLE_DELAY);
    } else {
        bta_dm_cb.disabling = TRUE;
        bta_dm_cb.disable_timer.p_cback = (TIMER_CBACK*)&bta_dm_disable_timer_cback;
        bta_sys_start_timer(&bta_dm_cb.disable_timer, 0, 5000);
    }
}

static void bta_dm_disable_timer_cback(TIMER_LIST_ENT* p_tle) {
    UINT8 i;

    APPL_TRACE_EVENT0(" bta_dm_disable_timer_cback  ");

    if (BTM_GetNumAclLinks() != 0 && !bta_dm_cb.keep_acl_on_shut_down) {
        for (i = 0; i < bta_dm_cb.device_list.count; i++) {
            btm_remove_acl(bta_dm_cb.device_list.peer_device[i].peer_bdaddr);
        }
    } else {
        bta_dm_cb.disabling = FALSE;
        bta_dm_cb.p_sec_cback(BTA_DM_DISABLE_EVT, NULL);
    }
}

void bta_dm_set_dev_name(tBTA_DM_MSG* p_data) {
    BTM_SetLocalDeviceName(p_data->set_name.name);
}

void bta_dm_set_visibility(tBTA_DM_MSG* p_data) {
    BTM_SetDiscoverability(p_data->set_visibility.disc_mode, 0, 0);
    BTM_SetConnectability(p_data->set_visibility.conn_mode, 0, 0);
}

void bta_dm_bond(tBTA_DM_MSG* p_data) {
    UINT32 trusted_mask[BTM_SEC_SERVICE_ARRAY_SIZE];
    UINT32* current_trusted_mask;

    current_trusted_mask = btm_read_trusted_mask(p_data->bond.bd_addr);

    if (current_trusted_mask) {
        memcpy(trusted_mask, current_trusted_mask, sizeof(trusted_mask));
    } else {
        memset(trusted_mask, 0, sizeof(trusted_mask));
    }

    BTM_SecBond(p_data->bond.bd_addr, p_data->bond.pin_len, p_data->bond.p_pin, trusted_mask);
}

void bta_dm_pin_reply(tBTA_DM_MSG* p_data) {
    UINT32 trusted_mask[BTM_SEC_SERVICE_ARRAY_SIZE];
    UINT32* current_trusted_mask;

    current_trusted_mask = btm_read_trusted_mask(p_data->pin_reply.bd_addr);

    if (current_trusted_mask) {
        memcpy(trusted_mask, current_trusted_mask, sizeof(trusted_mask));
    } else {
        memset(trusted_mask, 0, sizeof(trusted_mask));
    }

    if (p_data->pin_reply.accept) {
        BTM_PINCodeReply(p_data->pin_reply.bd_addr, BTM_SUCCESS, p_data->pin_reply.pin_len, p_data->pin_reply.p_pin, trusted_mask);
    } else {
        BTM_PINCodeReply(p_data->pin_reply.bd_addr, BTM_NOT_AUTHORIZED, 0, NULL, trusted_mask);
    }
}

void bta_dm_auth_reply(tBTA_DM_MSG* p_data) {
    UINT32 trusted_mask[BTM_SEC_SERVICE_ARRAY_SIZE];
    UINT8 btm_mask_index = 0;
    UINT32* current_trusted_mask;

    current_trusted_mask = btm_read_trusted_mask(p_data->auth_reply.bd_addr);

    if (current_trusted_mask) {
        memcpy(trusted_mask, current_trusted_mask, sizeof(trusted_mask));
    } else {
        memset(trusted_mask, 0, sizeof(trusted_mask));
    }

    if (p_data->auth_reply.response != BTA_DM_NOT_AUTH) {
        if (p_data->auth_reply.response == BTA_DM_AUTH_PERM) {
            if (p_data->auth_reply.service <= BTA_MAX_SERVICE_ID) {
                btm_mask_index = bta_service_id_to_btm_srv_id_lkup_tbl[p_data->auth_reply.service] / BTM_SEC_ARRAY_BITS;
                trusted_mask[btm_mask_index] |=
                    1 << (bta_service_id_to_btm_srv_id_lkup_tbl[p_data->auth_reply.service] - btm_mask_index * BTM_SEC_ARRAY_BITS);
            }
        }

        BTM_DeviceAuthorized(p_data->auth_reply.bd_addr, BTM_SUCCESS, trusted_mask);
    } else {
        BTM_DeviceAuthorized(p_data->auth_reply.bd_addr, BTM_NOT_AUTHORIZED, trusted_mask);
    }
}

void bta_dm_search_start(tBTA_DM_MSG* p_data) {
    BTM_ClearInqDb(NULL);

    bta_dm_search_cb.p_search_cback = p_data->search.p_cback;
    bta_dm_search_cb.services = p_data->search.services;

    BTM_StartInquiry((tBTM_INQ_PARMS*)&p_data->search.inq_params, (tBTM_INQ_RESULTS_CB*)&bta_dm_inq_results_cb, &bta_dm_inq_cmpl_cb);
}

void bta_dm_search_cancel(tBTA_DM_MSG* p_data) {
    tBTA_DM_MSG* p_msg;

    BTM_ClearInqDb(NULL);

    if (BTM_IsInquiryActive()) {
        BTM_CancelInquiry();
        bta_dm_search_cancel_notify(NULL);

        if ((p_msg = (tBTA_DM_MSG*)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL) {
            p_msg->hdr.event = BTA_DM_SEARCH_CMPL_EVT;
            bta_sys_sendmsg(p_msg);
        }
    } else if (!bta_dm_search_cb.services) {
        BTM_CancelRemoteDeviceName();
    }
}

void bta_dm_discover(tBTA_DM_MSG* p_data) {
    bta_dm_search_cb.p_search_cback = p_data->discover.p_cback;
    bta_dm_search_cb.services = p_data->discover.services;
    bta_dm_search_cb.services_to_search = bta_dm_search_cb.services;
    bta_dm_search_cb.service_index = 0;
    bta_dm_search_cb.services_found = 0;
    bta_dm_search_cb.peer_name[0] = '\0';

    BTM_SecAddRmtNameNotifyCallback(&bta_dm_service_search_remname_cback);

    bdcpy(bta_dm_search_cb.peer_bdaddr, p_data->discover.bd_addr);

    if (bta_dm_search_cb.services) {
        bta_dm_find_services(bta_dm_search_cb.peer_bdaddr);
    } else {
        bta_dm_start_remote_name_response_timer();

        if (BTM_ReadRemoteDeviceName(bta_dm_search_cb.peer_bdaddr, (tBTM_CMPL_CB*)&bta_dm_disc_remname_cback) != BTM_CMD_STARTED) {
            bta_dm_stop_remote_name_response_timer();
            bta_dm_remote_name_request_cmd_failed(bta_dm_search_cb.peer_bdaddr);
        }
    }
}

void bta_dm_inq_cmpl(tBTA_DM_MSG* p_data) {
    tBTA_DM_MSG* p_msg;
    tBTA_DM_SEARCH data;
    BOOLEAN search_complete = TRUE;
    tBTA_DM_DISC_RES disc_res;

    if ((bta_dm_search_cb.p_btm_inq_info = BTM_InqDbFirst()) != NULL) {
        search_complete = FALSE;

        if (bta_dm_search_cb.services) {
            if (BTM_IsAclConnectionUp(bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr)) {
                bta_dm_search_cb.wait_disc = FALSE;
            } else {
                bta_dm_search_cb.wait_disc = TRUE;
            }

            bta_dm_search_cb.sdp_results = FALSE;

            BTM_SecAddRmtNameNotifyCallback(&bta_dm_service_search_remname_cback);

            bta_dm_search_cb.service_index = 0;
            bta_dm_search_cb.services_found = 0;
            bta_dm_search_cb.services_to_search = bta_dm_search_cb.services;
            bta_dm_search_cb.peer_name[0] = '\0';

            bdcpy(bta_dm_search_cb.peer_bdaddr, bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr);

            bta_dm_find_services(bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr);
        } else {
            search_complete = TRUE;

            for (; bta_dm_search_cb.p_btm_inq_info; bta_dm_search_cb.p_btm_inq_info = BTM_InqDbNext(bta_dm_search_cb.p_btm_inq_info)) {
                if (bta_dm_search_cb.p_btm_inq_info->appl_knows_rem_name) {
                    bdcpy(disc_res.bd_addr, bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr);
                    disc_res.bd_name[0] = '\0';
                    disc_res.services = 0;

                    bta_dm_search_cb.p_search_cback(BTA_DM_DISC_RES_EVT, (tBTA_DM_SEARCH*)&disc_res);
                } else {
                    bta_dm_start_remote_name_response_timer();

                    if (BTM_ReadRemoteDeviceName(bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr,
                                                 (tBTM_CMPL_CB*)&bta_dm_remname_cback) != BTM_CMD_STARTED) {
                        bta_dm_stop_remote_name_response_timer();
                        bta_dm_remote_name_request_cmd_failed(bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr);
                    }

                    search_complete = FALSE;
                    break;
                }
            }
        }
    }

    if (search_complete) {
        bta_dm_search_cb.services = 0;

        if ((p_msg = (tBTA_DM_MSG*)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL) {
            p_msg->hdr.event = BTA_DM_SEARCH_CMPL_EVT;
            bta_sys_sendmsg(p_msg);
        }
    }

    data.inq_cmpl.num_resps = p_data->inq_cmpl.num;
    bta_dm_search_cb.p_search_cback(BTA_DM_INQ_CMPL_EVT, &data);
}

void bta_dm_rmt_name(tBTA_DM_MSG* p_data) {
    tBTA_DM_MSG* p_msg;
    BOOLEAN search_complete = TRUE;
    tBTA_DM_DISC_RES disc_res;

    while ((bta_dm_search_cb.p_btm_inq_info = BTM_InqDbNext(bta_dm_search_cb.p_btm_inq_info)) != NULL) {
        if (bta_dm_search_cb.p_btm_inq_info->appl_knows_rem_name) {
            bdcpy(disc_res.bd_addr, bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr);
            disc_res.bd_name[0] = '\0';
            disc_res.services = 0;

            bta_dm_search_cb.p_search_cback(BTA_DM_DISC_RES_EVT, (tBTA_DM_SEARCH*)&disc_res);
        } else {
            bta_dm_start_remote_name_response_timer();

            if (BTM_ReadRemoteDeviceName(bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr, (tBTM_CMPL_CB*)&bta_dm_remname_cback) !=
                BTM_CMD_STARTED) {
                bta_dm_stop_remote_name_response_timer();
                bta_dm_remote_name_request_cmd_failed(bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr);
            }

            search_complete = FALSE;
            break;
        }
    }

    if (search_complete) {
        if ((p_msg = (tBTA_DM_MSG*)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL) {
            p_msg->hdr.event = BTA_DM_SEARCH_CMPL_EVT;
            bta_sys_sendmsg(p_msg);
        }
    }

    bta_dm_search_cb.p_search_cback(BTA_DM_DISC_RES_EVT, &p_data->disc_result.result);
}

void bta_dm_disc_rmt_name(tBTA_DM_MSG* p_data) {
    tBTA_DM_MSG* p_msg;

    if ((p_msg = (tBTA_DM_MSG*)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL) {
        p_msg->hdr.event = BTA_DM_SEARCH_CMPL_EVT;
        bta_sys_sendmsg(p_msg);
    }

    p_data->disc_result.result.disc_res.services = 0;
    bta_dm_search_cb.p_search_cback(BTA_DM_DISC_RES_EVT, &p_data->disc_result.result);
}

void bta_dm_sdp_result(tBTA_DM_MSG* p_data) {
    tSDP_DISC_REC* p_sdp_rec = NULL;
    tBTA_DM_MSG* p_msg;
    BOOLEAN service_found = FALSE;
    UINT16 service;

    if (p_data->sdp_event.sdp_result == SDP_SUCCESS || p_data->sdp_event.sdp_result == SDP_NO_RECS_MATCH ||
        p_data->sdp_event.sdp_result == SDP_DB_FULL) {
        service = bta_service_id_to_uuid_lkup_tbl[bta_dm_search_cb.service_index - 1];

        if (p_data->sdp_event.sdp_result == SDP_DB_FULL ||
            (p_sdp_rec = SDP_FindServiceInDb(bta_dm_search_cb.p_sdp_db, service, p_sdp_rec)) != NULL) {
            if (service == UUID_SERVCLASS_PNP_INFORMATION) {
                if (p_sdp_rec && SDP_FindAttributeInRec(p_sdp_rec, ATTR_ID_EXT_BRCM_VERSION)) {
                    service_found = TRUE;
                }
            } else {
                service_found = TRUE;
            }

            if (service_found) {
                bta_dm_search_cb.services_found |= BTA_SERVICE_ID_TO_SERVICE_MASK(bta_dm_search_cb.service_index - 1);
            }
        }

        GKI_freebuf(bta_dm_search_cb.p_sdp_db);
        bta_dm_search_cb.p_sdp_db = NULL;

        if (bta_dm_search_cb.services_to_search) {
            bta_dm_find_services(bta_dm_search_cb.peer_bdaddr);
        } else {
            BTM_SecDeleteRmtNameNotifyCallback(&bta_dm_service_search_remname_cback);

            if ((p_msg = (tBTA_DM_MSG*)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL) {
                p_msg->hdr.event = BTA_DM_DISCOVERY_RESULT_EVT;
                p_msg->disc_result.result.disc_res.result = BTA_SUCCESS;
                p_msg->disc_result.result.disc_res.services = bta_dm_search_cb.services_found;
                bdcpy(p_msg->disc_result.result.disc_res.bd_addr, bta_dm_search_cb.peer_bdaddr);
                BCM_STRNCPY_S((char*)p_msg->disc_result.result.disc_res.bd_name, sizeof(BD_NAME), bta_dm_get_remname(),
                              BTA_DM_REMOTE_DEVICE_NAME_LENGTH);

                bta_sys_sendmsg(p_msg);
            }
        }
    } else {
        if (p_data->sdp_event.sdp_result == SDP_CONN_FAILED || p_data->sdp_event.sdp_result == SDP_CONN_REJECTED ||
            p_data->sdp_event.sdp_result == SDP_SECURITY_ERR) {
            bta_dm_search_cb.wait_disc = FALSE;
        }

        GKI_freebuf(bta_dm_search_cb.p_sdp_db);
        bta_dm_search_cb.p_sdp_db = NULL;

        BTM_SecDeleteRmtNameNotifyCallback(&bta_dm_service_search_remname_cback);

        if ((p_msg = (tBTA_DM_MSG*)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL) {
            p_msg->hdr.event = BTA_DM_DISCOVERY_RESULT_EVT;
            p_msg->disc_result.result.disc_res.result = BTA_FAILURE;
            p_msg->disc_result.result.disc_res.services = bta_dm_search_cb.services_found;
            bdcpy(p_msg->disc_result.result.disc_res.bd_addr, bta_dm_search_cb.peer_bdaddr);
            BCM_STRNCPY_S((char*)p_msg->disc_result.result.disc_res.bd_name, sizeof(BD_NAME), bta_dm_get_remname(),
                          BTA_DM_REMOTE_DEVICE_NAME_LENGTH);

            bta_sys_sendmsg(p_msg);
        }
    }
}

void bta_dm_search_cmpl(tBTA_DM_MSG* p_data) {
    bta_dm_search_cb.p_search_cback(BTA_DM_DISC_CMPL_EVT, NULL);
}

void bta_dm_disc_result(tBTA_DM_MSG* p_data) {
    tBTA_DM_MSG* p_msg;

    bta_dm_search_cb.p_search_cback(BTA_DM_DISC_RES_EVT, &p_data->disc_result.result);

    if ((p_msg = (tBTA_DM_MSG*)GKI_getbuf(sizeof(tBTA_DM_MSG))) != NULL) {
        p_msg->hdr.event = BTA_DM_SEARCH_CMPL_EVT;
        bta_sys_sendmsg(p_msg);
    }
}

void bta_dm_search_result(tBTA_DM_MSG* p_data) {
    if (p_data->disc_result.result.disc_res.services) {
        bta_dm_search_cb.p_search_cback(BTA_DM_DISC_RES_EVT, &p_data->disc_result.result);
    }

    if (!bta_dm_search_cb.wait_disc) {
        bta_dm_search_cb.wait_disc = FALSE;
        bta_dm_discover_next_device();
    } else {
        bta_dm_search_cb.sdp_results = TRUE;
        bta_dm_search_cb.search_timer.p_cback = (TIMER_CBACK*)&bta_dm_search_timer_cback;
        bta_sys_start_timer(&bta_dm_search_cb.search_timer, 0, 1000 * (L2CAP_LINK_INACTIVITY_TOUT + 1));
    }
}

static void bta_dm_search_timer_cback(TIMER_LIST_ENT* p_tle) {
    APPL_TRACE_EVENT0(" bta_dm_search_timer_cback  ");

    bta_dm_search_cb.wait_disc = FALSE;
    bta_dm_discover_next_device();
}

void bta_dm_free_sdp_db(tBTA_DM_MSG* p_data) {
    if (bta_dm_search_cb.p_sdp_db) {
        GKI_freebuf(bta_dm_search_cb.p_sdp_db);
        bta_dm_search_cb.p_sdp_db = NULL;
    }
}

void bta_dm_queue_search(tBTA_DM_MSG* p_data) {
    bta_dm_search_cb.p_search_queue = (tBTA_DM_MSG*)GKI_getbuf(sizeof(tBTA_DM_API_SEARCH));
    memcpy(bta_dm_search_cb.p_search_queue, p_data, sizeof(tBTA_DM_API_SEARCH));
}

void bta_dm_search_cancel_notify(tBTA_DM_MSG* p_data) {
    bta_dm_search_cb.p_search_cback(BTA_DM_SEARCH_CANCEL_CMPL_EVT, NULL);
}

static void bta_dm_start_remote_name_response_timer(void) {
    if (btm_cb.devcb.page_timeout < 30000) {
        bta_sys_start_timer(&bta_dm_search_cb.rnr_timer, BTA_DM_REMT_NAME_RESP_EVT, btm_cb.devcb.page_timeout + 100);
    }
}

static void bta_dm_stop_remote_name_response_timer(void) {
    bta_sys_stop_timer(&bta_dm_search_cb.rnr_timer);
}

static void bta_dm_remote_name_request_cmd_failed(UINT8* bd_addr) {
    tBTA_DM_DISC_RESULT* p_msg;

    if ((p_msg = (tBTA_DM_DISC_RESULT*)GKI_getbuf(sizeof(tBTA_DM_DISC_RESULT))) != NULL) {
        bdcpy(p_msg->result.disc_res.bd_addr, bd_addr);
        p_msg->result.disc_res.bd_name[0] = '\0';
        p_msg->hdr.event = BTA_DM_REMT_NAME_EVT;
        bta_sys_sendmsg(p_msg);
    }
}

static void bta_dm_init_compress(void) {
    memset(bta_dm_compress_srvcs, 0, sizeof(bta_dm_compress_srvcs));
    bta_sys_compress_register(&bta_dm_compress_cback);
    L2CA_RegisterCompression(&bta_dm_l2cap_server_compress_cback, 0);
}

static char* bta_dm_get_remname(void) {
    char* p_name = bta_dm_search_cb.peer_name;
    char* p_temp;

    if (*p_name == '\0') {
        if ((p_temp = BTM_SecReadDevName(bta_dm_search_cb.peer_bdaddr)) != NULL) {
            p_name = p_temp;
        }
    }

    return p_name;
}
