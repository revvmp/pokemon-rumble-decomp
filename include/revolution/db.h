#ifndef REVOLUTION_DB_H
#define REVOLUTION_DB_H

#include "revolution/os.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DBInterface {
    u32 bPresent;
    u32 exceptionMask;
    void (*ExceptionDestination)(void);
    void* exceptionReturn;
} DBInterface;

extern DBInterface* __DBInterface;
extern BOOL DBVerbose;

void DBInit(void);
void __DBExceptionDestinationAux(void);
void __DBExceptionDestination(void);
BOOL __DBIsExceptionMarked(__OSException exception);
void DBPrintf(char* format, ...);

#ifdef __cplusplus
}
#endif

#endif
