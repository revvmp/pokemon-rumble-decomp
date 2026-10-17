#include <stdlib.h>

int abs(int n) {
    return n > 0 ? n : -n;
}

long labs(long n) {
    return n > 0 ? n : -n;
}

div_t div(int numerator, int denominator) {
    int numeratorSign = 1;
    int denominatorSign = 1;
    div_t value;

    if (numerator < 0) {
        numerator = -numerator;
        numeratorSign = -1;
    }

    if (denominator < 0) {
        denominator = -denominator;
        denominatorSign = -1;
    }

    value.quot = (numerator / denominator) * (numeratorSign * denominatorSign);
    value.rem = numerator * numeratorSign - value.quot * denominator * denominatorSign;
    return value;
}
