#ifndef GAME_CMSGBALLOON_H
#define GAME_CMSGBALLOON_H

#include <game/CLayout.h>
#include <game/IDlg.h>
#include <game/StageID.h>
#include <list>
#include <memory>
#include <nw4r/math/math_types.h>
#include <revolution/types.h>
#include <string>
#include <vector>

class BUILayout;
class BWorldCamera;
class BWorldObj;
class CPiiTex;
class CUIManager;

class IMsgBalloon : public IDlg {
public:
    virtual ~IMsgBalloon() {}
    virtual void Show(bool show) = 0;
    virtual void Add(const wchar_t* pText, f32 z, f32 x) = 0;
    virtual void Register(BWorldObj* pObject, const wchar_t* pText, f32 radius) = 0;
    virtual void RegisterWarpzone(BWorldObj* pObject, f32 radius, const StageID& rStageID) = 0;
    virtual bool IsShown(int index) = 0;
    virtual void AdjustY(BWorldObj* pObject, const nw4r::math::VEC3& rPosition) = 0;

    static std::tr1::shared_ptr<IMsgBalloon> Create(bool flag);
};

class CMsgBalloon : public IMsgBalloon, public std::tr1::enable_shared_from_this<CMsgBalloon> {
public:
    struct Entry {
        Entry() : mPosition(0.0f, 0.0f, 0.0f) {}

        std::tr1::weak_ptr<BWorldObj> mObject;
        std::wstring mText;
        nw4r::math::VEC3 mPosition;
        f32 mDistanceScale;
        u32 mFlags;
        union {
            u32 mStageValue;
            StageID mStageID;
        };

        void ClearFlags(u32 flags) { mFlags &= ~flags; }
    };

    CMsgBalloon();
    virtual ~CMsgBalloon();
    virtual void Calc();
    virtual void CalcMtx();
    virtual void CalcView();
    virtual void Draw();
    virtual bool Close();
    virtual void Show(bool show);
    virtual void Add(const wchar_t* pText, f32 z, f32 x);
    virtual void Register(BWorldObj* pObject, const wchar_t* pText, f32 radius);
    virtual void RegisterWarpzone(BWorldObj* pObject, f32 radius, const StageID& rStageID);
    virtual bool IsShown(int index);
    virtual void AdjustY(BWorldObj* pObject, const nw4r::math::VEC3& rPosition);

    void Init(bool flag);

private:
    void UpdateBalloon(BWorldCamera* pCamera, CLayout* pLayout, bool* pBehind);
    void SetupBattleWarp(StageID stageID);
    void SetupStageWarp(StageID stageID);
    void SetupStageModels(StageID stageID);

    bool mIsShown;
    int mShowWait;
    int mHideWait;
    std::tr1::shared_ptr<CUIManager> mUIManager;
    std::tr1::shared_ptr<BUILayout> mLayout;
    std::tr1::shared_ptr<BUILayout> mMsgLayout;
    std::tr1::shared_ptr<BUILayout> mWarpLayout;
    std::vector<std::tr1::shared_ptr<BUILayout> > mModelLayouts;
    std::vector<std::tr1::shared_ptr<CPiiTex> > mModelTextures;
    std::list<Entry> mEntries;
    Entry* mCurrentEntry;
    int mCurrentIndex;
    Entry* mShownEntries[4];
    nw4r::math::VEC3 mOffset;
    std::tr1::shared_ptr<CLayout::CMotionProgram> mMotion;
};

#endif
