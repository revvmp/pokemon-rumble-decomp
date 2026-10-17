#ifndef NW4R_LYT_DRAW_INFO_H
#define NW4R_LYT_DRAW_INFO_H
#include <nw4r/types_nw4r.h>

#include <nw4r/math/math_types.h>
#include <nw4r/ut/ut_Rect.h>

namespace nw4r {
namespace lyt {

class DrawInfo {
public:
    DrawInfo();
    virtual ~DrawInfo();

    bool IsYAxisUp() const {
        return mViewRect.bottom - mViewRect.top < 0.0f;
    }

    const math::MTX34& GetViewMtx() const {
        return mViewMtx;
    }
    void SetViewMtx(const math::MTX34& rViewMtx) {
        mViewMtx = rViewMtx;
    }

    const ut::Rect& GetViewRect() const {
        return mViewRect;
    }
    void SetViewRect(const ut::Rect& rViewRect) {
        mViewRect = rViewRect;
    }

    const math::VEC2& GetLocationAdjustScale() const {
        return mLocationAdjustScale;
    }
    void SetLocationAdjustScale(const math::VEC2& rScale) {
        mLocationAdjustScale = rScale;
    }

    f32 GetGlobalAlpha() const {
        return mGlobalAlpha;
    }
    void SetGlobalAlpha(f32 alpha) {
        mGlobalAlpha = alpha;
    }

    bool IsMultipleViewMtxOnDraw() const {
        return mFlag.mulViewDraw;
    }

    bool IsInfluencedAlpha() const {
        return mFlag.influencedAlpha;
    }
    void SetInfluencedAlpha(bool influenced) {
        mFlag.influencedAlpha = influenced;
    }

    bool IsLocationAdjust() const {
        return mFlag.locationAdjust;
    }
    void SetLocationAdjust(bool adjust) {
        mFlag.locationAdjust = adjust;
    }

    bool IsInvisiblePaneCalculateMtx() const {
        return mFlag.invisiblePaneCalculateMtx;
    }
    void SetInvisiblePaneCalculateMtx(bool calc) {
        mFlag.invisiblePaneCalculateMtx = calc;
    }

    bool IsDebugDrawMode() const {
        return mFlag.debugDrawMode;
    }
    void SetDebugDrawMode(bool debug) {
        mFlag.debugDrawMode = debug;
    }

protected:
    math::MTX34 mViewMtx;
    ut::Rect mViewRect;
    math::VEC2 mLocationAdjustScale;
    f32 mGlobalAlpha;

    struct {
        u8 mulViewDraw : 1;
        u8 influencedAlpha : 1;
        u8 locationAdjust : 1;
        u8 invisiblePaneCalculateMtx : 1;
        u8 debugDrawMode : 1;
    } mFlag;
};

}
}

#endif
