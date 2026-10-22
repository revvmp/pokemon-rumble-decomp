#include <revolution/fa/puh_ker.h>

typedef struct UHFKerSemaphoreSlot {
    OSSemaphore* p_semaphore;
    s32 initial_count;
    s32 attribute;
} UHFKerSemaphoreSlot;

typedef struct UHFKerSemaphoreTable {
    UHFKerSemaphoreSlot slot[UHF_KER_SEMAPHORE_MAX];
    u32 used_count;
    OSSemaphore* p_lock;
    u32 reserved[4];
} UHFKerSemaphoreTable;

static UHFKerSemaphoreTable g_semaphore_table;
static OSSemaphore g_semaphore_pool[UHF_KER_SEMAPHORE_MAX];
static s32 g_lock_enabled;

static int uhf_ker_lock(void) {
    if (g_lock_enabled != 1 || g_semaphore_table.p_lock == NULL) {
        return -1;
    }
    OSWaitSemaphore(g_semaphore_table.p_lock);
    return 0;
}

static void uhf_ker_unlock(void) {
    if (g_lock_enabled == 1 && g_semaphore_table.p_lock != NULL) {
        OSSignalSemaphore(g_semaphore_table.p_lock);
    }
}

s32 uhf_ker_create_sem(u32 initial_count, int attribute) {
    int i;
    int wait_count;
    OSSemaphore* p_semaphore;

    if (initial_count > 1) {
        return -5;
    }
    if (attribute != 0) {
        return -5;
    }
    if (uhf_ker_lock() != 0) {
        return -5;
    }
    if (g_semaphore_table.used_count == UHF_KER_SEMAPHORE_MAX) {
        uhf_ker_unlock();
        return -24;
    }
    for (i = 0; i < UHF_KER_SEMAPHORE_MAX; i++) {
        if (g_semaphore_table.slot[i].p_semaphore == NULL) {
            p_semaphore = &g_semaphore_pool[i];
            OSInitSemaphore(p_semaphore, 1);
            for (wait_count = 1 - initial_count; wait_count > 0; wait_count--) {
                OSWaitSemaphore(p_semaphore);
            }
            g_semaphore_table.slot[i].p_semaphore = p_semaphore;
            g_semaphore_table.slot[i].initial_count = initial_count;
            g_semaphore_table.slot[i].attribute = attribute;
            g_semaphore_table.used_count++;
            uhf_ker_unlock();
            return i + 1;
        }
    }
    uhf_ker_unlock();
    return -24;
}

s32 uhf_ker_delete_sem(int semaphore_id) {
    UHFKerSemaphoreSlot* p_slot;

    if (semaphore_id == 0 || semaphore_id > UHF_KER_SEMAPHORE_MAX) {
        return -5;
    }
    if (uhf_ker_lock() != 0) {
        return -5;
    }
    p_slot = &g_semaphore_table.slot[semaphore_id - 1];
    if (p_slot->p_semaphore == NULL) {
        uhf_ker_unlock();
        return -5;
    }
    p_slot->p_semaphore = NULL;
    p_slot->initial_count = 0;
    p_slot->attribute = 0;
    g_semaphore_table.used_count--;
    uhf_ker_unlock();
    return 0;
}

s32 uhf_ker_get_sem(int semaphore_id) {
    if (semaphore_id == 0 || semaphore_id > UHF_KER_SEMAPHORE_MAX) {
        return -5;
    }
    if (g_semaphore_table.slot[semaphore_id - 1].p_semaphore == NULL) {
        return -5;
    }
    OSWaitSemaphore(g_semaphore_table.slot[semaphore_id - 1].p_semaphore);
    return 0;
}
