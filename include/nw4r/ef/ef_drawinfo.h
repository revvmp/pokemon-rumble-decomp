#ifndef NW4R_EF_DRAW_INFO_H
#define NW4R_EF_DRAW_INFO_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/ef_util.h>
#include <nw4r/math/math_types.h>

#include <revolution/gx/GXEnum.h>
#include <revolution/gx/GXStruct.h>
#include <revolution/gx/GXTransform.h>

namespace nw4r {
namespace ef {

class DrawInfo {
private:
    math::MTX34 mViewMtx;
    math::MTX34 mProjMtx;
    bool mLightEnable;
    GXLightID mLightMask;
    GXLightID mAlphaLightMask;
    bool mIsSpotLight;
    GXFogType mFogType;
    f32 mFogStartz;
    f32 mFogEndz;
    f32 mFogNearz;
    f32 mFogFarz;
    GXColor mFogColor;
    f32 mZOffset;
    math::VEC3 mZOffsetOrig;
    GXColor mChanMatColor;
    GXColor mChanAmbColor;

public:
    DrawInfo() {
        math::MTX34Identity(&mViewMtx);
        math::MTX34Identity(&mProjMtx);

        mLightEnable = false;
        mLightMask = GX_LIGHT_NULL;
        mAlphaLightMask = GX_LIGHT_NULL;
        mIsSpotLight = true;
        mFogType = GX_FOG_NONE;
        mFogStartz = 0.0f;
        mFogEndz = 1.0f;
        mFogNearz = 0.0f;
        mFogFarz = 1.0f;
        mZOffset = 0.0f;

        mZOffsetOrig.x = 0.0f;
        mZOffsetOrig.y = 0.0f;
        mZOffsetOrig.z = 0.0f;

        mChanMatColor.r = mChanMatColor.g = mChanMatColor.b = 255;
        mChanMatColor.a = 255;

        mChanAmbColor.r = mChanAmbColor.g = mChanAmbColor.b = 0;
        mChanAmbColor.a = 255;
    }

    const math::MTX34* GetViewMtx() const {
        return &mViewMtx;
    }
    void SetViewMtx(const math::MTX34& rMtx) {
        mViewMtx = rMtx;
    }

    void GetViewMtxCooked(math::MTX34* pMtx) const {
        *pMtx = mViewMtx;

        if (mZOffset != 0.0f) {
            f32 proj[GX_PROJECTION_SZ];
            GXGetProjectionv(proj);

            switch (static_cast<int>(proj[0])) {
            case GX_PERSPECTIVE: {
                math::VEC3 dir;
                math::VEC3Transform(&dir, &mViewMtx, &mZOffsetOrig);

                if (Normalize(&dir, &dir)) {
                    if (dir.z >= 0.0f) {
                        pMtx->_03 += dir.x * mZOffset;
                        pMtx->_13 += dir.y * mZOffset;
                        pMtx->_23 += dir.z * mZOffset;
                    } else {
                        pMtx->_03 -= dir.x * mZOffset;
                        pMtx->_13 -= dir.y * mZOffset;
                        pMtx->_23 -= dir.z * mZOffset;
                    }
                } else {
                    pMtx->_23 += mZOffset;
                }
                break;
            }

            case GX_ORTHOGRAPHIC: {
                pMtx->_23 += mZOffset;
                break;
            }
            }
        }
    }

    const math::MTX34* GetProjMtx() const {
        return &mProjMtx;
    }
    void SetProjMtx(const math::MTX34& rMtx) {
        mProjMtx = rMtx;
    }

    bool IsLightEnable() const {
        return mLightEnable;
    }

    GXLightID GetLightMask() const {
        return mLightMask;
    }

    GXLightID GetAlphaLightMask() const {
        return mAlphaLightMask;
    }

    bool IsSpotLight() const {
        return mIsSpotLight;
    }

    void GetFog(GXFogType* pType, f32* pStartZ, f32* pEndZ, f32* pNearZ,
                f32* pFarZ, GXColor* pColor) const {
        *pType = mFogType;
        *pStartZ = mFogStartz;
        *pEndZ = mFogEndz;
        *pNearZ = mFogNearz;
        *pFarZ = mFogFarz;
        *pColor = mFogColor;
    }

    void GetZOffset(f32& rOffset, math::VEC3& rPos) const {
        rOffset = mZOffset;
        rPos = mZOffsetOrig;
    }
    void SetZOffset(f32 offset, const math::VEC3& rPos) {
        mZOffset = offset;
        mZOffsetOrig = rPos;
    }

    const GXColor& GetChanMatColor() const {
        return mChanMatColor;
    }
    const GXColor& GetChanAmbColor() const {
        return mChanAmbColor;
    }
};

}
}

#endif
