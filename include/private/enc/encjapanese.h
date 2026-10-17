#ifndef PRIVATE_ENC_JAPANESE_H
#define PRIVATE_ENC_JAPANESE_H

#include <revolution/types.h>

#include <private/enc/encutility.h>
#include <revolution/enc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef ENCMBState ENCJISState;
enum {
    ENC_JIS_STATE_INVALID,

    ENC_JIS_STATE_US_ASCII,
    ENC_JIS_STATE_JIS_C6220_1969_RO,
    ENC_JIS_STATE_JIS_C6220_1969_JP,

    ENC_JIS_STATE_JIS_X_0208_1978,
    ENC_JIS_STATE_JIS_X_0208_1983,
    ENC_JIS_STATE_JIS_X_0212_1990,

    ENC_JIS_STATE_7,
};

extern const u16 enc_tbl_jp_mbtowc[];
extern const u8 enc_tbl_jp_wctomb[];
extern const u16 enc_offset_jp[];

extern BOOL enc_tbl_jp_loaded;

DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(Sjis)
DECLARE_PRIVATE_ENC_FUNCTION_FROM_UTF16(Sjis)
DECLARE_PRIVATE_ENC_FUNCTION_WITH_MB_STATE(Jis, u8, Sjis, u8)
DECLARE_PRIVATE_ENC_FUNCTION_WITH_MB_STATE(Sjis, u8, Jis, u8)
DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16_WITH_MB_STATE(Jis)
DECLARE_PRIVATE_ENC_FUNCTION_FROM_UTF16_WITH_MB_STATE(Jis)

#ifdef __cplusplus
}
#endif

#endif
