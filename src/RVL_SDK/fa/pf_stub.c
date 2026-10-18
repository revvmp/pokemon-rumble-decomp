#include <private/fa/pf_stub.h>
#include <revolution/fa/pf_system.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_kernel.h>

extern PDM_DISK* gOpenDisk[26];
extern PDM_PARTITION* gOpenPartition[26];

u32 pfd_sddrv_is_media_insert(void);
u32 pfd_mscdrv_is_media_insert(PDM_DISK* p_disk);

void pfstub_init_stub(void) {
    if (!pf_sys_set.stub.initialized) {
        PFK_MAILBOX_PARAMS mailbox;
        PFK_TASK_PARAMS task;

        mailbox.queue = &pf_sys_set.stub.mailbox;
        mailbox.messages = pf_sys_set.stub.messages;
        mailbox.capacity = 20;
        pf_sys_set.stub.p_mailbox = pfk_create_mailbox(&mailbox);

        task.entry = pfstub_entry;
        task.argument = NULL;
        task.priority = 16;
        task.stack_size = sizeof(pf_sys_set.stub.stack);
        task.stack = pf_sys_set.stub.stack;
        task.thread = &pf_sys_set.stub.thread;
        pf_sys_set.stub.p_thread = pfk_create_task(&task);
        pfk_start_task(pf_sys_set.stub.p_thread);
        pf_sys_set.stub.initialized = 1;
    }
}

void* pfstub_entry(void* p_parameter) {
    OSMessage message = NULL;
    PF_STUB_COMMAND* p_command;
    int command;

    for (;;) {
        if (pfk_receive_message(pf_sys_set.stub.p_mailbox, &message) != 0) {
            continue;
        }
        p_command = message;
        if (p_command == NULL) {
            continue;
        }
        pf_vol_set.current_context_id = p_command->context_id;
        if ((command = p_command->command) >= 0 && command < 100) {
            pfstub_call_function_standard(p_command);
        } else if ((command = p_command->command) >= 100 && command < 200) {
            pfstub_call_function_unicode(p_command);
        } else {
            p_command->result = -1;
        }

        if (p_command->callback == NULL) {
            pfk_release_semaphore(p_command->p_semaphore);
            continue;
        }

        if (p_command->command == 27) {
            s32 drive = p_command->driver_table[0]->drive - 'A';
            gOpenDisk[drive] = p_command->p_disk;
            gOpenPartition[drive] = p_command->p_partition;
            if (p_command->first.device_type == 0) {
                if (pfd_sddrv_is_media_insert()) {
                    p_command->driver_table[0]->stat |= 0x10;
                }
            } else if (p_command->first.device_type == 1) {
                if (pfd_mscdrv_is_media_insert(p_command->p_disk)) {
                    p_command->driver_table[0]->stat |= 0x10;
                }
            }
        }

        p_command->callback(pfstub_store_result(p_command), p_command->p_parameter);
    }
}

s32 pfstub_com_massage(PF_STUB_COMMAND* p_command) {
    OSThread* p_thread;
    PFK_SEMAPHORE_PARAMS config;
    OSSemaphore semaphore;

    if (p_command->callback == NULL) {
        config.initial_count = 0;
        config.maximum_count = 1;
        config.semaphore = &semaphore;
        p_command->p_semaphore = pfk_create_semaphore(&config);
    } else {
        p_command->p_semaphore = NULL;
    }
    if (pfk_get_task_id(&p_thread) != 0) {
        return -1;
    }
    p_command->context_id = (s32)p_thread;
    if (pfk_send_message(pf_sys_set.stub.p_mailbox, p_command) != 0) {
        return -1;
    }
    if (p_command->callback == NULL) {
        s32 status = pfk_get_semaphore(p_command->p_semaphore);
        pfk_delete_semaphore(p_command->p_semaphore);
        if (status != 0) {
            return -1;
        }
    }
    return 0;
}

s32 pfstub_store_result(PF_STUB_COMMAND* p_command) {
    s32 status = 0;
    if (p_command->p_result != NULL) {
        *p_command->p_result = p_command->result;
        switch (p_command->command) {
        case 2:
            break;
        case 0:
        case 1:
        case 54:
        case 100:
        case 101:
        case 122:
            if (p_command->result == 0) {
                status = -1;
            }
            break;
        case 35:
            if (p_command->result != p_command->first.count) {
                status = -1;
            }
            break;
        case 3:
        case 4:
            if (p_command->result != p_command->second.count) {
                status = -1;
            }
            break;
        default:
            break;
        }
    } else {
        status = p_command->result;
    }
    return status;
}
