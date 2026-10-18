#include <revolution/fa/pf_filelock.h>
#include <revolution/fa/pf_system.h>
#include <revolution/fa/pf_kernel.h>

void PF_InitLockFile(void) {
    pf_sys_set.flock_count = 0;
}

s32 PF_LockFile(PF_FILE* p_file, PF_FILE* p_owner) {
    s32 result = -1;

    if (p_file->p_sfd->lock.count == 0 && p_file->p_sfd->lock.resource == NULL) {
        if (pf_sys_set.flock_count < 0xFFFFFFFF) {
            PFK_SEMAPHORE_PARAMS config;
            config.initial_count = 1;
            config.maximum_count = 1;
            config.semaphore = &p_file->p_sfd->lock.semaphore;
            p_file->p_sfd->lock.resource = pfk_create_semaphore(&config);
            pf_sys_set.flock_count++;
            result = 0;
        } else {
            result = -1;
        }
    }
    pfk_get_semaphore(p_file->p_sfd->lock.resource);
    return result;
}

s32 PF_UnLockFile(PF_FILE* p_file) {
    pfk_release_semaphore(p_file->p_sfd->lock.resource);
    if (p_file->p_sfd->lock.count == 0 && p_file->p_sfd->lock.wcount == 0 && pf_sys_set.flock_count != 0) {
        pfk_delete_semaphore(p_file->p_sfd->lock.resource);
        p_file->p_sfd->lock.resource = NULL;
        pf_sys_set.flock_count--;
    }
    return 0;
}
