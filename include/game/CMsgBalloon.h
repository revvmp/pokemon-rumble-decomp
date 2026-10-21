#ifndef GAME_CMSGBALLOON_H
#define GAME_CMSGBALLOON_H

#include <game/CLayout.h>
#include <game/IDlg.h>
#include <list>
#include <memory>
#include <nw4r/math/math_types.h>
#include <revolution/types.h>
#include <string>
#include <vector>

class BUILayout;
class BWorldObj;
class COffScreenTexture;
class CUIManager;

class IMsgBalloon : public IDlg {
public:
    virtual ~IMsgBalloon() {}
    virtual void Show(bool show) = 0;
    virtual void Add(int id, f32 x, f32 y) = 0;
    virtual void Register(int id, int kind, f32 y) = 0;
    virtual void RegisterWarpzone(int id, int kind, f32 y) = 0;
    virtual bool IsShown(int index) = 0;
    virtual void AdjustY(int id, int y) = 0;

    static std::tr1::shared_ptr<IMsgBalloon> Create(bool flag);
};

class CMsgBalloon : public IMsgBalloon, public std::tr1::enable_shared_from_this<CMsgBalloon> {
public:
    struct Entry {
        std::tr1::weak_ptr<BWorldObj> mObject;
        std::wstring mText;
        nw4r::math::VEC3 mPosition;
    };

    CMsgBalloon();
    virtual ~CMsgBalloon();
    virtual void Calc();
    virtual void CalcMtx();
    virtual void CalcView();
    virtual void Draw();
    virtual bool Close();
    virtual void Show(bool show);
    virtual void Add(int id, f32 x, f32 y);
    virtual void Register(int id, int kind, f32 y);
    virtual void RegisterWarpzone(int id, int kind, f32 y);
    virtual bool IsShown(int index);
    virtual void AdjustY(int id, int y);

    void Init(bool flag);

private:
    bool mIsShown;
    int mShowWait;
    int mHideWait;
    std::tr1::shared_ptr<CUIManager> mUIManager;
    std::tr1::shared_ptr<BUILayout> mLayout;
    std::tr1::shared_ptr<BUILayout> mMsgLayout;
    std::tr1::shared_ptr<BUILayout> mWarpLayout;
    std::vector<std::tr1::shared_ptr<BUILayout> > mModelLayouts;
    std::vector<std::tr1::shared_ptr<COffScreenTexture> > mModelTextures;
    std::list<Entry> mEntries;
    Entry* mCurrentEntry;
    int mCurrentIndex;
    Entry* mShownEntries[4];
    nw4r::math::VEC3 mOffset;
    std::tr1::shared_ptr<CLayout::CMotionProgram> mMotion;
};

#endif
