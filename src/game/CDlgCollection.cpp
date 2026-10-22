#include <game/IDlgCollection.h>

#include <algorithm>
#include <game/CFiber.h>
#include <game/CLayout.h>
#include <game/CPiiCollectionBox.h>
#include <game/CPiiPersonalData.h>
#include <game/CPiiTex.h>
#include <game/CPlayer.h>
#include <game/CUIList.h>
#include <game/CUIManager.h>
#include <game/IController.h>
#include <game/IDlgMsgWin.h>
#include <game/IGameMap.h>
#include <game/IGameSession.h>
#include <game/LayoutResources.h>
#include <game/Message.h>
#include <game/PiiSpeciesData.h>
#include <game/SaveData.h>
#include <game/Sound.h>
#include <game/StringFormat.h>
#include <memory>
#include <set>
#include <vector>






namespace {

const u16 sSecretSpecies = 0x97;

class CDlgCollectionItemModel {
public:
    struct Model {
        Model() {}

        CLayout mLayout;
        bool mReady;
        u16 mSpecies;
        std::tr1::shared_ptr<CPiiTex> mTex;
    };

    CDlgCollectionItemModel() : mInitialized(false), mAnimFrame(0) {}

    void Load(bool background) {
            std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
            const char* const* pArchives = gSharedLayoutArchives;
            CLayout layout;
            layout.Load(pArchives, "collection_list_item", fonts, background);
            unsigned count = 0;
            while (layout.FindPane(FormatString("MODEL%d", count).c_str())) {
                count++;
            }
            mModels.resize(count);
            for (unsigned i = 0; i < count; i++) {
                std::tr1::shared_ptr<Model> model(new Model);
                mModels[i] = model;
                model->mLayout.Load(pArchives, "collection_list_item_model", fonts, background);
                model->mLayout.SetVisible(false);
                model->mReady = true;
            }
    }

private:
    bool mInitialized;

public:
    std::vector<std::tr1::shared_ptr<Model> > mModels;
    int mAnimFrame;

    virtual void Update(CLayout* pLayout, const std::vector<u16>& rSpecies) {
            if (!mInitialized) {
                mInitialized = true;
                for (unsigned i = 0; i < mModels.size(); i++) {
                    pLayout->AttachLayout(FormatString("MODEL%d", i).c_str(), &mModels[i]->mLayout);
                }
            }
            for (unsigned i = 0; i < mModels.size(); i++) {
                if (i < rSpecies.size()) {
                    mModels[i]->mReady = false;
                    mModels[i]->mSpecies = rSpecies[i];
                } else {
                    mModels[i]->mReady = true;
                    mModels[i]->mSpecies = 0;
                }
                mModels[i]->mLayout.SetVisible(false);
                mModels[i]->mTex.reset();
            }
            mAnimFrame = 60 - mModels.size();
    }
};

class CDlgCollectionItem : public IUIListItem {
public:
    CDlgCollectionItem(const std::tr1::shared_ptr<CDlgCollectionItemModel>& rModel, const std::vector<u16>& rSpecies)
        : mModel(rModel), mSpecies(rSpecies) {}
    virtual ~CDlgCollectionItem() {}
    virtual bool IsSelectable() const { return true; }
    virtual void Update(CUIList* pList, CLayout* pLayout) { mModel->Update(pLayout, mSpecies); }

private:
    std::tr1::shared_ptr<CDlgCollectionItemModel> mModel;
    std::vector<u16> mSpecies;
};

class CSpeciesIdSet {
public:
    CSpeciesIdSet() {
            int level = std::min(std::max(0, static_cast<int>(GetSaveData(-1)->mData.mFlags[1])), 2);
            if (level >= 0) {
                for (u16 id = 1; id <= 0x96; id++) {
                    mIds.insert(id);
                }
            }
            if (level >= 1) {
                for (u16 id = 0x183; id <= 0x1ec; id++) {
                    mIds.insert(id);
                }
            }
            if (IGameSession::GetInstance()->GetPiiCollectionBox()->GetCollectionType(sSecretSpecies) == 3) {
                mIds.insert(sSecretSpecies);
            }
    }

    std::set<u16> mIds;
};

class CDlgCollectionImpl {
public:
    explicit CDlgCollectionImpl(int player)
        : mPlayer(player), mState(0), mComplete(false), mUIManager(new CUIManager), mRootLayout(new BUILayout),
          mList(new CUIList) {}

    void Init() {
        std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
        const char* const* pArchives = gSharedLayoutArchives;
        mRootLayout->Load(pArchives, "collection_root", fonts, false);
        mList->Load(pArchives, "collection_list", fonts, false);
        mUIManager->AddChild(mRootLayout);
        mRootLayout->AttachChild("COLLECTION_LIST", mList);
        mList->SetScrollMode(1);
        mList->GetScrollCallbacks().Add(
            std::tr1::bind(&CDlgCollectionImpl::PlayScrollSound, this, std::tr1::placeholders::_1));
        mItemModel.reset(new CDlgCollectionItemModel);
        mItemModel->Load(false);
        mUIManager->SetFocusedUI(mList.get());
        int collected = 0;
        CPiiCollectionBox* pBox = IGameSession::GetInstance()->GetPiiCollectionBox();
        CSpeciesIdSet species;
        int total = species.mIds.size();
        unsigned perRow = mItemModel->mModels.size();
        while (!species.mIds.empty()) {
            std::vector<u16> row;
            while (row.size() < perRow && !species.mIds.empty()) {
                std::set<u16>::iterator it = species.mIds.begin();
                u16 id = *it;
                row.push_back(id);
                species.mIds.erase(it);
                int type = pBox->GetCollectionType(id);
                if (type != 2 && type == 3) {
                    collected++;
                }
            }
            mList->GetItems().push_back(
                std::tr1::shared_ptr<CDlgCollectionItem>(new CDlgCollectionItem(mItemModel, row)));
        }
        mList->UpdateSelection();
        mRootLayout->SetText("COMP_NUM_GOT", FormatString(L"%d", collected).c_str());
        mRootLayout->SetText("COMP_NUM_MAX", FormatString(L"%d", total).c_str());
        mComplete = GetSaveData(-1)->mData.mFlags[1] >= 1 && collected == total;
        mFiber.Start(std::tr1::bind(&CDlgCollectionImpl::Run, this), 0x8000);
    }
    void PlayScrollSound(BUI* pUI) { PlaySound(0x16); }
    void Run() {
            YieldFiberStarted();
            if (mComplete && !IsGameFlagSet(0x12)) {
                PlaySound(0x17);
                mMsgWin = IDlgMsgWin::Create(false, true);
                mMsgWin->OpenMessage(GetMessageText(0x60075, gEmptyText));
                while (!mMsgWin->IsMessageClosed()) {
                    YieldFiber();
                }
                mMsgWin.reset();
                SetFlag(0x12, 1);
            }
            mRootLayout->GetLayout()->CreateMotion("begin")->Wait();
            mState = 0;
            for (;;) {
                IController* pController = IGameSession::GetInstance()->GetPlayer(mPlayer)->GetController();
                if (pController != nullptr && (pController->GetTrig() & 0x30)) {
                    mState = 1;
                } else {
                    mUIManager->DispatchFocusedInput(pController);
                }
                int state = mState;
                mState = 0;
                if (state == 1) {
                    break;
                }
                YieldFiber();
            }
            PlaySound(0x5a);
            mRootLayout->GetLayout()->CreateMotion("end")->Wait();
    }

    int mPlayer;
    int mState;
    bool mComplete;
    std::tr1::shared_ptr<CUIManager> mUIManager;
    std::tr1::shared_ptr<BUILayout> mRootLayout;
    std::tr1::shared_ptr<CUIList> mList;
    std::tr1::shared_ptr<CDlgCollectionItemModel> mItemModel;
    std::tr1::shared_ptr<IDlgMsgWin> mMsgWin;
    CFiber mFiber;
};

class CDlgCollection : public IDlgCollection, public std::tr1::enable_shared_from_this<CDlgCollection> {
public:
    CDlgCollection() {}
    virtual ~CDlgCollection() {}
    virtual bool Close() {
            if (mImpl) {
                mImpl->mState = 1;
            }
            GetCurrentGameMap()->UnregisterDialog(shared_from_this());
            return true;
    }
    virtual bool IsShown() { return mImpl; }
    virtual bool Show(int player) {
            if (mImpl) {
                return true;
            }
            {
                if (!GetCurrentGameMap()->RegisterDialog(shared_from_this())) {
                    return false;
                }
                GetCurrentGameMap()->SetPausing(true);
                mImpl.reset(new CDlgCollectionImpl(player));
                CDlgCollectionImpl* pImpl = mImpl.get();
                pImpl->Init();
            }
            return true;
    }
    virtual void Calc() {
            CDlgCollectionImpl* pImpl = mImpl.get();
            if (mImpl) {
                pImpl->mUIManager->Calc();
                CDlgCollectionItemModel* pModel = pImpl->mItemModel.get();
                bool canInitialize = true;
                CPiiCollectionBox* pBox = IGameSession::GetInstance()->GetPiiCollectionBox();
                for (unsigned i = 0; i < pModel->mModels.size(); i++) {
                    CDlgCollectionItemModel::Model* pItem = pModel->mModels[i].get();
                    if (canInitialize && !pItem->mReady) {
                        bool even = !(i & 1);
                        u16 species = pItem->mSpecies;
                        int type = pBox->GetCollectionType(species);
                        bool unknown = false;
                        if (type != 3 && type != 2) {
                            unknown = true;
                        }
                        pItem->mLayout.SetPaneVisible("UNKNOWN", unknown);
                        pItem->mLayout.SetPaneVisible("DEFEATED", type == 2);
                        pItem->mLayout.SetPaneVisible("GOT", type == 3);
                        pItem->mLayout.SetPaneVisible("RARE", false);
                        pItem->mLayout.SetPaneVisible("NAME", !unknown);
                        pItem->mLayout.SetPaneVisible("EVEN_BK", even);
                        pItem->mLayout.SetPaneVisible("ODD_BK", !even);
                        int normalRank = pBox->GetDefeatCount(species, 0);
                        pItem->mLayout.SetPaneVisible("N_SILVER", normalRank == 2);
                        pItem->mLayout.SetPaneVisible("N_GOLD", normalRank == 3);
                        pItem->mLayout.SetPaneVisible("N_BRONZE", normalRank == 1);
                        int hardRank = pBox->GetDefeatCount(species, 1);
                        pItem->mLayout.SetPaneVisible("H_SILVER", hardRank == 2);
                        pItem->mLayout.SetPaneVisible("H_GOLD", hardRank == 3);
                        pItem->mLayout.SetPaneVisible("H_BRONZE", hardRank == 1);
                        int easyRank = pBox->GetDefeatCount(species, 2);
                        pItem->mLayout.SetPaneVisible("E_SILVER", easyRank == 2);
                        pItem->mLayout.SetPaneVisible("E_GOLD", easyRank == 3);
                        pItem->mLayout.SetPaneVisible("E_BRONZE", easyRank == 1);
                        if (type == 3) {
                            bool rare = false;
                            u32 rareMask = pBox->GetRareColorMask(species);
                            u32 normalMask = pBox->GetNormalColorMask(species);
                            if (normalMask == 0 && rareMask == 0) {
                                normalMask = 1;
                            }
                            pModel->mModels[i]->mTex.reset(new CPiiTex(256, 256));
                            pItem->mLayout.SetTexture("GOT_TEX", pItem->mTex->GetTexMap(), -1);
                            for (u8 form = 0; form < 32; form++) {
                                if (!IsValidPiiForm(species, form)) {
                                    break;
                                }
                                const PiiSpeciesData* pSpecies = GetPiiSpeciesData(species, form);
                                u32 bit = 1 << form;
                                u32 normalBit = normalMask & bit;
                                u32 rareBit = rareMask & bit;
                                for (int color = 0; color <= 1; color++) {
                                    if (color == 0) {
                                        if (normalBit == 0) {
                                            continue;
                                        }
                                    } else {
                                        if (rareBit == 0) {
                                            continue;
                                        }
                                        rare = true;
                                    }
                                    std::tr1::shared_ptr<CPiiPersonalData> personalData = CreatePiiPersonalData(
                                        IGameSession::GetInstance()->GetOyaInfo(), pSpecies, pSpecies->GetDefaultSex());
                                    if (color != 0) {
                                        personalData->SetRareColorRnd();
                                    } else {
                                        personalData->SetNormalColorRnd();
                                    }
                                    unsigned index = pModel->mModels[i]->mTex->AddPii(personalData);
                                    if (index != 0) {
                                        pModel->mModels[i]->mTex->SetPiiVisible(index, false);
                                    }
                                }
                            }
                            pItem->mLayout.SetPaneVisible("RARE", rare);
                        } else if (type == 2) {
                            const PiiSpeciesData* pSpecies = GetPiiSpeciesData(species, 0);
                            std::tr1::shared_ptr<CPiiPersonalData> personalData = CreatePiiPersonalData(
                                IGameSession::GetInstance()->GetOyaInfo(), pSpecies, pSpecies->GetDefaultSex());
                            personalData->SetNormalColorRnd();
                            pModel->mModels[i]->mTex.reset(new CPiiTex(personalData, 256, 256));
                            pItem->mLayout.SetTexture("DEFEATED_TEX", pItem->mTex->GetTexMap(), -1);
                        }
                        pItem->mLayout.SetText("NAME", GetPiiSpeciesData(species, 0)->GetName());
                        pItem->mLayout.SetVisible(true);
                        pModel->mModels[i]->mReady = true;
                        canInitialize = false;
                    }
                    CPiiTex* pTex = pModel->mModels[i]->mTex.get();
                    if (pTex) {
                        if (pModel->mAnimFrame == 0 && pTex->GetPiiCount() >= 2) {
                            unsigned current = 0;
                            while (current < pTex->GetPiiCount()) {
                                if (pTex->IsPiiVisible(current)) {
                                    pTex->SetPiiVisible(current, false);
                                    break;
                                }
                                current++;
                            }
                            pTex->SetPiiVisible((current + 1) % pTex->GetPiiCount(), true);
                        }
                        pModel->mModels[i]->mTex->Calc();
                    }
                    pItem->mLayout.Calc();
                }
                pModel->mAnimFrame = (pModel->mAnimFrame + 1) % 60;
                if (pImpl->mMsgWin) {
                    pImpl->mMsgWin->Calc();
                }
                pImpl->mFiber.Resume();
                if (mImpl->mFiber.IsFinished()) {
                    mImpl.reset();
                    GetCurrentGameMap()->UnregisterDialog(shared_from_this());
                }
            }
    }
    virtual void CalcMtx() {
            CDlgCollectionImpl* pImpl = mImpl.get();
            if (mImpl) {
                std::vector<std::tr1::shared_ptr<CDlgCollectionItemModel::Model> >& rModels = pImpl->mItemModel->mModels;
                for (unsigned i = 0; i < rModels.size(); i++) {
                    if (rModels[i]->mTex) {
                        rModels[i]->mTex->CalcMtx();
                    }
                }
                if (pImpl->mMsgWin) {
                    pImpl->mMsgWin->CalcMtx();
                }
            }
    }
    virtual void CalcView() {
            CDlgCollectionImpl* pImpl = mImpl.get();
            if (mImpl) {
                std::vector<std::tr1::shared_ptr<CDlgCollectionItemModel::Model> >& rModels = pImpl->mItemModel->mModels;
                for (unsigned i = 0; i < rModels.size(); i++) {
                    if (rModels[i]->mTex) {
                        rModels[i]->mTex->CalcView();
                    }
                }
                if (pImpl->mMsgWin) {
                    pImpl->mMsgWin->CalcView();
                }
            }
    }
    virtual void Draw() {
            CDlgCollectionImpl* pImpl = mImpl.get();
            if (mImpl) {
                pImpl->mUIManager->Draw();
                CDlgCollectionItemModel* pModel = pImpl->mItemModel.get();
                for (unsigned i = 0; i < pModel->mModels.size(); i++) {
                    pModel->mModels[i]->mLayout.Draw();
                }
                if (pImpl->mMsgWin) {
                    pImpl->mMsgWin->Draw();
                }
            }
    }

private:
    std::tr1::shared_ptr<CDlgCollectionImpl> mImpl;
};

}

std::tr1::shared_ptr<IDlgCollection> IDlgCollection::Create(bool) {
    return std::tr1::shared_ptr<CDlgCollection>(new CDlgCollection);
}
