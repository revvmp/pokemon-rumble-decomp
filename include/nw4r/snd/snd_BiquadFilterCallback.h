#ifndef NW4R_SND_BIQUAD_FILTER_CALLBACK_H
#define NW4R_SND_BIQUAD_FILTER_CALLBACK_H
#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace snd {

class BiquadFilterCallback {
public:
    struct BiquadCoef {
        u16 b0;
        u16 b1;
        u16 b2;
        u16 a1;
        u16 a2;
    };

public:
    virtual ~BiquadFilterCallback() {}

    virtual void GetCoef(int type, f32 value, BiquadCoef* pCoef) const = 0;
};

}
}

#endif
