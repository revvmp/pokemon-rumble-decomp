#ifndef REVOLUTION_ENC_UTILITY_H
#define REVOLUTION_ENC_UTILITY_H

#include <revolution/enc/enctypes.h>

#define CREATE_STATE_VARIABLES(dstCnt_, dstLimit_, dstValid_, srcCnt_, srcLimit_, srcLimited_) \
    s32(srcCnt_) = 0; \
    s32(dstCnt_) = 0; \
 \
    s32(srcLimit_) = ENC_SRC_UNLIMITED; \
    s32(dstLimit_) = ENC_DST_INVALID; \
 \
    BOOL(dstValid_) = TRUE; \
    BOOL(srcLimited_) = TRUE;

#define CHECK_PARAMETERS(ret_, dstStream_, dstSize_, dstLimit_, dstValid_, srcStream_, srcSize_, srcLimit_, srcLimited_) \
    do { \
        *(ret_) = ENCiCheckParameters((dstStream_) != NULL, dstSize_, dstLimit_, dstValid_, (srcStream_) != NULL, srcSize_, srcLimit_, srcLimited_); \
 \
        if (*(ret_) != ENC_OK) \
            return *(ret_); \
    } while (0)

#define UNSAFE_THROW_AND_QUIT(ret_, err_) \
    { \
        *(ret_) = (err_); \
        break; \
    }

#define UNSAFE_CHECK_DST_SPACE(cnt_, limit_, valid_, ret_) \
    if ((cnt_) >= (limit_) && (valid_)) { \
        UNSAFE_THROW_AND_QUIT(ret_, ENC_ERR_NO_BUF_LEFT); \
    }

#define UNSAFE_CHECK_BREAK_TYPE(dstStream_, dstCnt_, dstLimit_, dstValid_, srcStream_, srcCnt_, srcLimit_, srcLimited_, breakType_, ret_) \
    if ((breakType_) > ENC_BR_KEEP) { \
        int srcBrkLen = ENCiCheckBreakType((*(srcStream_))[0], (srcLimit_) - *(srcCnt_) > 1 || !(srcLimited_) ? (*(srcStream_))[1] : 0x00); \
 \
        if (srcBrkLen > 0) { \
            UNSAFE_CHECK_DST_BREAK_TYPE(dstStream_, dstCnt_, dstLimit_, dstValid_, srcStream_, srcCnt_, breakType_, ret_); \
        } \
    }

#define UNSAFE_CHECK_DST_BREAK_TYPE(dstStream_, dstCnt_, dstLimit_, dstValid_, srcStream_, srcCnt_, breakType_, ret_) \
    { \
        int dstBrkLen = ENCiWriteBreakType(*(dstStream_), sizeof(**(dstStream_)), breakType_, dstValid_); \
 \
        if ((dstLimit_) - *(dstCnt_) < dstBrkLen && (dstValid_)) { \
            UNSAFE_THROW_AND_QUIT(ret_, ENC_ERR_NO_BUF_LEFT); \
        } \
 \
        *(srcStream_) += srcBrkLen; \
        *(srcCnt_) += srcBrkLen; \
        *(dstCnt_) += dstBrkLen; \
 \
        if (dstValid_) \
            *(dstStream_) += dstBrkLen; \
 \
        continue; \
    }

#define WRITE_CHAR(char_, dstStream_, dstCnt_, dstValid_, srcStream_, srcCnt_) \
    do { \
        if (dstValid_) { \
            **(dstStream_) = (char_); \
            ++*(dstStream_); \
        } \
 \
        ++*(dstCnt_); \
        ++*(srcStream_); \
        ++*(srcCnt_); \
    } while (0)

#define INCREMENT_STREAMS(dstStream_, dstCnt_, dstValid_, srcStream_, srcCnt_) \
    do { \
        if (dstValid_) { \
 \
            ++*(dstStream_); \
        } \
 \
        ++*(dstCnt_); \
        ++*(srcStream_); \
        ++*(srcCnt_); \
    } while (0)

#define WRITE_BACK_SIZES(srcSize_, srcCnt_, dstSize_, dstCnt_) \
    do { \
        if (srcSize_) \
            *(srcSize_) = (srcCnt_); \
 \
        if (dstSize_) \
            *(dstSize_) = (dstCnt_); \
    } while (0)

#define IS_ASCII(x) ((x) < 0x80)

#define UTF16_BE_BOM 0xFEFF
#define UTF16_LE_BOM 0xFFFE

#define UTF16_CHECK_BOM(stream_, srcSize_, limit_, limited_, cnt_, dstSize_) \
    do { \
        if ((limit_) > 0 || !(limited_)) { \
            if (**(stream_) == UTF16_BE_BOM) { \
                ++*(cnt_); \
                ++*(stream_); \
            } else if (**(stream_) == UTF16_LE_BOM) { \
                if (dstSize_) \
                    *(dstSize_) = 0; \
 \
                *(srcSize_) = 0; \
                return ENC_ERR_INVALID_FORMAT; \
            } \
        } \
    } while (0)

#ifdef __cplusplus
extern "C" {
#endif

void ENCiRegisterVersion(void);
ENCResult ENCiCheckParameters(BOOL dstValid, s32* dstSizeIn, s32* dstSizeOut, BOOL* dstValidOut, BOOL srcValid, s32* srcSizeIn, s32* srcSizeOut,
                              BOOL* srcLimitedOut);
int ENCiCheckBreakType(unsigned a, unsigned b);
int ENCiWriteBreakType(void* stream, int width, ENCBreakType breakType, BOOL valid);

#ifdef __cplusplus
}
#endif

#endif
