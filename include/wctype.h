#ifndef MSL_WCTYPE_H
#define MSL_WCTYPE_H

#include <wchar_t.h>
#include "MSL_C/locale.h"

#ifdef __cplusplus
extern "C" {
#endif

#define wctype_alpha 0x0001
#define wctype_blank 0x0002
#define wctype_cntrl 0x0004
#define wctype_digit 0x0008
#define wctype_graph 0x0010
#define wctype_lower 0x0020
#define wctype_print 0x0040
#define wctype_punct 0x0080
#define wctype_space 0x0100
#define wctype_upper 0x0200
#define wctype_xdigit 0x0400

static inline int iswalpha(wint_t c) {
    return (c < 0 || c >= 256) ? 0 : (int)(_current_locale.ctype_cmpt_ptr->wctype_map_ptr[c] & wctype_alpha);
}

static inline int iswdigit(wint_t c) {
    return (c < 0 || c >= 256) ? 0 : (int)(_current_locale.ctype_cmpt_ptr->wctype_map_ptr[c] & wctype_digit);
}

static inline int iswspace(wint_t c) {
    return (c < 0 || c >= 256) ? 0 : (int)(_current_locale.ctype_cmpt_ptr->wctype_map_ptr[c] & wctype_space);
}

static inline wint_t towupper(wint_t c) {
    return (c < 0 || c >= 256) ? c : (int)_current_locale.ctype_cmpt_ptr->wupper_map_ptr[c];
}

#ifdef __cplusplus
}
#endif

#endif
