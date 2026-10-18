#ifndef GAME_CSHADOWCAMERA_H
#define GAME_CSHADOWCAMERA_H

#include <game/BWorldCamera.h>
#include <game/CaptureTexture.h>

class CShadowCamera : public BWorldCamera {
public:
    enum { TYPE = TYPE_SHADOW_CAMERA };

    explicit CShadowCamera(u32 type);
    virtual ~CShadowCamera();
    virtual void UpdateVisible();

    bool IsAutoSelectLight() const { return mAutoSelectLight; }
    void SelectLight();

private:
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
