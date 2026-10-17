#ifndef REVOLUTION_ENC_TYPES_H
#define REVOLUTION_ENC_TYPES_H

#include <revolution/types.h>

#define ENC_DST_INVALID (-1)
#define ENC_SRC_UNLIMITED (-1)

typedef enum ENCResult {
    ENC_OK = 0,
    ENC_ERR_NO_BUF_LEFT = -1,
    ENC_ERR_NO_MAP_RULE = -2,
    ENC_ERR_INVALID_PARAM = -3,
    ENC_ERR_INVALID_FORMAT = -4,
    ENC_ERR_UNKNOWN_ENCODING = -5,
    ENC_ERR_UNSUPPORTED = -6,
    ENC_ERR_NOT_LOADED = -7
} ENCResult;

typedef enum ENCBreakType {
    ENC_BR_KEEP,
    ENC_BR_CRLF,
    ENC_BR_CR,
    ENC_BR_LF
} ENCBreakType;

#endif
