#ifndef GAME_CLAYOUT_H
#define GAME_CLAYOUT_H

#include <game/ICalcDraw.h>
#include <memory>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <nw4r/lyt/lyt_arcResourceAccessor.h>
#include <nw4r/lyt/lyt_drawInfo.h>
#include <revolution/gx/GXEnum.h>
#include <revolution/types.h>

namespace nw4r {
namespace lyt {
class AnimTransform;
class Layout;
class Pane;
}
namespace ut {
class Font;
}
}

namespace std {
namespace tr1 {
template <typename Signature> class function;
}
}

class CFileData;

class CLayout : public ICalcDraw {
public:
    class CInitializeException;
    class PaneMP;
    typedef std::map<std::string, std::tr1::shared_ptr<nw4r::ut::Font> > FontMap;

    class CMotionProgram {
    public:
        bool IsStarted() const { return mState != 0; }
        bool IsFinished() const { return mState == 3 || mState == 2; }

    private:
        f32 mFrame;
        f32 mRate;
        nw4r::lyt::AnimTransform* mpAnimTransform;
        nw4r::lyt::Pane* mpTarget;
        u16 mLastFrame;
        int mState;
    };

    CLayout();
    virtual ~CLayout();
    virtual void Calc();
    virtual void Draw();

    void Load(const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts, bool background);
    nw4r::lyt::Pane* FindPane(const char* pName);
    void SetVisible(bool visible);
    std::tr1::shared_ptr<CMotionProgram> CreateMotion(const char* pName);

private:
    u32 mFlags;
    std::string mName;
    std::list<std::tr1::shared_ptr<CFileData> > mFiles;
    std::vector<std::tr1::shared_ptr<nw4r::lyt::ArcResourceLink> > mArchives;
    nw4r::lyt::MultiArcResourceAccessor mAccessor;
    std::tr1::shared_ptr<nw4r::lyt::Layout> mLayout;
    std::tr1::shared_ptr<FontMap> mFonts;
    nw4r::lyt::DrawInfo mDrawInfo;
    std::map<std::string, nw4r::lyt::AnimTransform*> mAnimations;
    std::map<nw4r::lyt::Pane*, std::tr1::shared_ptr<PaneMP> > mPanePrograms;
    nw4r::ut::Rect mViewRect;
    nw4r::math::VEC2 mViewScale;
    std::tr1::shared_ptr<std::tr1::function<void(const CInitializeException&)> > mInitializeErrorCallback;
    GXCompare mDepthCompare;
};

#endif
