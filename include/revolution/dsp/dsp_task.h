#ifndef DSP_TASK_H
#define DSP_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "revolution/dsp.h"
#include "revolution/os.h"

extern DSPTaskInfo* __DSP_curr_task;
extern DSPTaskInfo* __DSP_first_task;
extern DSPTaskInfo* __DSP_last_task;
extern DSPTaskInfo* __DSP_tmp_task;
extern DSPTaskInfo* __DSP_rude_task;
extern BOOL __DSP_rude_task_pending;

void __DSPHandler(__OSInterrupt, OSContext *);

void __DSP_exec_task(DSPTaskInfo *, DSPTaskInfo *);
void __DSP_boot_task(DSPTaskInfo *);
void __DSP_insert_task(DSPTaskInfo *);
void __DSP_add_task(DSPTaskInfo *);
void __DSP_remove_task(DSPTaskInfo *);

#ifdef __cplusplus
}
#endif

#endif
