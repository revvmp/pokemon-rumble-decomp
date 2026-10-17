#include "revolution/dsp.h"
#include "revolution/dsp/dsp_debug.h"
#include "revolution/dsp/dsp_task.h"
#include "private/flipper.h"

static BOOL __DSP_init_flag = FALSE;

const char* __DSPVersion = "<< RVL_SDK - DSP \trelease build: Feb 27 2009 10:01:57 (0x4302_145) >>";

u32 DSPCheckMailToDSP(void) {
    return (__DSPRegs[0] >> 15) & 1;
}

u32 DSPCheckMailFromDSP(void) {
    return (__DSPRegs[2] >> 15) & 1;
}

u32 DSPReadMailFromDSP(void) {
    return (__DSPRegs[2] << 16) | __DSPRegs[3];
}

void DSPSendMailToDSP(u32 mail) {
    __DSPRegs[0] = (u16)(mail >> 16);
    __DSPRegs[1] = (u16)mail;
}

void DSPAssertInt(void) {
    BOOL enabled;
    u16 reg;

    enabled = OSDisableInterrupts();
    reg = __DSPRegs[5];
    reg = (u16)((reg & ~0xA8) | 0x2);
    __DSPRegs[5] = reg;
    OSRestoreInterrupts(enabled);
}

void DSPInit(void) {
    BOOL enabled;
    u16 reg;

    __DSP_debug_printf("DSPInit(): Build Date: %s %s\n", "Feb 27 2009", "10:01:57");

    if (__DSP_init_flag == TRUE) {
        return;
    }

    OSRegisterVersion(__DSPVersion);

    enabled = OSDisableInterrupts();
    __OSSetInterruptHandler(7, __DSPHandler);
    __OSUnmaskInterrupts(0x1000000);

    reg = __DSPRegs[5];
    reg = (u16)((reg & ~0xA8) | 0x800);
    __DSPRegs[5] = reg;

    reg = __DSPRegs[5];
    reg = (u16)(reg & ~0xAC);
    __DSPRegs[5] = reg;

    __DSP_first_task = __DSP_last_task = __DSP_curr_task = __DSP_tmp_task = NULL;
    __DSP_init_flag = TRUE;

    OSRestoreInterrupts(enabled);
}

BOOL DSPCheckInit(void) {
    return __DSP_init_flag;
}

DSPTaskInfo* DSPAddTask(DSPTaskInfo* pTask) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    __DSP_insert_task(pTask);
    pTask->state = 0;
    pTask->flags = 1;
    OSRestoreInterrupts(enabled);

    if (pTask == __DSP_first_task) {
        __DSP_boot_task(pTask);
    }

    return pTask;
}

DSPTaskInfo* DSPAssertTask(DSPTaskInfo* pTask) {
    BOOL enabled;

    enabled = OSDisableInterrupts();

    if (__DSP_curr_task == pTask) {
        __DSP_rude_task = pTask;
        __DSP_rude_task_pending = TRUE;
        OSRestoreInterrupts(enabled);
        return pTask;
    }

    if (pTask->priority < __DSP_curr_task->priority) {
        __DSP_rude_task = pTask;
        __DSP_rude_task_pending = TRUE;

        if (__DSP_curr_task->state == 1) {
            DSPAssertInt();
        }

        OSRestoreInterrupts(enabled);
        return pTask;
    }

    OSRestoreInterrupts(enabled);
    return NULL;
}
