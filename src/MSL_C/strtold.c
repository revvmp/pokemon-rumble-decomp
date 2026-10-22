#include <float.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <wctype.h>
#include <MSL_C/ansi_fp.h>
#include <MSL_C/ctype.h>
#include <MSL_C/errno.h>
#include <MSL_C/locale.h>
#include <MSL_C/math.h>
#include <MSL_C/stdio_api.h>
#include <MSL_C/strtold.h>

extern int __double_huge[];

static inline int IsSpace(int c) {
    return c < 0 || c > 255 ? 0 : _current_locale.ctype_cmpt_ptr->ctype_map_ptr[c] & 0x100;
}

static inline int IsNarrowDigit(int c) {
    return c < 0 || c > 255 ? 0 :
        _current_locale.ctype_cmpt_ptr->ctype_map_ptr[c] & wctype_digit;
}

static inline int IsNarrowAlpha(int c) {
    return c < 0 || c > 255 ? 0 :
        _current_locale.ctype_cmpt_ptr->ctype_map_ptr[c] & wctype_alpha;
}

static inline int IsNarrowHexDigit(int c) {
    return c < 0 || c > 255 ? 0 :
        _current_locale.ctype_cmpt_ptr->ctype_map_ptr[c] & wctype_xdigit;
}

enum ScanState {
    SCAN_START = 0x0001,
    SCAN_SIG_START = 0x0002,
    SCAN_LEADING_SIG_ZEROS = 0x0004,
    SCAN_INT_DIGIT_LOOP = 0x0008,
    SCAN_FRAC_DIGIT_START = 0x0010,
    SCAN_FRAC_DIGIT_LOOP = 0x0020,
    SCAN_EXP_START = 0x0040,
    SCAN_EXP_SIGN = 0x0080,
    SCAN_EXP_DIGIT_START = 0x0100,
    SCAN_LEADING_EXP_ZEROS = 0x0200,
    SCAN_EXP_DIGIT_LOOP = 0x0400,
    SCAN_FINISHED = 0x0800,
    SCAN_FAILURE = 0x1000,
    SCAN_NAN = 0x2000,
    SCAN_INFINITY = 0x4000,
    SCAN_HEX = 0x8000
};

enum HexState {
    HEX_START = 0x01,
    HEX_LEADING_ZEROS = 0x02,
    HEX_INT_DIGITS = 0x04,
    HEX_FRAC_DIGITS = 0x08,
    HEX_EXP_START = 0x10,
    HEX_EXP_SIGN = 0x20,
    HEX_EXP_DIGIT_START = 0x40,
    HEX_LEADING_EXP_ZEROS = 0x80,
    HEX_EXP_DIGITS = 0x100
};

#define SCAN_FINAL (SCAN_FINISHED | SCAN_FAILURE)
#define SCAN_SUCCESS (SCAN_LEADING_SIG_ZEROS | SCAN_INT_DIGIT_LOOP | SCAN_FRAC_DIGIT_LOOP | SCAN_LEADING_EXP_ZEROS | SCAN_EXP_DIGIT_LOOP | SCAN_FINISHED)
#define HEX_SUCCESS (HEX_LEADING_ZEROS | HEX_INT_DIGITS | HEX_FRAC_DIGITS | HEX_LEADING_EXP_ZEROS | HEX_EXP_DIGITS)

#define MAX_DIGITS 20

#define final_state(scan_state) ((scan_state) & SCAN_FINAL)
#define success(scan_state) ((scan_state) & SCAN_SUCCESS)
#define hex_success(count, scan_state) ((count) - 1 > 2 && ((scan_state) & HEX_SUCCESS))
#define fetch() (count++, (*read)(read_arg, 0, __GetAwChar))
#define unfetch(c) (*read)(read_arg, c, __UngetAwChar)

double __strtold(int max_width, int (*read)(void*, int, int),
                void* read_arg, int* chars_scanned, int* overflow) {
    int scan_state = SCAN_START;
    int hex_scan_state = 0;
    int count = 0;
    int spaces = 0;
    int c;
    decimal d = {0, 0, 0, {0, ""}};
    int sig_negative = 0;
    int exp_negative = 0;
    long exp_value = 0;
    int exp_adjust = 0;
    register double result = 0.0;
    int sign_detected = 0;
    int radix_marker = *(unsigned char*)(__lconv).decimal_point;

    unsigned char* chptr;
    unsigned char mantissa[8];
    unsigned mantissa_digits;
    unsigned long exponent = 0;

    int ui;
    unsigned char uch, uch1;
    int NibbleIndex;
    int expsign = 0;
    int exp_digits = 0;
    unsigned intdigits = 0;

    *overflow = 0;
    c = fetch();

    while (count <= max_width && c != -1 && !final_state(scan_state)) {
        switch (scan_state) {
            case SCAN_START:
                if (IsSpace(c)) {
                    c = fetch();
                    count--;
                    spaces++;
                    break;
                }

                switch(toupper(c)) {
                    case '-':
                        sig_negative = 1;

                    case '+':
                        c = fetch();
                        sign_detected = 1;
                        break;
                    case 'I':
                        c = fetch();
                        scan_state = SCAN_INFINITY;
                        break;

                    case 'N':
                        c = fetch();
                        scan_state = SCAN_NAN;
                        break;

                    default:
                        scan_state = SCAN_SIG_START;
                        break;
                }
                break;

            case SCAN_INFINITY: {
                int i = 1;
                char model[] = "INFINITY";

                while((i < 8) && (toupper(c) == model[i])) {
                    i++;
                    c = fetch();
                }

                if((i == 3) || (i == 8)) {
                    if (sig_negative) {
                        float infinity = -INFINITY;
                        result = infinity;
                    }
                    else {
                        result = INFINITY;
                    }

                    *chars_scanned = spaces + i + sign_detected;
                    return result;
                }
                else {
                    scan_state = SCAN_FAILURE;
                }

                break;
            }

            case SCAN_NAN: {
                int i = 1, j = 0;
                char model[] = "NAN(";
                char nan_arg[32] = "";
                while((i < 4) && (toupper(c) == model[i])) {
                    i++;
                    c = fetch();
                }

                if ((i == 3) || (i == 4)) {
                    if (i == 4) {
                        while((j < 32) && (IsNarrowDigit(c) || IsNarrowAlpha(c) || (c == radix_marker))) {
                            nan_arg[j++] = (char)c;
                            c = fetch();
                        }

                        if (c != ')') {
                            scan_state = SCAN_FAILURE;
                            break;
                        }
                        else {
                            j++;
                        }
                    }
                    nan_arg[j] = '\0';

                    if (sig_negative) {
                        result = -nan(nan_arg);
                    }
                    else {
                        result = nan(nan_arg);
                    }

                    *chars_scanned = spaces + i + j + sign_detected;
                    return result;
                }
                else {
                    scan_state = SCAN_FAILURE;
                }
                break;
            }

            case SCAN_SIG_START:
                if (c == radix_marker) {
                    scan_state = SCAN_FRAC_DIGIT_START;
                    c = fetch();
                    break;
                }
                if (!IsNarrowDigit(c)) {
                    scan_state = SCAN_FAILURE;
                    break;
                }

                if (c == '0') {
                    c = fetch();
                    if (toupper(c) == 'X') {
                        scan_state = SCAN_HEX;
                        hex_scan_state = HEX_START;
                    }
                    else {
                        scan_state = SCAN_LEADING_SIG_ZEROS;
                    }
                    break;
                }

                scan_state = SCAN_INT_DIGIT_LOOP;
                break;

            case SCAN_LEADING_SIG_ZEROS:
                if (c == '0') {
                    c = fetch();

                    break;
                }
                scan_state = SCAN_INT_DIGIT_LOOP;
                break;

            case SCAN_INT_DIGIT_LOOP:
                if (!IsNarrowDigit(c)) {
                    if (c == radix_marker) {
                        scan_state = SCAN_FRAC_DIGIT_LOOP;
                        c = fetch();
                    }
                    else {
                        scan_state = SCAN_EXP_START;
                    }
                    break;
                }
                if (d.sig.length < MAX_DIGITS) {
                    d.sig.text[d.sig.length++] = (unsigned char)c;
                }
                else {
                    exp_adjust++;
                }

                c = fetch();
                break;

            case SCAN_FRAC_DIGIT_START:
                if (!IsNarrowDigit(c)) {
                    scan_state = SCAN_FAILURE;
                    break;
                }

                scan_state = SCAN_FRAC_DIGIT_LOOP;
                break;

            case SCAN_FRAC_DIGIT_LOOP:
                if (!IsNarrowDigit(c)) {
                    scan_state = SCAN_EXP_START;
                    break;
                }

                if (d.sig.length < MAX_DIGITS) {
                    if ( c != '0' || d.sig.length) {
                        d.sig.text[d.sig.length++] = (unsigned char)c;
                    }

                    exp_adjust--;
                }
                c = fetch();
                break;

            case SCAN_EXP_START:
                if (toupper(c) == 'E') {
                    scan_state = SCAN_EXP_SIGN;
                    c = fetch();
                    break;
                }
                scan_state = SCAN_FINISHED;
                break;

            case SCAN_EXP_SIGN:
                if (c == '+') {
                    c = fetch();
                }
                else if (c == '-') {
                    c = fetch();
                    exp_negative = 1;
                }

                scan_state = SCAN_EXP_DIGIT_START;
                break;

            case SCAN_EXP_DIGIT_START:
                if (!IsNarrowDigit(c)) {
                    scan_state = SCAN_FAILURE;
                    break;
                }

                if (c == '0') {
                    scan_state = SCAN_LEADING_EXP_ZEROS;
                    c = fetch();
                    break;
                }

                scan_state = SCAN_EXP_DIGIT_LOOP;
                break;

            case SCAN_LEADING_EXP_ZEROS:
                if (c == '0') {
                    c = fetch();
                    break;
                }

                scan_state = SCAN_EXP_DIGIT_LOOP;
                break;

            case SCAN_EXP_DIGIT_LOOP:
                if (!IsNarrowDigit(c)) {
                    scan_state = SCAN_FINISHED;
                    break;
                }

                exp_value = exp_value*10 + (c - '0');
                if (exp_value > 308) {
                    *overflow = 1;
                }

                c = fetch();
                break;

            case SCAN_HEX: {
                switch(hex_scan_state) {
                    case HEX_START:
                        memset(mantissa, 0, sizeof(mantissa));
                        chptr           = mantissa;
                        mantissa_digits = (DBL_MANT_DIG + 3) / 4;
                        intdigits       = 0;
                        NibbleIndex     = 0;
                        hex_scan_state = HEX_LEADING_ZEROS;
                        c = fetch();
                        break;

                    case HEX_LEADING_ZEROS:
                        if (c == '0') {
                            c = fetch();
                            break;
                        }

                        hex_scan_state = HEX_INT_DIGITS;
                        break;

                    case HEX_INT_DIGITS:
                        if (!IsNarrowHexDigit(c)) {
                            if (c == radix_marker) {
                                hex_scan_state = HEX_FRAC_DIGITS;
                                c = fetch();
                            }

                            else {
                                hex_scan_state = HEX_EXP_START;
                            }
                            break;
                        }

                        if (intdigits < mantissa_digits) {
                            intdigits++;
                            uch = *(chptr + NibbleIndex / 2);

                            ui = toupper(c);
                            if (ui >= 'A') {
                                ui = ui - 'A' + 10;
                            }
                            else {
                                ui -= '0';
                            }

                            uch1 = (unsigned char)ui;

                            if ((NibbleIndex % 2) != 0) {
                                uch |= uch1;
                            }
                            else {
                                uch |= (unsigned char)(uch1 << 4);
                            }

                            *(chptr + NibbleIndex++ / 2) = uch;
                            c = fetch();
                        }

                        else {
                            c = fetch();
                        }

                        break;

                    case HEX_FRAC_DIGITS:
                        if (!IsNarrowHexDigit(c)) {
                            hex_scan_state = HEX_EXP_START;
                            break;
                        }

                        if (intdigits < mantissa_digits) {
                            uch = *(chptr + NibbleIndex / 2);
                            ui = toupper(c);

                            if (ui >= 'A') {
                                ui = ui - 'A' + 10;
                            }
                            else {
                                ui -= '0';
                            }

                            uch1 = (unsigned char)ui;

                            if ((NibbleIndex % 2) != 0) {
                                uch |= uch1;
                            }
                            else {
                                uch |= (unsigned char)(uch1 << 4);
                            }

                            *(chptr + NibbleIndex++ / 2) = uch;
                            c = fetch();
                        }
                        else {
                            c = fetch();
                        }
                        break;

                    case HEX_EXP_START:
                        if (toupper(c) == 'P') {
                            hex_scan_state = HEX_EXP_SIGN;
                            exp_digits++;
                            c = fetch();
                        }
                        else {
                            scan_state = SCAN_FINISHED;
                        }

                        break;

                    case HEX_EXP_SIGN:
                        exp_digits++;
                        if (c == '-') {
                            expsign = 1;
                        }
                        else if (c != '+') {
                            c = unfetch(c);
                            count--;
                            exp_digits--;
                        }

                        hex_scan_state = HEX_EXP_DIGIT_START;
                        c = fetch();
                        break;

                    case HEX_EXP_DIGIT_START:
                        if (!IsNarrowDigit(c)) {
                            scan_state = SCAN_FAILURE;
                            break;
                        }

                        if (c == '0') {
                            exp_digits++;
                            hex_scan_state = HEX_LEADING_EXP_ZEROS;
                            c = fetch();
                            break;
                        }

                        hex_scan_state = HEX_EXP_DIGITS;
                        break;

                    case HEX_LEADING_EXP_ZEROS:
                        if (c == '0') {
                            c = fetch();
                            break;
                        }

                        hex_scan_state = HEX_EXP_DIGITS;
                        break;

                    case HEX_EXP_DIGITS:
                        if (!IsNarrowDigit(c)) {
                            scan_state = SCAN_FINISHED;
                            break;
                        }

                        exponent = exponent * 10 + (c - '0');

                        if (exp_value > SHRT_MAX) {
                            *overflow = 1;
                        }

                        exp_digits++;
                        c = fetch();

                        break;

                    }
            }
            break;
        }
    }

    if (scan_state != SCAN_HEX ? !success(scan_state) : !hex_success(count, hex_scan_state)) {
        count = 0;
        *chars_scanned = 0;
    }
    else {
        count--;
        *chars_scanned = count + spaces;
    }

    unfetch(c);

    if (hex_scan_state == 0) {
        if (exp_negative) {
            exp_value = -exp_value;
        }

        {
            int n = d.sig.length;
            unsigned char* p = &d.sig.text[n];

            while (n-- && *--p == '0') {
                exp_adjust++;
            }

            d.sig.length = (unsigned char)(n + 1);

            if (d.sig.length == 0) {
                d.sig.text[d.sig.length++] = '0';
            }
        }

        if (exp_value < -308 || exp_value > 308) {
            if (!*overflow && d.sig.text[0] == '0' && d.sig.text[1] == 0) {
                return 0.0;
            }

            *overflow = 1;
        }

        exp_value += exp_adjust;

        if (*overflow) {
            if (exp_negative) {
                return 0.0;
            }
            else {
                return sig_negative ? -(*(double*)__double_huge) : (*(double*)__double_huge);
            }
        }

        d.exp = (short) exp_value;

        result = __dec2num(&d);

        if (result != 0.0 && result < DBL_MIN) {
            *overflow = 1;
        }
        else if (result > DBL_MAX) {
            *overflow = 1;
            result    = (*(double*)__double_huge);
        }

        if (sig_negative && success(scan_state)) {
            result = -result;
        }

        return result;
    }
    else {
        unsigned mantissa_bit, dbl_bit;
        unsigned one_bit;
        double dbl_bits_storage;
        unsigned char* dbl_bits = (unsigned char*)&dbl_bits_storage;

        if (expsign) {
            exponent = -exponent;
        }

        exponent += intdigits*4;

        one_bit = 0;
        while (one_bit < 4 && !(mantissa[0] & (0x80 >> one_bit))) {
            one_bit++;
            exponent--;
        }

        exponent--;

        one_bit++;

        if (one_bit) {
            unsigned char carry = 0;
            for (chptr = mantissa+sizeof(mantissa)-1; chptr >= mantissa; chptr--) {
                unsigned char a = *chptr;
                *chptr = (unsigned char)((a << one_bit) | carry);
                carry = (unsigned char)(a >> (8 - one_bit));
            }
        }

        memset(dbl_bits, 0, sizeof(dbl_bits_storage));
        dbl_bit = (64 - 52);

        for (mantissa_bit = 0; mantissa_bit < 52; mantissa_bit += 8) {
            unsigned char ui = mantissa[mantissa_bit >> 3];
            int halfbits;

            if (mantissa_bit + 8 > 52) {
                ui &= 0xff << (52 - mantissa_bit);
            }

            halfbits = (dbl_bit & 7);
            dbl_bits[dbl_bit>>3] |= (unsigned char)(ui >> halfbits);
            dbl_bit += 8;
            dbl_bits[dbl_bit>>3] |= (unsigned char)(ui << (8 - halfbits));
        }

        exponent += exp_value + 1023;

        if ((exponent & ~(1024 * 2 - 1))) {
            *overflow = 1;
            return 0.0;
        }

        exponent <<= 32 - 11;

        dbl_bits[0] |= exponent >> 25;

        dbl_bits[1] |= exponent >> 17;

        if (sig_negative) {
            dbl_bits[0] |= 0x80;
        }

        result = *(double *)dbl_bits;

        return result;
    }
}


float strtof(const char* str, char** end) {
    float value;
    int count;
    int overflow;
    __InStrCtrl control;

    control.next_char = (char*)str;
    control.null_char_detected = 0;
    value = __strtold(INT_MAX, __StringRead, &control, &count, &overflow);

    if (end != NULL) {
        *end = (char*)str + count;
    }

    if (overflow) {
        errno = ERANGE;
    }

    return (float)value;
}
