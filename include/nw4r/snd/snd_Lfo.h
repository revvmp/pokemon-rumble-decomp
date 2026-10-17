#ifndef NW4R_SND_LFO_H
#define NW4R_SND_LFO_H
#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace snd {
namespace detail {

struct LfoParam {
    LfoParam() {
        Init();
    }

    void Init();

    f32 depth;
    f32 speed;
    u32 delay;
    u8 range;
    u8 PADDING_0xD[0x10 - 0xD];
};

class Lfo {
public:
    Lfo() : mDelayCounter(0), mCounter(0.0f) {}

    LfoParam& GetParam() {
        return mParam;
    }
    void SetParam(const LfoParam& rParam) {
        mParam = rParam;
    }

    void Reset();
    void Update(int msec);

    f32 GetValue() const;

private:
    static const int TABLE_SIZE = 32;

private:
    static s8 GetSinIdx(int idx);

private:
    LfoParam mParam;
    u32 mDelayCounter;
    f32 mCounter;
};

}
}
}

#endif
