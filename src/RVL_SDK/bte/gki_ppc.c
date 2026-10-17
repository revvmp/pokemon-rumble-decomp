#include <revolution/bte/gki_int.h>
#include <revolution/os/OSInterrupt.h>
#include <string.h>

static UINT8 GKI_init_btu_task(UINT8 task_id);

extern void* App_MEMalloc(size_t size);
extern int App_MEMfree(void* ptr);

tGKI_CB gki_cb;

void GKI_init(void) {
    memset(&gki_cb, 0, sizeof gki_cb);

    gki_buffer_init();
    gki_timers_init();

    GKI_init_btu_task(PPC_TASK);

    gki_cb.os.int_index = 0;
    gki_cb.os.int_state[gki_cb.os.int_index] = OSEnableInterrupts();
    ++gki_cb.os.int_index;
}

void GKI_shutdown(void) {
    GKI_exit_task(PPC_TASK);

    while (gki_cb.os.int_index > 0) {
        GKI_enable();
    }
}

static UINT8 GKI_init_btu_task(UINT8 task_id) {
    if (task_id >= GKI_MAX_TASKS) {
        return GKI_FAILURE;
    }

    gki_cb.com.OSRdyTbl[task_id] = TASK_READY;
    gki_cb.com.OSTName[task_id] = NULL;
    gki_cb.com.OSWaitTmr[task_id] = 0;
    gki_cb.com.OSWaitEvt[task_id] = 0;

    return GKI_SUCCESS;
}

void GKI_run(void* p_task_id) {}

void GKI_exit_task(UINT8 task_id) {
    GKI_disable();

    gki_cb.com.OSRdyTbl[task_id] = TASK_DEAD;

    GKI_enable();
}

void GKI_sched_lock(void) {}

void GKI_sched_unlock(void) {}

void GKI_delay(UINT32 timeout) {}

UINT8 GKI_send_event(UINT8 task_id, UINT16 event) {
    if (task_id >= GKI_MAX_TASKS) {
        return GKI_FAILURE;
    }

    GKI_disable();

    gki_cb.com.OSWaitEvt[task_id] |= event;

    GKI_enable();

    return GKI_SUCCESS;
}

UINT8 GKI_get_taskid(void) {
    return PPC_TASK;
}

void GKI_enable(void) {
    --gki_cb.os.int_index;

    OSRestoreInterrupts(gki_cb.os.int_state[gki_cb.os.int_index]);
}

void GKI_disable(void) {
    BOOL intrStatus;

    intrStatus = OSDisableInterrupts();

    gki_cb.os.int_state[gki_cb.os.int_index] = intrStatus;
    ++gki_cb.os.int_index;
}

void GKI_exception(UINT16 code, char* msg) {}

void* GKI_os_malloc(UINT32 size) {
    void* mem = NULL;

    mem = App_MEMalloc(size);

    if (mem == 0) {
        return NULL;
    }

    return mem;
}

void GKI_os_free(void* p_mem) {
    App_MEMfree(p_mem);
}
