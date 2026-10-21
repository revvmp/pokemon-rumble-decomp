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
class TexMap;
class TextBox;
}
namespace ut {
class Font;
}
}

class CFileData;

namespace {
nw4r::lyt::Pane* spDefaultMotionPane;
}

class CLayout : public ICalcDraw {
public:
    class CAttach;
    class CInitializeException;
    class PaneMP;
    typedef std::map<std::string, std::tr1::shared_ptr<nw4r::ut::Font> > FontMap;

    class CMotionProgram {
    public:
        bool IsStarted() const { return mState != 0; }
        bool IsFinished() const { return mState == 3 || mState == 2; }
        bool IsEnded() const { return mState == 3; }
        bool IsEnded(bool includeFinished) const { return includeFinished ? IsFinished() : IsEnded(); }
        void Wait();

    private:
        f32 mFrame;
        f32 mRate;
        nw4r::lyt::AnimTransform* mpAnimTransform;
        PaneMP* mpTarget;
        u16 mLastFrame;
        int mState;
    };

    CLayout();
    virtual ~CLayout();
    virtual void Calc();
    virtual void Draw();

    void Load(const char* pArchive, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts, bool background);
    void Load(const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts, bool background);
    nw4r::lyt::Pane* FindPane(const char* pName);
    void SetPaneVisible(const char* pName, bool visible);
    void SetVisible(bool visible);
    void UpdateAttachedLayouts();
    void DetachChildren();
    const nw4r::ut::Rect& GetViewRect() const { return mViewRect; }
    std::tr1::shared_ptr<CMotionProgram> CreateMotion(const char* pName) { return CreateMotion(pName, spDefaultMotionPane, 0.0f, 1.0f); }
    std::tr1::shared_ptr<CMotionProgram> CreateMotion(const char* pName, nw4r::lyt::Pane* pPane, f32 startFrame, f32 rate);
    std::tr1::shared_ptr<CMotionProgram> CreateMotion(const char* pName, const char* pPaneName, f32 startFrame, f32 rate);
    std::tr1::shared_ptr<CMotionProgram> QueueMotion(const char* pName) { return QueueMotion(pName, spDefaultMotionPane, 0.0f, 1.0f); }
    std::tr1::shared_ptr<CMotionProgram> QueueMotion(const char* pName, nw4r::lyt::Pane* pPane, f32 startFrame, f32 rate);
    std::tr1::shared_ptr<CMotionProgram> QueueMotion(const char* pName, const char* pPaneName, f32 startFrame, f32 rate);
    u16 GetMotionLength(const char* pName);
    nw4r::lyt::TextBox* SetText(const char* pName, const wchar_t* pText);

    std::tr1::shared_ptr<CMotionProgram> AppendMotion(const char* pName, nw4r::lyt::Pane* pTarget, f32 startFrame, f32 rate);
    void SetVisible(nw4r::lyt::Pane* pPane, bool visible);
    bool SetTexture(const char* pName, const nw4r::lyt::TexMap& rTexture, int index);

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
    std::tr1::shared_ptr<CAttach> mAttach;
    GXCompare mDepthCompare;
};

#endif
