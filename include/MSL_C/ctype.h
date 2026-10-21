#ifndef MSL_C_CTYPE_H
#define MSL_C_CTYPE_H

#include "MSL_C/locale.h"

#ifdef __cplusplus
extern "C" {
#endif

inline int isdigit(int c) {
    return ((c < 0) || (c >= 0x100)) ? 0 : (int)(_current_locale.ctype_cmpt_ptr->ctype_map_ptr[c] & 0x8);
}

inline int tolower(int c) {
    return ((c < 0) || (c >= 0x100)) ? c : (int)(_current_locale.ctype_cmpt_ptr->lower_map_ptr[c]);
}

inline int toupper(int c) {
    return ((c < 0) || (c >= 0x100)) ? c : (int)(_current_locale.ctype_cmpt_ptr->upper_map_ptr[c]);
}

#ifdef __cplusplus
}
#endif

#endif
