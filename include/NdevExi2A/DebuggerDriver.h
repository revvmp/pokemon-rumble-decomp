#ifndef NDEVEXI2A_DEBUGGERDRIVER_H
#define NDEVEXI2A_DEBUGGERDRIVER_H

#include "NdevExi2A/exi2.h"
#include "revolution/os/OSInterrupt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DB_USB_CSR 0x34000000
#define DB_MAILBOX_READ 0x34000200
#define DB_MAILBOX_WRITE 0xB4000100

#define DB_CSR_SEND_BUSY 0x4
#define DB_CSR_RECV_EMPTY 0x8

#define DB_BUFFER_BASE 0xD10000
#define DB_RECV_OFFSET 0x1000
#define DB_PAGE_SIZE 0x800

#define ROUND_UP(x, align) (((x) + (align) - 1) & -(align))

void __DBMtrHandler(__OSInterrupt type, OSContext* ctx);
void __DBIntrHandler(__OSInterrupt type, OSContext* ctx);
void DBInitComm(u8** flagOut, __OSInterruptHandler handler);
void DBInitInterrupts(void);
u32 DBQueryData(void);
BOOL DBRead(void* dst, u32 size);
BOOL DBWrite(const void* src, u32 size);
void DBOpen(void);
void DBClose(void);

static inline u32 ODEMUGenMailData(u32 ofs, u32 size) {
    return (ofs & 0xFF) << 16 | 0x1F000000 | size & 0x1FFF;
}

static inline u32 ODEMUGetPage(u32 mail) {
    return (mail & 0xFF0000) >> 16;
}

static inline u32 ODEMUGetPc2NngcOffset(u32 mail) {
    if (!(ODEMUGetPage(mail) & 0x1)) {
        return 0;
    }

    return DB_PAGE_SIZE;
}

static inline BOOL ODEMUIsValidMail(u32 mail) {
    return (mail & 0x1F000000) == 0x1F000000;
}

static inline u32 ODEMUGetSize(u32 mail) {
    return mail & 0x1FFF;
}

static inline u32 __DBRamAddress(u32 ofs) {
    return ((ofs + DB_BUFFER_BASE) * 0x40) & 0x3FFFFF00;
}

static inline BOOL __DBReadMailbox(u32* mailOut) {
    return __DBEXIReadReg(DB_MAILBOX_READ, mailOut, sizeof(*mailOut));
}

static inline BOOL __DBRead(u32 ofs, void* dest, u32 size) {
    return __DBEXIReadRam(__DBRamAddress(ofs), dest, size);
}

static inline BOOL __DBWriteMailbox(u32 mail) {
    return __DBEXIWriteReg(DB_MAILBOX_WRITE, &mail, sizeof(mail));
}

static inline BOOL __DBWrite(u32 ofs, const void* src, u32 size) {
    return __DBEXIWriteRam(__DBRamAddress(ofs) | 0x80000000, src, size);
}

#ifdef __cplusplus
}
#endif

#endif
