#ifndef NCD_H
#define NCD_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef s32 NCDErr;

#define NCD_MAC_ADDRESS_LENGTH 6

#define NCD_RESULT_SUCCESS 0
#define NCD_RESULT_FATAL -1
#define NCD_RESULT_UNDECIDED -2
#define NCD_RESULT_ILLEGAL_PARAM -3
#define NCD_RESULT_NOT_THREAD -5
#define NCD_RESULT_INPROGRESS -8

NCDErr NCDiGetWirelessMacAddress(u8* macAddr);

#ifdef __cplusplus
}
#endif

#endif
