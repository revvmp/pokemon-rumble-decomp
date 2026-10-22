#include <stdlib.h>
#include <string.h>
#include "MSL_C/locale.h"

int mbtowc(wchar_t* pwc, const char* s, size_t n) {
    return _current_locale.ctype_cmpt_ptr->decode_mb(pwc, s, n);
}

int __mbtowc_noconv(wchar_t* pwc, const char* s, size_t n) {
    if (!s) {
        return 0;
    }

    if (!n) {
        return -1;
    }

    if (pwc) {
        *pwc = (unsigned char)*s;
    }

    if (!*s) {
        return 0;
    }

    return 1;
}

int __wctomb_noconv(char* s, wchar_t wchar) {
    if (!s) {
        return 0;
    }

    *s = (char)wchar;
    return 1;
}

size_t mbstowcs(wchar_t* pwcs, const char* s, size_t n) {
    int chars_written;
    int result;
    size_t source_len = strlen(s);

    if (pwcs) {
        for (chars_written = 0; chars_written < n; chars_written++) {
            if (*s) {
                result = _current_locale.ctype_cmpt_ptr->decode_mb(pwcs++, s, source_len);

                if (result > 0) {
                    s += result;
                    source_len -= result;
                } else {
                    return -1;
                }
            } else {
                *pwcs = 0;
                break;
            }
        }
    } else {
        chars_written = 0;
    }

    return chars_written;
}

size_t wcstombs(char* s, const wchar_t* pwcs, size_t n) {
    int result;
    int chars_written = 0;
    char temp[8];

    if (!s || !pwcs) {
        return 0;
    } else {
        const wchar_t* src = pwcs;

        while (chars_written <= n) {
            if (!*src) {
                s[chars_written] = 0;
                break;
            } else {
                result = _current_locale.ctype_cmpt_ptr->encode_wc(temp, *src++);

                if (chars_written + result > n) {
                    break;
                }

                strncpy(s + chars_written, temp, result);
                chars_written += result;
            }
        }
    }

    return chars_written;
}
