#include <game/IDlgCollection.h>
#include <game/MathUtil.h>

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

namespace std {

template <>
inline void __vec_deleter<unsigned short, allocator<unsigned short> >::construct_at_end(
    const unsigned short* pFirst, const unsigned short* pLast, Metrowerks::int2type<true>) {
    ptrdiff_t n = pLast - pFirst;
    memmove(data_ + size_, pFirst, (static_cast<const unsigned short*>(pLast) - pFirst) * sizeof(unsigned short));
    size_ += n;
}

}

template <typename T>
inline void AllocateCollectionVector(std::__vec_deleter<T, std::allocator<T> >& rVector, unsigned long count) {
    rVector.allocate(count, typename std::__vec_deleter<T, std::allocator<T> >::pod_type());
}

template <typename T>
inline void CopyCollectionVector(std::__vec_deleter<T, std::allocator<T> >& rVector, const T* first, const T* last) {
    unsigned long count = last - first;
    if (count != 0) {
        AllocateCollectionVector(rVector, count);
        rVector.construct_at_end(first, last, typename std::__vec_deleter<T, std::allocator<T> >::pod_type());
    }
}

namespace std {

template <>
inline __vec_constructor<unsigned short, allocator<unsigned short>, false>::__vec_constructor(
    const __vec_constructor& other) : base() {
    CopyCollectionVector(*this, other.data_, other.data_ + other.size_);
}

}

inline BUI::Callback BUI::CCallbackListOwner::Add(const std::tr1::function<void(BUI*)>& rCallback) {
    if (rCallback) {
        Callback callback(new std::tr1::function<void(BUI*)>(rCallback));
        if (mpList == nullptr) mpList = new CallbackList();
        mpList->push_back(callback);
        return callback;
    }
    return Callback();
}






namespace {

enum { SECRET_SPECIES = 0x97 };

u16 sSecretSpecies = SECRET_SPECIES;

inline u32 GetFormMask(u32 mask, u8 form) { return mask & (1 << form); }

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

    unsigned GetModelCount() const { return mModels.size(); }

    void Calc() {
        bool canInitialize = true;
        CPiiCollectionBox* pBox = IGameSession::GetInstance()->GetPiiCollectionBox();
        for (unsigned i = 0; i < mModels.size(); i++) {
            Model* pItem = mModels[i].get();
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
                    mModels[i]->mTex.reset(new CPiiTex(256, 256));
                    mModels[i]->mLayout.SetTexture("GOT_TEX", mModels[i]->mTex->GetTexMap(), -1);
                    for (u8 form = 0; form < 32; form++) {
                        if (!IsValidPiiForm(species, form)) {
                            break;
                        }
                        const PiiSpeciesData* pSpecies = GetPiiSpeciesData(species, form);
                        u32 normalBit = GetFormMask(normalMask, form);
                        u32 rareBit = GetFormMask(rareMask, form);
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
                            unsigned index = mModels[i]->mTex->AddPii(personalData);
                            if (index != 0) {
                                mModels[i]->mTex->SetPiiVisible(index, false);
                            }
                        }
                    }
                    pItem->mLayout.SetPaneVisible("RARE", rare);
                } else if (type == 2) {
                    const PiiSpeciesData* pSpecies = GetPiiSpeciesData(species, 0);
                    std::tr1::shared_ptr<CPiiPersonalData> personalData = CreatePiiPersonalData(
                        IGameSession::GetInstance()->GetOyaInfo(), pSpecies, pSpecies->GetDefaultSex());
                    personalData->SetNormalColorRnd();
                    mModels[i]->mTex.reset(new CPiiTex(personalData, 256, 256));
                    mModels[i]->mLayout.SetTexture("DEFEATED_TEX", mModels[i]->mTex->GetTexMap(), -1);
                }
                pItem->mLayout.SetText("NAME", GetPiiSpeciesData(species, 0)->GetName());
                pItem->mLayout.SetVisible(true);
                mModels[i]->mReady = true;
                canInitialize = false;
            }
            CPiiTex* pTex = mModels[i]->mTex.get();
            if (mModels[i]->mTex) {
                if (mAnimFrame == 0 && pTex->GetPiiCount() >= 2) {
                    unsigned current = 0;
                    while (current < pTex->GetPiiCount()) {
                        if (pTex->IsPiiVisible(current)) {
                            pTex->SetPiiVisible(current, false);
                            break;
                        }
                        current++;
                    }
                    mModels[i]->mTex->SetPiiVisible((current + 1) % pTex->GetPiiCount(), true);
                }
                mModels[i]->mTex->Calc();
            }
            pItem->mLayout.Calc();
        }
        mAnimFrame = (mAnimFrame + 1) % 60;
    }

    void CalcMtx() {
        for (unsigned i = 0; i < mModels.size(); i++) {
            if (mModels[i]->mTex) {
                mModels[i]->mTex->CalcMtx();
            }
        }
    }

    void CalcView() {
        for (unsigned i = 0; i < mModels.size(); i++) {
            if (mModels[i]->mTex) {
                mModels[i]->mTex->CalcView();
            }
        }
    }

    void Draw() {
        for (unsigned i = 0; i < mModels.size(); i++) {
            mModels[i]->mLayout.Draw();
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
    CDlgCollectionItem(std::tr1::shared_ptr<CDlgCollectionItemModel> model, const std::vector<u16>& rSpecies)
        : mModel(model), mSpecies(rSpecies) {}
    virtual ~CDlgCollectionItem() {}
    virtual bool IsSelectable() const { return true; }
    virtual void Update(CUIList* pList, CLayout* pLayout) { mModel->Update(pLayout, mSpecies); }

private:
    std::tr1::shared_ptr<CDlgCollectionItemModel> mModel;
    std::vector<u16> mSpecies;
};

inline std::set<u16> GetCollectableSpecies() {
    std::set<u16> ids;
    int level = Clamp<int>(GetSaveData(-1)->mData.mCollectionLevel, 0, 2);
    if (level >= 0) {
        for (u16 id = 1; id <= 0x96; id++) {
            ids.insert(id);
        }
    }
    if (level >= 1) {
        for (u16 id = 0x183; id <= 0x1ec; id++) {
            ids.insert(id);
        }
    }
    if (IGameSession::GetInstance()->GetPiiCollectionBox()->GetCollectionType(SECRET_SPECIES) == 3) {
        ids.insert(sSecretSpecies);
    }
    return ids;
}

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
        std::set<u16> species = GetCollectableSpecies();
        int total = species.size();
        unsigned perRow = mItemModel->GetModelCount();
        while (!species.empty()) {
            std::vector<u16> row;
            while (row.size() < perRow && !species.empty()) {
                u16 id = *species.begin();
                row.push_back(id);
                species.erase(species.begin());
                int type = pBox->GetCollectionType(id);
                if (type != 2 && type == 3) {
                    collected++;
                }
            }
            std::tr1::shared_ptr<CDlgCollectionItem> item(new CDlgCollectionItem(mItemModel, row));
            mList->GetItems().push_back(item);
        }
        mList->UpdateSelection();
        mList->GetLayout()->SetText("COMP_NUM_GOT", FormatString(L"%d", collected).c_str());
        mList->GetLayout()->SetText("COMP_NUM_MAX", FormatString(L"%d", total).c_str());
        mComplete = GetSaveData(-1)->mData.mCollectionLevel >= 1 && collected == total;
        mFiber.Start(std::tr1::bind(&CDlgCollectionImpl::Run, this), 0x8000);
    }
    void PlayScrollSound(BUI* pUI) { PlaySound(0x16); }
    void Run() {
            YieldFiberStarted();
            if (mComplete && !IsGameFlagSet(0x12)) {
                PlaySound(0x17);
                mMsgWin = IDlgMsgWin::Create(false, true);
                const wchar_t* pText = GetMessageString(0x60075, nullptr);
                mMsgWin->OpenMessage(pText);
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

    void CalcViewModels() { mItemModel->CalcView(); }
    void CalcMtxModels() { mItemModel->CalcMtx(); }
    void DrawModels() { mItemModel->Draw(); }

    void CalcModels() { mItemModel->Calc(); }

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
            if (!mImpl) {
                if (!GetCurrentGameMap()->RegisterDialog(shared_from_this())) {
                    return false;
                }
                GetCurrentGameMap()->SetPausing(true);
                mImpl.reset(new CDlgCollectionImpl(player));
                Initialize();
            }
            return true;
    }
    virtual void Calc() {
            CDlgCollectionImpl* pImpl = mImpl.get();
            if (mImpl) {
                pImpl->mUIManager->Calc();
                pImpl->CalcModels();
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
                pImpl->CalcMtxModels();
                if (pImpl->mMsgWin) {
                    pImpl->mMsgWin->CalcMtx();
                }
            }
    }
    virtual void CalcView() {
            CDlgCollectionImpl* pImpl = mImpl.get();
            if (mImpl) {
                pImpl->CalcViewModels();
                if (pImpl->mMsgWin) {
                    pImpl->mMsgWin->CalcView();
                }
            }
    }
    virtual void Draw() {
            CDlgCollectionImpl* pImpl = mImpl.get();
            if (mImpl) {
                pImpl->mUIManager->Draw();
                pImpl->DrawModels();
                if (pImpl->mMsgWin) {
                    pImpl->mMsgWin->Draw();
                }
            }
    }

private:
    void Initialize() { mImpl->Init(); }

    std::tr1::shared_ptr<CDlgCollectionImpl> mImpl;
};

}

std::tr1::shared_ptr<IDlgCollection> IDlgCollection::Create(bool) {
    return std::tr1::shared_ptr<CDlgCollection>(new CDlgCollection);
}
