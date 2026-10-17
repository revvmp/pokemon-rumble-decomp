#ifndef RVL_SDK_ENC_UTILITY_H
#define RVL_SDK_ENC_UTILITY_H

#include <revolution/enc/encutility.h>
#include <revolution/types.h>

#include <revolution/enc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DECLARE_PRIVATE_ENC_FUNCTION(srcEnc_, srcChar_, dstEnc_, dstChar_)                                                                           \
    ENCResult ENCiConvertString##srcEnc_##To##dstEnc_(dstChar_* dst, s32* dstSize, srcChar_ const* src, s32* srcSize, ENCBreakType breakType);

#define DECLARE_PRIVATE_ENC_FUNCTION_WITH_MB_STATE(srcEnc_, srcChar_, dstEnc_, dstChar_)                                                             \
    ENCResult ENCiConvertString##srcEnc_##To##dstEnc_(dstChar_* dst, s32* dstSize, srcChar_ const* src, s32* srcSize, ENCBreakType breakType,        \
                                                      ENCMBState* mbState);

#define DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(srcEnc_) DECLARE_PRIVATE_ENC_FUNCTION(srcEnc_, u8, Unicode, u16)
#define DECLARE_PRIVATE_ENC_FUNCTION_FROM_UTF16(dstEnc_) DECLARE_PRIVATE_ENC_FUNCTION(Unicode, u16, dstEnc_, u8)

#define DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16_WITH_MB_STATE(srcEnc_) DECLARE_PRIVATE_ENC_FUNCTION_WITH_MB_STATE(srcEnc_, u8, Unicode, u16)
#define DECLARE_PRIVATE_ENC_FUNCTION_FROM_UTF16_WITH_MB_STATE(dstEnc_) DECLARE_PRIVATE_ENC_FUNCTION_WITH_MB_STATE(Unicode, u16, dstEnc_, u8)

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_VIA(srcEnc_, srcChar_, srcVia_, dstEnc_, dstChar_, dstVia_)                                                     \
    ENCResult ENCConvertString##srcEnc_##To##dstEnc_(dstChar_* dst, s32* dstSize, srcChar_ const* src, s32* srcSize) {                               \
        return ENCiConvertString##srcVia_##To##dstVia_(dst, dstSize, src, srcSize, ENC_BR_KEEP);                                                     \
    }

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE_VIA(srcEnc_, srcChar_, srcVia_, dstEnc_, dstChar_, dstVia_)                                       \
    ENCResult ENCConvertString##srcEnc_##To##dstEnc_(dstChar_* dst, s32* dstSize, srcChar_ const* src, s32* srcSize) {                               \
        return ENCiConvertString##srcVia_##To##dstVia_(dst, dstSize, src, srcSize, ENC_BR_KEEP, NULL);                                               \
    }

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_GIVEN_MB_STATE_VIA(srcEnc_, srcChar_, srcVia_, dstEnc_, dstChar_, dstVia_, mbState_)                       \
    ENCResult ENCConvertString##srcEnc_##To##dstEnc_(dstChar_* dst, s32* dstSize, srcChar_ const* src, s32* srcSize) {                               \
        ENCMBState mbState = (mbState_);                                                                                                             \
                                                                                                                                                     \
        return ENCiConvertString##srcVia_##To##dstVia_(dst, dstSize, src, srcSize, ENC_BR_KEEP, &mbState);                                           \
    }

#define DEFINE_PUBLIC_ENC_TRAMPOLINE(srcEnc_, srcChar_, dstEnc_, dstChar_)                                                                           \
    DEFINE_PUBLIC_ENC_TRAMPOLINE_VIA(srcEnc_, srcChar_, srcEnc_, dstEnc_, dstChar_, dstEnc_)

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE(srcEnc_, srcChar_, dstEnc_, dstChar_)                                                             \
    DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE_VIA(srcEnc_, srcChar_, srcEnc_, dstEnc_, dstChar_, dstEnc_)

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_GIVEN_MB_STATE(srcEnc_, srcChar_, dstEnc_, dstChar_, mbState_)                                             \
    DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_GIVEN_MB_STATE_VIA(srcEnc_, srcChar_, srcEnc_, dstEnc_, dstChar_, dstEnc_, mbState_)

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_FROM_UTF16(dstEnc_) DEFINE_PUBLIC_ENC_TRAMPOLINE_VIA(Unicode, u16, Unicode, dstEnc_, u8, dstEnc_)

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_TO_UTF16_VIA(srcEnc_, srcVia_) DEFINE_PUBLIC_ENC_TRAMPOLINE_VIA(srcEnc_, u8, srcVia_, Unicode, u16, Unicode)

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_TO_UTF16(srcEnc_) DEFINE_PUBLIC_ENC_TRAMPOLINE_VIA(srcEnc_, u8, srcEnc_, Unicode, u16, Unicode)

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE_FROM_UTF16(dstEnc_)                                                                               \
    DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE_VIA(Unicode, u16, Unicode, dstEnc_, u8, dstEnc_)

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE_TO_UTF16_VIA(srcEnc_, srcVia_)                                                                    \
    DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE_VIA(srcEnc_, u8, srcVia_, Unicode, u16, Unicode)

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE_TO_UTF16(srcEnc_)                                                                                 \
    DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE_VIA(srcEnc_, u8, srcEnc_, Unicode, u16, Unicode)










#define UNSAFE_WRITE_MULTI_BYTE_CHAR(mb_, dstStream_, dstCnt_, dstLimit_, dstValid_, srcStream_, srcCnt_, ret_)                                      \
    {                                                                                                                   \
        u8 b1 = (mb_)[0];                                                                                                                            \
        u8 b2 = (mb_)[1];                                                                                                                            \
                                                                                                                                                     \
        if (!b1) {                                                                                                                                   \
            UNSAFE_THROW_AND_QUIT(ret_, ENC_ERR_NO_MAP_RULE);                                                                                        \
        }                                                                                                                                            \
                                                                                                                                                     \
        if (!b2) {                                                                                                                                   \
            if (dstValid_) {                                                                                                                         \
                                                                                                                       \
                **(dstStream_) = b1;                                                                                                                 \
                ++*(dstStream_);                                                                                                                     \
            }                                                                                                                                        \
        } else {                                                                                                                                     \
            if (dstValid_) {                                                                                                                         \
                if ((dstLimit_) - *(dstCnt_) < 2) {                                                                                                  \
                    UNSAFE_THROW_AND_QUIT(ret_, ENC_ERR_NO_BUF_LEFT);                                                                                \
                }                                                                                                                                    \
                                                                                                                                                     \
                                                                                                                       \
                **(dstStream_) = b1;                                                                                                                 \
                ++*(dstStream_);                                                                                                                     \
                **(dstStream_) = b2;                                                                                                                 \
                ++*(dstStream_);                                                                                                                     \
                                                                                                                                                     \
                ++*(dstCnt_);                                                                                                                        \
            } else {                                                                                                                                 \
                ++*(dstCnt_);                                                                                                                        \
            }                                                                                                                                        \
        }                                                                                                                                            \
                                                                                                                                                     \
        ++*(dstCnt_);                                                                                                                                \
        ++*(srcStream_);                                                                                                                             \
        ++*(srcCnt_);                                                                                                                                \
    }




void ENCiRegisterVersion(void);
ENCResult ENCiCheckParameters(BOOL dstValid, s32* dstSizeIn, s32* dstSizeOut, BOOL* dstValidOut, BOOL srcValid, s32* srcSizeIn, s32* srcSizeOut,
                              BOOL* srcLimitedOut);
int ENCiCheckBreakType(unsigned a, unsigned b);
int ENCiWriteBreakType(void* stream, int width, ENCBreakType breakType, BOOL valid);

#ifdef __cplusplus
}
#endif

#endif
