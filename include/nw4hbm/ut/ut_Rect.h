#ifndef NW4HBM_UT_RECT_H
#define NW4HBM_UT_RECT_H
#include "nw4hbm/math.h"

#include "nw4hbm/types_nw4hbm.h"

namespace nw4hbm {
namespace ut {

struct Rect {
    f32 left;
    f32 top;
    f32 right;
    f32 bottom;

    Rect() : left(0.0f), top(0.0f), right(0.0f), bottom(0.0f) {}
    Rect(f32 l, f32 t, f32 r, f32 b) : left(l), top(t), right(r), bottom(b) {}
    ~Rect() {}

    void SetWidth(f32 width) {
        right = left + width;
    }
    f32 GetWidth() const {
        return right - left;
    }

    void SetHeight(f32 height) {
        bottom = top + height;
    }
    f32 GetHeight() const {
        return bottom - top;
    }

    void Normalize() {
        f32 l = left;
        f32 t = top;
        f32 r = right;
        f32 b = bottom;

        left = math::FSelect(r - l, l, r);
        right = math::FSelect(r - l, r, l);
        top = math::FSelect(b - t, t, b);
        bottom = math::FSelect(b - t, b, t);
    }

    void MoveTo(f32 x, f32 y) {
        right = x + GetWidth();
        left = x;
        bottom = y + GetHeight();
        top = y;
    }
};

}
}

#endif
