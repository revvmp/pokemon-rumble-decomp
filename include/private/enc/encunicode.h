#ifndef PRIVATE_ENC_UNICODE_H
#define PRIVATE_ENC_UNICODE_H

#include <revolution/types.h>

#include <private/enc/encutility.h>
#include <revolution/enc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef ENCMBState ENCUTF7State;
#define ENC_UTF7_STATE_INITIAL ENC_STATE_INITIAL

#define ENC_UTF7_STATE_BASE64_MODE (1 << 3)

#define ENC_UTF7_STATE_IS_BASE64_MODE(mbState_) ((mbState_) & ENC_UTF7_STATE_BASE64_MODE)
#define ENC_UTF7_STATE_GET_PARTIAL_BITS(mbState_) ((mbState_) & 0xfffc0000)

#define ENC_UTF7_STATE_GET_PARTIAL_OFFSET(mbState_) (((mbState_) & 0x00000007) << 1)

#define ENC_UTF7_STATE_COLLECT(partialBits_, partialOffset_)                                                                                         \
    (ENC_UTF7_STATE_BASE64_MODE | ((partialBits_) & 0xfffc0000) | ((partialOffset_) >> 1 & 0x00000007l))

typedef ENCMBState ENCUTF16State;
enum { ENC_UTF16_STATE_BIG_ENDIAN, ENC_UTF16_STATE_LITTLE_ENDIAN };

DECLARE_PRIVATE_ENC_FUNCTION_FROM_UTF16(Ascii)
DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(Ascii)
DECLARE_PRIVATE_ENC_FUNCTION(Utf32, u32, Utf16, u16)
DECLARE_PRIVATE_ENC_FUNCTION(Utf16, u16, Utf32, u32)
DECLARE_PRIVATE_ENC_FUNCTION(Utf32, u32, Utf8, u8)
DECLARE_PRIVATE_ENC_FUNCTION(Utf8, u8, Utf32, u32)
DECLARE_PRIVATE_ENC_FUNCTION(Utf16, u16, Utf8, u8)
DECLARE_PRIVATE_ENC_FUNCTION(Utf8, u8, Utf16, u16)
DECLARE_PRIVATE_ENC_FUNCTION_WITH_MB_STATE(Utf7, u8, Utf16, u16)
DECLARE_PRIVATE_ENC_FUNCTION_WITH_MB_STATE(Utf16, u16, Utf16, u16)

#ifdef __cplusplus
}
#endif

#endif
