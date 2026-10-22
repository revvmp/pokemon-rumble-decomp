#ifndef GAME_CMULTIRANKING_H
#define GAME_CMULTIRANKING_H

#include <nw4r/ut/ut_algorithm.h>
#include <game/IMultiRanking.h>
#include <game/BWorld.h>
#include <game/BWorldCamera.h>
#include <game/BUILayout.h>
#include <game/CUIManager.h>
#include <game/CUIPlayerID.h>
#include <game/CPii.h>
#include <game/IPauseMenu.h>
#include <nw4r/snd/snd_SoundHandle.h>

class CMultiRanking : public IMultiRanking {
    friend class IMultiRanking;
public:
    explicit CMultiRanking(const MapDesc* pDesc);
    virtual void Calc();
    virtual void CalcMtx();
    virtual void CalcView();
    virtual void Draw();
    virtual void RunAction();
    virtual BWorld* GetWorld() const { return mWorld.get(); }
    virtual const MapDesc& GetMapDesc() const { return *mpMapDesc; }
    virtual IPauseMenu* GetPauseMenu() { return mPauseMenu.get(); }
    bool Init(bool background);
    void SkipMotions();

private:
    const MapDesc* mpMapDesc;
    std::tr1::shared_ptr<BWorld> mWorld;
    std::tr1::shared_ptr<BWorldCamera> mCamera;
    std::tr1::shared_ptr<CUIManager> mUI;
    std::tr1::shared_ptr<BUILayout> mRootLayout;
    std::tr1::shared_ptr<BUILayout> mRankingLayout;
    std::tr1::shared_ptr<IPauseMenu> mPauseMenu;
    std::tr1::shared_ptr<BModelObj> mModels[4];
    std::tr1::shared_ptr<CPii> mPiis[4];
    std::tr1::shared_ptr<BUILayout> mPlayerLayouts[4];
    std::tr1::shared_ptr<CUIPlayerID> mPlayerIDs[4];
    int mScores[4];
    std::tr1::shared_ptr<CModel::CMotionProgram> mMotions[4];
    bool mIDVisible[4];
    bool mMotionFinished[4];
    nw4r::math::VEC3 mOffsets[4];
    int mMaximumScore;
    int mPlayerCount;
    int mFlashFrame;
    nw4r::snd::SoundHandle mSound;
    int mMotionCount;
    int mFinishedMotionCount;
    bool mReturnFromGame;
    bool mActive;
};

#endif



