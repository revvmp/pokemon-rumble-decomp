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

    void Setup(u16 width, u16 height, int textureLevels, bool useTextureAlpha);
    bool IsAutoSelectLight() const { return mAutoSelectLight; }
    void SelectLight();
    void SetupShadowModel(std::tr1::shared_ptr<CModel> model);

    int GetTextureLevels() const { return mTextureLevels; }
    CaptureTexture& GetTexture(int index) { return mTextures[index]; }
    bool IsTextureAlpha() const { return mUseTextureAlpha; }
    u8 GetAlpha() const { return mAlpha; }

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
