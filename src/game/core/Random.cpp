#include <game/core/Random.h>

#include <revolution/os.h>
#include <stdlib.h>

void InitRandom() {
    srand(OSGetTime());
}

void ShutdownRandom() {}

int RandInt(int min, int max) {
    u32 range = max - min + 1;
    if (range == 0) {
        return Rand32();
    }

    u32 value;
    if (range <= 0x7FFF) {
        value = rand();
    } else if (range <= 0x3FFFFFFF) {
        value = (rand() << 15) + rand();
    } else {
        value = Rand32();
    }
    return min + value % range;
}

u32 Rand32() {
    return (rand() << 30) + (rand() << 15) + rand();
}

namespace {

union FloatBits {
    u32 bits;
    f32 value;
};

inline f32 RandUnit() {
    FloatBits bits;
    bits.bits = (((rand() << 15) + rand()) & 0x7FFFFF) | 0x3F800000;
    return bits.value - 1.0f;
}

}

f32 RandReal(f32 min, f32 max) {
    return min + (max - min) * RandUnit();
}

bool RandBool(f32 probability) {
    return RandUnit() < probability;
}
