#ifndef OS_DOLPHIN_DOLPHIN_TRK_GLUE_H
#define OS_DOLPHIN_DOLPHIN_TRK_GLUE_H

#include <MSL_C/UART.h>
#include <MetroTRK/trk.h>
#include <revolution/os.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*DBCommFunc)(void);
typedef int (*DBCommInitFunc)(void*, __OSInterruptHandler);
typedef int (*DBCommReadFunc)(u8*, int);
typedef int (*DBCommWriteFunc)(const u8*, int);

typedef struct DBCommTable {
    DBCommInitFunc initialize_func;
    DBCommFunc init_interrupts_func;
    DBCommFunc shutdown_func;
    DBCommFunc peek_func;
    DBCommReadFunc read_func;
    DBCommWriteFunc write_func;
    DBCommFunc open_func;
    DBCommFunc close_func;
    DBCommFunc pre_continue_func;
    DBCommFunc post_stop_func;
} DBCommTable;

extern u8 TRK_Use_BBA;
extern DBCommTable gDBCommTable;

void TRKLoadContext(OSContext* context, u32 exceptionID);
void TRKEXICallBack(s16 interrupt, OSContext* context);
int InitMetroTRKCommTable(int hardwareID);
void TRKUARTInterruptHandler(void);
DSError TRKInitializeIntDrivenUART(BOOL interruptDriven, BOOL rxInterrupts, void** inputPending);
void EnableEXI2Interrupts(void);
int TRKPollUART(void);
UARTError TRKReadUARTN(void* bytes, u32 length);
UARTError TRKWriteUARTN(const void* bytes, u32 length);
void ReserveEXI2Port(void);
void UnreserveEXI2Port(void);
void TRK_board_display(const char* message);
void InitializeProgramEndTrap(void);

#ifdef __cplusplus
}
#endif

#endif
