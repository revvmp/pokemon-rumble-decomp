#include <MSL_C/ctype.h>

int __msl_strnicmp(const char* s1, const char* s2, int count) {
    int i;
    char c1;
    char c2;

    for (i = 0; i < count; i++) {
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);

        if (c1 < c2) {
            return -1;
        }

        if (c1 > c2) {
            return 1;
        }

        if (c1 == 0) {
            return 0;
        }
    }

    return 0;
}
