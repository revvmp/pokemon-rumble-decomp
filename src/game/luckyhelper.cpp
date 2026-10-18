#include <game/luckyhelper.h>

const u8 gLuckyNumberTable[100] = {
    70, 37, 83, 20, 99, 22, 65, 31, 13, 32,
    64, 16, 10, 72, 19, 68, 77, 40, 90, 43,
    23, 24, 14, 56, 5,  7,  48, 8,  55, 52,
    91, 47, 66, 78, 85, 62, 86, 69, 15, 75,
    41, 73, 76, 49, 98, 94, 63, 12, 71, 35,
    59, 38, 42, 97, 58, 67, 53, 36, 18, 61,
    87, 79, 89, 0,  39, 44, 6,  11, 21, 92,
    29, 82, 54, 80, 57, 33, 84, 51, 81, 46,
    3,  28, 1,  30, 17, 50, 96, 34, 2,  45,
    95, 26, 88, 4,  27, 9,  74, 93, 60, 25,
};

static const u32 KEY_0 = 66339863;
static const u32 KEY_1 = 93740587;
static const u32 KEY_2 = 22531951;
static const u32 KEY_3 = 39631509;

static const int DIGIT_BASE = 10000;
static const u32 HALF_MODULUS = 100000000;

static inline void Shuffle(u16& rDigit0, u16& rDigit1, u16& rDigit2, u16& rDigit3, u32 key0, u32 key1) {
    u32 low = (static_cast<u64>(rDigit0 + rDigit1 * DIGIT_BASE) * key0 + key1) % HALF_MODULUS;
    u32 high = (static_cast<u64>(rDigit2 + rDigit3 * DIGIT_BASE) * key1 + key0) % HALF_MODULUS;
    rDigit2 = low / DIGIT_BASE;
    rDigit1 = low % DIGIT_BASE;
    rDigit0 = high / DIGIT_BASE;
    rDigit3 = high % DIGIT_BASE;
}

static inline int Substitute(int value, const u8* pTable) {
    return pTable[value / 100] * 100 + pTable[value % 100];
}

void ScrambleLuckyNumber(u16* pDigits) {
    Shuffle(pDigits[0], pDigits[1], pDigits[2], pDigits[3], KEY_0, KEY_1);
    Shuffle(pDigits[0], pDigits[1], pDigits[2], pDigits[3], KEY_1, KEY_2);
    Shuffle(pDigits[0], pDigits[1], pDigits[2], pDigits[3], KEY_2, KEY_3);
    Shuffle(pDigits[0], pDigits[1], pDigits[2], pDigits[3], KEY_3, KEY_0);
}

u16 CalcLuckyNumberCheck(const u16* pDigits, const u8* pTable) {
    u16 digit0 = pDigits[0];
    u16 digit1 = pDigits[1];
    u16 digit2 = pDigits[2];
    u16 digit3 = pDigits[3];
    Shuffle(digit0, digit1, digit2, digit3, KEY_0, KEY_1);
    Shuffle(digit0, digit1, digit2, digit3, KEY_1, KEY_2);
    Shuffle(digit0, digit1, digit2, digit3, KEY_2, KEY_3);
    Shuffle(digit0, digit1, digit2, digit3, KEY_3, KEY_0);

    u16 check = digit0;
    check = (Substitute(check, pTable) + digit1) % DIGIT_BASE;
    check = (Substitute(check, pTable) + digit2) % DIGIT_BASE;
    check = (Substitute(check, pTable) + digit3) % DIGIT_BASE;
    return Substitute(check, pTable);
}
