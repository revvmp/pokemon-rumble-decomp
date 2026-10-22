#ifndef GAME_CLAYOUT_H
#define GAME_CLAYOUT_H

#include <game/ICalcDraw.h>
#include <memory>
#include <list>
#include <MSL_CPP/runtime_error.h>
#include <map>
#include <set>
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
    enum Flags {
        FLAG_VISIBLE = 1,
        FLAG_LOADED = 2,
        FLAG_ANIMATED = 4,
        FLAG_DIRTY = 8
    };

    class CAttach {
    public:
        CAttach() : mpParent(nullptr), mpPane(nullptr) {}
        CLayout* mpParent;
        nw4r::lyt::Pane* mpPane;
        std::set<CLayout*> mChildren;
    };
    class PaneMP;
    typedef std::map<std::string, std::tr1::shared_ptr<nw4r::ut::Font> > FontMap;
    typedef std::map<nw4r::lyt::Pane*, std::tr1::shared_ptr<PaneMP> > PaneProgramMap;

    class CInitializeException : public std::runtime_error {
    public:
        CInitializeException(int error, const char* pArchive, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts);
        CInitializeException(int error, const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts);
        virtual ~CInitializeException() {}

    private:
        int mError;
        std::list<std::string> mArchives;
        std::string mLayoutName;
        std::tr1::shared_ptr<FontMap> mFonts;
    };

    class CMotionProgram {
        friend class CLayout;

    public:
        bool IsStarted() const { return mState != 0; }
        bool IsFinished() const { return mState == 3 || mState == 2; }
        bool IsEnded() const { return mState == 3; }
        bool IsEnded(bool includeFinished) const { return includeFinished ? (mState == 3 || mState == 2) : mState == 3; }
        void Finish() { mState = 3; }
        f32 GetFrame() const;
        void Wait();
        void WaitStart();

    private:
        f32 mFrame;
        f32 mRate;
        nw4r::lyt::AnimTransform* mpAnimTransform;
        PaneMP* mpTarget;
        u16 mLastFrame;
        int mState;
    };

    typedef std::list<std::tr1::shared_ptr<CMotionProgram> > MotionList;

    class PaneMP {
    public:
        nw4r::lyt::Pane* mpPane;
        f32 mFrame;
        MotionList mMotions;
        bool mRunning;
        std::set<nw4r::lyt::Pane*> mDescendants;
    };

    CLayout();
    virtual ~CLayout();
    virtual void Calc();
    virtual void Draw();

    void Load(const char* pArchive, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts, bool background);
    void Load(const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts, bool background);
    void Load(const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts) {
        Load(pArchives, pLayoutName, rFonts, false);
    }
    int LoadFromMemory(void* pArchive, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts) throw();
    nw4r::lyt::Pane* FindPane(const char* pName);
    void CalculateMtx(bool recursive);
    bool AttachLayout(const char* pPaneName, CLayout* pChild);
    bool Attach(nw4r::lyt::Pane* pPane, CLayout* pChild);
    static void HandleInitializeError(const CInitializeException& rError);
    void SetViewScale(const nw4r::math::VEC2& rScale);
    nw4r::math::_VEC2 NormalizeViewPosition(const nw4r::math::VEC2& rPosition) const;
    nw4r::math::_VEC2 GetViewScale() const;
    void Render(const nw4r::math::VEC2& rSize);
    void SetPaneVisible(const char* pName, bool visible);
    void SetVisible(bool visible);
    void UpdateAttachedLayouts();
    bool Detach();
    void DetachAttachedLayouts();
    const nw4r::ut::Rect& GetViewRect() const { return mViewRect; }
    std::tr1::shared_ptr<CMotionProgram> CreateMotion(const char* pName) { return CreateMotion(pName, spDefaultMotionPane, 0.0f, 1.0f); }
    std::tr1::shared_ptr<CMotionProgram> CreateMotion(const char* pName, nw4r::lyt::Pane* pPane, f32 startFrame, f32 rate);
    std::tr1::shared_ptr<CMotionProgram> CreateMotion(const char* pName, const char* pPaneName, f32 startFrame, f32 rate);
    std::tr1::shared_ptr<CMotionProgram> QueueMotion(const char* pName) { return QueueMotion(pName, spDefaultMotionPane, 0.0f, 1.0f); }
    std::tr1::shared_ptr<CMotionProgram> QueueMotion(const char* pName, nw4r::lyt::Pane* pPane, f32 startFrame, f32 rate);
    std::tr1::shared_ptr<CMotionProgram> QueueMotion(const char* pName, const char* pPaneName, f32 startFrame, f32 rate);
    u16 GetMotionLength(const char* pName);
    nw4r::lyt::TextBox* SetText(const char* pName, const wchar_t* pText);
    void StopMotion(nw4r::lyt::Pane* pPane = spDefaultMotionPane);
    bool IsVisible() const;

    std::tr1::shared_ptr<CMotionProgram> AppendMotion(const char* pName) { return AppendMotion(pName, spDefaultMotionPane, 0.0f, 1.0f); }
    std::tr1::shared_ptr<CMotionProgram> AppendMotion(const char* pName, nw4r::lyt::Pane* pTarget, f32 startFrame, f32 rate);
    std::tr1::shared_ptr<CMotionProgram> AppendMotion(const char* pName, const char* pPaneName, f32 startFrame, f32 rate);
    void SetVisible(nw4r::lyt::Pane* pPane, bool visible);
    bool SetTexture(const char* pName, const nw4r::lyt::TexMap& rTexture, int index);
    bool SetTexture(nw4r::lyt::Pane* pPane, const nw4r::lyt::TexMap& rTexture, int index);
    bool SetImage(const char* pPaneName, const char* pImageName, int index);
    nw4r::lyt::Layout* GetLayout() const { return mLayout.get(); }
    void SetDepthCompare(GXCompare compare) { mDepthCompare = compare; }

private:
    int LoadArchive(const char* pArchive, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts, bool background);
    int LoadArchives(const char* const* pArchives, const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts, bool background);
    int LoadLayout(const char* pLayoutName, const std::tr1::shared_ptr<FontMap>& rFonts) throw();
    void SetFlags(u32 flags) { mFlags |= flags; }
    bool HasFlags(u32 flags) const { return (mFlags & flags) != 0; }
    void UpdateViewRect();
    void ApplyMotionFrames();
    void RebindAnimations();
    void CalculateAttachedMtx();
    f32 GetPaneAlpha(nw4r::lyt::Pane* pPane);
    f32 GetLayoutAlpha();
    void AdvancePaneMotions(PaneMP* pPaneMP, f32 frames);
    std::tr1::shared_ptr<CMotionProgram> BuildMotion(const char* pName, nw4r::lyt::Pane* pPane, f32 startFrame, f32 rate);
    std::tr1::shared_ptr<CMotionProgram> ReplaceMotions(const std::tr1::shared_ptr<CMotionProgram>& rProgram);
    std::tr1::shared_ptr<CMotionProgram> ReplaceQueuedMotions(const std::tr1::shared_ptr<CMotionProgram>& rProgram);
    std::tr1::shared_ptr<CMotionProgram> AppendProgram(const std::tr1::shared_ptr<CMotionProgram>& rProgram);
    nw4r::lyt::AnimTransform* FindAnimation(const char* pName);
    PaneMP* FindPaneMP(nw4r::lyt::Pane* pPane);

    u32 mFlags;
    std::string mName;
    std::list<std::tr1::shared_ptr<CFileData> > mFiles;
    std::vector<std::tr1::shared_ptr<nw4r::lyt::ArcResourceLink> > mArchives;
    nw4r::lyt::MultiArcResourceAccessor mAccessor;
    std::tr1::shared_ptr<nw4r::lyt::Layout> mLayout;
    std::tr1::shared_ptr<FontMap> mFonts;
    nw4r::lyt::DrawInfo mDrawInfo;
    std::map<std::string, nw4r::lyt::AnimTransform*> mAnimations;
    PaneProgramMap mPanePrograms;
    nw4r::ut::Rect mViewRect;
    nw4r::math::VEC2 mPixelScale;
    std::tr1::shared_ptr<CAttach> mAttach;
    GXCompare mDepthCompare;
};

inline CLayout::Flags operator|(CLayout::Flags lhs, CLayout::Flags rhs) {
    return static_cast<CLayout::Flags>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

#endif
