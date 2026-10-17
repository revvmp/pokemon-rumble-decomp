#include "revolution/dsp/dsp_task.h"
#include "revolution/dsp/dsp_debug.h"
#include "private/flipper.h"

typedef enum DSPTaskMail {
    DSP_MAIL_INIT = 0xDCD10000,
    DSP_MAIL_RESUME = 0xDCD10001,
    DSP_MAIL_YIELD = 0xDCD10002,
    DSP_MAIL_DONE = 0xDCD10003,
    DSP_MAIL_SYNC = 0xDCD10004
} DSPTaskMail;

typedef enum CPUTaskMail {
    CPU_MAIL_NEW_UCODE = 0xCDD10001,
    CPU_MAIL_RESET = 0xCDD10002,
    CPU_MAIL_CONTINUE = 0xCDD10003
} CPUTaskMail;

typedef enum DSPBootMail {
    BOOT_MAIL_IRAM_MMEM_ADDR = 0x80F3A001,
    BOOT_MAIL_IRAM_LENGTH = 0x80F3A002,
    BOOT_MAIL_DRAM_MMEM_ADDR = 0x80F3B002,
    BOOT_MAIL_IRAM_DSP_ADDR = 0x80F3C002,
    BOOT_MAIL_START_VECTOR = 0x80F3D001
} DSPBootMail;

DSPTaskInfo* __DSP_curr_task;
DSPTaskInfo* __DSP_first_task;
DSPTaskInfo* __DSP_last_task;
DSPTaskInfo* __DSP_tmp_task;
DSPTaskInfo* __DSP_rude_task;
BOOL __DSP_rude_task_pending;

static inline void SendMailToDSP(u32 mail) {
    DSPSendMailToDSP(mail);

    while (DSPCheckMailToDSP()) {
    }
}

void __DSPHandler(__OSInterrupt interrupt, OSContext* pContext) {
    OSContext exceptionContext;
    u16 reg;
    u32 mail;

    reg = __DSPRegs[5];
    reg = (u16)((reg & ~0x28) | 0x80);
    __DSPRegs[5] = reg;

    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);

    while (!DSPCheckMailFromDSP()) {
    }

    mail = DSPReadMailFromDSP();

    if ((__DSP_curr_task->flags & 2) && mail == DSP_MAIL_YIELD) {
        mail = DSP_MAIL_DONE;
    }

    switch (mail) {
    case DSP_MAIL_INIT:
        __DSP_curr_task->state = 1;

        if (__DSP_curr_task->init_cb != NULL) {
            __DSP_curr_task->init_cb(__DSP_curr_task);
        }
        break;
    case DSP_MAIL_RESUME:
        __DSP_curr_task->state = 1;

        if (__DSP_curr_task->res_cb != NULL) {
            __DSP_curr_task->res_cb(__DSP_curr_task);
        }
        break;
    case DSP_MAIL_YIELD:
        if (__DSP_rude_task_pending) {
            if (__DSP_curr_task == __DSP_rude_task) {
                SendMailToDSP(CPU_MAIL_CONTINUE);

                __DSP_rude_task = NULL;
                __DSP_rude_task_pending = FALSE;

                if (__DSP_curr_task->res_cb != NULL) {
                    __DSP_curr_task->res_cb(__DSP_curr_task);
                }
            } else {
                SendMailToDSP(CPU_MAIL_NEW_UCODE);

                __DSP_exec_task(__DSP_curr_task, __DSP_rude_task);
                __DSP_curr_task->state = 2;
                __DSP_curr_task = __DSP_rude_task;
                __DSP_rude_task = NULL;
                __DSP_rude_task_pending = FALSE;
            }
        } else if (__DSP_curr_task->next == NULL) {
            if (__DSP_curr_task == __DSP_first_task) {
                SendMailToDSP(CPU_MAIL_CONTINUE);

                if (__DSP_curr_task->res_cb != NULL) {
                    __DSP_curr_task->res_cb(__DSP_curr_task);
                }
            } else {
                SendMailToDSP(CPU_MAIL_NEW_UCODE);

                __DSP_exec_task(__DSP_curr_task, __DSP_first_task);
                __DSP_curr_task->state = 2;
                __DSP_curr_task = __DSP_first_task;
            }
        } else {
            SendMailToDSP(CPU_MAIL_NEW_UCODE);

            __DSP_exec_task(__DSP_curr_task, __DSP_curr_task->next);
            __DSP_curr_task->state = 2;
            __DSP_curr_task = __DSP_curr_task->next;
        }
        break;
    case DSP_MAIL_DONE:
        if (__DSP_rude_task_pending) {
            if (__DSP_curr_task != __DSP_rude_task) {
                if (__DSP_curr_task->done_cb != NULL) {
                    __DSP_curr_task->done_cb(__DSP_curr_task);
                }

                SendMailToDSP(CPU_MAIL_NEW_UCODE);

                __DSP_exec_task(NULL, __DSP_rude_task);
                __DSP_remove_task(__DSP_curr_task);
                __DSP_curr_task = __DSP_rude_task;
                __DSP_rude_task = NULL;
                __DSP_rude_task_pending = FALSE;
                break;
            }

            __DSP_rude_task = NULL;
            __DSP_rude_task_pending = FALSE;
        }

        if (__DSP_curr_task->next == NULL) {
            if (__DSP_curr_task == __DSP_first_task) {
                if (__DSP_curr_task->done_cb != NULL) {
                    __DSP_curr_task->done_cb(__DSP_curr_task);
                }

                SendMailToDSP(CPU_MAIL_RESET);

                __DSP_curr_task->state = 3;
                __DSP_remove_task(__DSP_curr_task);
            } else {
                if (__DSP_curr_task->done_cb != NULL) {
                    __DSP_curr_task->done_cb(__DSP_curr_task);
                }

                SendMailToDSP(CPU_MAIL_NEW_UCODE);

                __DSP_curr_task->state = 3;
                __DSP_exec_task(NULL, __DSP_first_task);
                __DSP_curr_task = __DSP_first_task;
                __DSP_remove_task(__DSP_last_task);
            }
        } else {
            if (__DSP_curr_task->done_cb != NULL) {
                __DSP_curr_task->done_cb(__DSP_curr_task);
            }

            SendMailToDSP(CPU_MAIL_NEW_UCODE);

            __DSP_curr_task->state = 3;
            __DSP_exec_task(NULL, __DSP_curr_task->next);
            __DSP_curr_task = __DSP_curr_task->next;
            __DSP_remove_task(__DSP_curr_task->prev);
        }
        break;
    case DSP_MAIL_SYNC:
        if (__DSP_curr_task->req_cb != NULL) {
            __DSP_curr_task->req_cb(__DSP_curr_task);
        }
        break;
    default:
        break;
    }

    OSClearContext(&exceptionContext);
    OSSetCurrentContext(pContext);
}

void __DSP_exec_task(DSPTaskInfo* pCurrent, DSPTaskInfo* pNext) {
    if (pCurrent != NULL) {
        SendMailToDSP((u32)pCurrent->dram_mem_addr);
        SendMailToDSP(pCurrent->dram_len);
        SendMailToDSP(pCurrent->dram_addr);
    } else {
        SendMailToDSP(0);
        SendMailToDSP(0);
        SendMailToDSP(0);
    }

    SendMailToDSP((u32)pNext->iram_mmem_addr);
    SendMailToDSP(pNext->iram_length);
    SendMailToDSP(pNext->iram_addr);

    if (pNext->state == 0) {
        SendMailToDSP(pNext->dsp_vector);
        SendMailToDSP(0);
        SendMailToDSP(0);
        SendMailToDSP(0);
    } else {
        SendMailToDSP(pNext->dsp_res_vector);
        SendMailToDSP((u32)pNext->dram_mem_addr);
        SendMailToDSP(pNext->dram_len);
        SendMailToDSP(pNext->dram_addr);
    }
}

void __DSP_boot_task(DSPTaskInfo* pTask) {
    volatile u32 mail;

    while (!DSPCheckMailFromDSP()) {
    }

    mail = DSPReadMailFromDSP();

    SendMailToDSP(BOOT_MAIL_IRAM_MMEM_ADDR);
    SendMailToDSP((u32)pTask->iram_mmem_addr);
    SendMailToDSP(BOOT_MAIL_IRAM_DSP_ADDR);
    SendMailToDSP(pTask->iram_addr & 0xFFFF);
    SendMailToDSP(BOOT_MAIL_IRAM_LENGTH);
    SendMailToDSP(pTask->iram_length);
    SendMailToDSP(BOOT_MAIL_DRAM_MMEM_ADDR);
    SendMailToDSP(0);
    SendMailToDSP(BOOT_MAIL_START_VECTOR);
    SendMailToDSP(pTask->dsp_vector);

    __DSP_debug_printf("DSP is booting task: 0x%08X\n", pTask);
    __DSP_debug_printf("__DSP_boot_task()  : IRAM MMEM ADDR: 0x%08X\n", pTask->iram_mmem_addr);
    __DSP_debug_printf("__DSP_boot_task()  : IRAM DSP ADDR : 0x%08X\n", pTask->iram_addr);
    __DSP_debug_printf("__DSP_boot_task()  : IRAM LENGTH   : 0x%08X\n", pTask->iram_length);
    __DSP_debug_printf("__DSP_boot_task()  : DRAM MMEM ADDR: 0x%08X\n", pTask->dram_len);
    __DSP_debug_printf("__DSP_boot_task()  : Start Vector  : 0x%08X\n", pTask->dsp_vector);
}

void __DSP_insert_task(DSPTaskInfo* pTask) {
    DSPTaskInfo* pTemp;

    if (__DSP_first_task == NULL) {
        __DSP_first_task = __DSP_last_task = __DSP_curr_task = pTask;
        pTask->next = pTask->prev = NULL;
        return;
    }

    pTemp = __DSP_first_task;

    while (pTemp != NULL) {
        if (pTask->priority < pTemp->priority) {
            pTask->prev = pTemp->prev;
            pTemp->prev = pTask;
            pTask->next = pTemp;

            if (pTask->prev == NULL) {
                __DSP_first_task = pTask;
            } else {
                pTask->prev->next = pTask;
            }
            break;
        }

        pTemp = pTemp->next;
    }

    if (pTemp == NULL) {
        __DSP_last_task->next = pTask;
        pTask->next = NULL;
        pTask->prev = __DSP_last_task;
        __DSP_last_task = pTask;
    }
}

void __DSP_add_task(DSPTaskInfo* pTask) {
    if (__DSP_last_task == NULL) {
        __DSP_first_task = __DSP_last_task = __DSP_curr_task = pTask;
        pTask->next = pTask->prev = NULL;
    } else {
        __DSP_last_task->next = pTask;
        pTask->next = NULL;
        pTask->prev = __DSP_last_task;
        __DSP_last_task = pTask;
    }

    pTask->state = 0;

    __DSP_debug_printf("__DSP_add_task() : Added task    : 0x%08X\n", pTask);
}

void __DSP_remove_task(DSPTaskInfo* pTask) {
    pTask->flags = 0;
    pTask->state = 3;

    if (__DSP_first_task == pTask) {
        if (pTask->next != NULL) {
            __DSP_first_task = pTask->next;
            pTask->next->prev = NULL;
        } else {
            __DSP_first_task = __DSP_last_task = __DSP_curr_task = NULL;
        }
        return;
    }

    if (__DSP_last_task == pTask) {
        __DSP_last_task = pTask->prev;
        pTask->prev->next = NULL;
        __DSP_curr_task = __DSP_first_task;
        return;
    }

    __DSP_curr_task = pTask->next;
    pTask->prev->next = pTask->next;
    pTask->next->prev = pTask->prev;
}
