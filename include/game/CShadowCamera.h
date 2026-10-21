#ifndef GAME_CSHADOWCAMERA_H
#define GAME_CSHADOWCAMERA_H

#include <game/BWorldCamera.h>
#include <game/CaptureTexture.h>
#include <memory>

class CModel;

class CShadowCamera : public BWorldCamera {
public:
    enum { TYPE = TYPE_SHADOW_CAMERA };

    explicit CShadowCamera(u32 type);
    virtual ~CShadowCamera();
    virtual void UpdateVisible();

    bool IsAutoSelectLight() const { return mAutoSelectLight; }
    void Init(u16 width, u16 height, int levels, bool useTextureAlpha);
    void SetupShadowModel(std::tr1::shared_ptr<CModel> model);
    void SelectLight();

private:
    bool CalcLightProjection();

    int mTextureLevels;
    CaptureTexture mTextures[3];
    bool mUseTextureAlpha;
    u8 mAlpha;
    bool mAutoSelectLight;
    int mLightSetIndex;
    int mLightIndex;
    f32 mPlaneHeight;
};

#endif
