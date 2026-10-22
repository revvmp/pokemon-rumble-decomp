#ifndef GAME_CPOSTEFFECT_H
#define GAME_CPOSTEFFECT_H

#include <game/CaptureTexture.h>
#include <game/IPostEffect.h>
#include <list>
#include <memory>

class CFileData;
class CModel;

namespace nw4r {
namespace g3d {
class ScnRoot;
}
}

class CPostEffect : public IPostEffect {
public:
    enum { EFFECT_NONE = 8 };

    struct EffectInfo {
        int mId;
        const char* mModelName;
        const char* mAnimationName;
        f32 mWidth;
        f32 mHeight;
        int mReserved;
        int mScaleMode;
    };

    CPostEffect(bool flag, std::tr1::shared_ptr<CFileData> file);
    virtual ~CPostEffect();
    virtual void Calc();
    virtual void Draw();
    virtual void Show(int effect);
    virtual void Hide();

private:
    void BindCaptureTexture(std::tr1::shared_ptr<CModel> model);

    std::tr1::shared_ptr<CFileData> mFile;
    int mFlag;
    std::list<std::tr1::shared_ptr<CModel> > mModels;
    std::tr1::shared_ptr<nw4r::g3d::ScnRoot> mScnRoot;
    CaptureTexture mTexture;
};

extern const CPostEffect::EffectInfo gPostEffectInfos[8];

#endif
