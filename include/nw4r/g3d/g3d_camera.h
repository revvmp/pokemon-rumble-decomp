#ifndef NW4R_G3D_CAMERA_H
#define NW4R_G3D_CAMERA_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_rescommon.h>

#include <nw4r/math/math_types.h>

#include <revolution/MTX.h>

namespace nw4r {
namespace g3d {

struct CameraData {
    enum Flag {
        FLAG_CAM_LOOKAT = (1 << 0),
        FLAG_CAM_ROTATE = (1 << 1),
        FLAG_CAM_AIM = (1 << 2),
        FLAG_CAM_MTX_READY = (1 << 3),

        FLAG_PROJ_FRUSTUM = (1 << 4),
        FLAG_PROJ_PERSP = (1 << 5),
        FLAG_PROJ_ORTHO = (1 << 6),
        FLAG_PROJ_MTX_READY = (1 << 7),

        FLAG_VI_ODD_FIELD = (1 << 8),
    };

    math::MTX34 cameraMtx;
    math::MTX44 projMtx;
    u32 flags;
    math::VEC3 cameraPos;
    math::VEC3 cameraUp;
    math::VEC3 cameraTarget;
    math::VEC3 cameraRotate;
    f32 cameraTwist;
    GXProjectionType projType;
    f32 projFovy;
    f32 projAspect;
    f32 projNear;
    f32 projFar;
    f32 projTop;
    f32 projBottom;
    f32 projLeft;
    f32 projRight;
    f32 lightScaleS;
    f32 lightScaleT;
    f32 lightTransS;
    f32 lightTransT;
    math::VEC2 viewportOrigin;
    math::VEC2 viewportSize;
    f32 viewportNear;
    f32 viewportFar;
    u32 scissorX;
    u32 scissorY;
    u32 scissorWidth;
    u32 scissorHeight;
    s32 scissorOffsetX;
    s32 scissorOffsetY;
};

class Camera : public ResCommon<CameraData> {
public:
    enum PostureType {
        POSTURE_LOOKAT,
        POSTURE_ROTATE,
        POSTURE_AIM
    };

    struct PostureInfo {
        PostureType tp;
        math::VEC3 cameraUp;
        math::VEC3 cameraTarget;
        math::VEC3 cameraRotate;
        f32 cameraTwist;
    };

public:
    explicit Camera(CameraData *pData);

    void Init();
    void Init(u16 efbWidth, u16 efbHeight, u16 xfbWidth, u16 xfbHeight, u16 viWidth, u16 viHeight);

    void SetPosition(f32 x, f32 y, f32 z);
    void SetPosition(const math::VEC3 &rPos);
    void GetPosition(math::VEC3 *pPos) const;

    void SetPosture(const PostureInfo &rInfo);
    void GetPosture(PostureInfo *pInfo) const;
    void SetCameraMtxDirectly(const math::MTX34 &rMtx);
    void SetPerspective(f32 fovy, f32 aspect, f32 near, f32 far);
    void SetFrustum(f32 top, f32 bottom, f32 left, f32 right, f32 near, f32 far);
    void SetOrtho(f32 top, f32 bottom, f32 left, f32 right, f32 near, f32 far);
    void SetProjectionMtxDirectly(const math::MTX44 *pMtx);
    void SetTexMtxParam(f32 lightScaleS, f32 lightScaleT, f32 lightTransS, f32 lightTransT);

    void SetScissor(u32 x, u32 y, u32 width, u32 height);
    void SetScissorBoxOffset(s32 ox, s32 oy);

    void SetViewport(f32 x, f32 y, f32 width, f32 height);
    void SetViewportZRange(f32 near, f32 far);
    void GetViewport(f32 *pX, f32 *pY, f32 *pWidth, f32 *pHeight, f32 *pNear, f32 *pFar) const;

    void Project(math::VEC3 *pSrcPos, const math::VEC3 &worldPos) const;

    void GetCameraMtx(math::MTX34 *pMtx) const;
    void GetProjectionMtx(math::MTX44 *pMtx) const;
    void GetProjectionTexMtx(math::MTX34 *pMtx) const;
    void GetEnvironmentTexMtx(math::MTX34 *pMtx) const;

    void GXSetViewport() const;
    void GXSetProjection() const;
    void GXSetScissor() const;
    void GXSetScissorBoxOffset() const;

    GXProjectionType GetProjectionType() const {
        return ref().projType;
    }

private:
    void UpdateCameraMtx() const;
    void UpdateProjectionMtx() const;
};

}
}

#endif
