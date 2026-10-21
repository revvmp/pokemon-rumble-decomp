#include <revolution/fa/puh_msc_cmd.h>

s32 __uhf_msc_cmd_check_sense(u8* sense) {
    s32 code = (sense[0] << 16) | (sense[1] << 8) | sense[2];
    switch (code) {
        case 0x000000:
            return 0;
        case 0x011701:
            return 1;
        case 0x011800:
            return 2;
        case 0x020401:
            return 3;
        case 0x020402:
            return 4;
        case 0x020404:
            return 5;
        case 0x0204FF:
            return 6;
        case 0x020600:
            return 7;
        case 0x020800:
            return 8;
        case 0x020801:
            return 9;
        case 0x020880:
            return 10;
        case 0x023A00:
            return 11;
        case 0x025400:
            return 12;
        case 0x028000:
            return 13;
        case 0x02FFFF:
            return 14;
        case 0x030200:
            return 15;
        case 0x030300:
            return 16;
        case 0x031000:
            return 17;
        case 0x031100:
            return 18;
        case 0x031200:
            return 19;
        case 0x031300:
            return 20;
        case 0x031400:
            return 21;
        case 0x033001:
            return 22;
        case 0x033101:
            return 23;
        case 0x051A00:
            return 25;
        case 0x052000:
            return 26;
        case 0x052100:
            return 27;
        case 0x052400:
            return 28;
        case 0x052500:
            return 29;
        case 0x052600:
            return 30;
        case 0x052601:
            return 31;
        case 0x052602:
            return 32;
        case 0x053900:
            return 33;
        case 0x062800:
            return 34;
        case 0x062900:
            return 35;
        case 0x062F00:
            return 36;
        case 0x072700:
            return 37;
        case 0x0B4E00:
            return 38;
        default:
            if (((sense[0] << 8) | sense[1]) == 0x440u) {
                return 24;
            }
            return 39;
    }
}
