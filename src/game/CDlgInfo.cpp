#include <game/IDlgInfo.h>

#include <algorithm>
#include <game/CFiber.h>
#include <game/CLayout.h>
#include <game/CPlayer.h>
#include <game/CUIList.h>
#include <game/CUIManager.h>
#include <game/IController.h>
#include <game/IGameMap.h>
#include <game/IGameSession.h>
#include <game/IUIListItem.h>
#include <game/LayoutResources.h>
#include <game/PaneRectangle.h>
#include <game/Sound.h>
#include <game/StringFormat.h>
#include <math.h>
#include <memory>
#include <nw4r/lyt/lyt_pane.h>
#include <nw4r/lyt/lyt_textBox.h>
#include <string>
#include <vector>

#pragma pool_data off

namespace {

const char* sColumnPaneNames[4] = {"CAPTION", "COLON", "VALUE", "UNIT"};

struct Line {
    std::wstring mFirst;
    std::wstring mSecond;
    std::wstring mThird;
    std::wstring mText;

    bool HasText() const { return mText.size() != 0; }
    bool HasColumns() const { return mFirst.size() != 0 && mSecond.size() != 0; }
};

struct Page {
    std::wstring mTitle;
    std::vector<Line> mLines;
};

inline f32 GetTextWidth(nw4r::lyt::TextBox* pText) {
    nw4r::ut::Rect rect = pText->GetTextDrawRect();
    return ceil(rect.right - rect.left);
}

class CDlgInfoItemLine {
public:
    CDlgInfoItemLine() : mInitialized(false) {}

    void Load(bool background) {
            std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
            const char* const* pArchives = gSharedLayoutArchives;
            CLayout layout;
            layout.Load(pArchives, "info_list_item", fonts, background);
            unsigned count = 0;
            while (layout.FindPane(FormatString("LINE%d", count).c_str())) {
                count++;
            }
            mLayouts.resize(count);
            for (unsigned i = 0; i < count; i++) {
                std::tr1::shared_ptr<CLayout> line(new CLayout);
                line->Load(pArchives, "info_list_item_line", fonts, background);
                line->SetVisible(false);
                mLayouts[i] = line;
                for (int j = 0; j < 4; j++) {
                    mRectangles[j] = NErrMsg::GetPaneRectangle(line->FindPane(sColumnPaneNames[j]));
                }
            }
    }

    void Calc() {
        for (unsigned i = 0; i < mLayouts.size(); i++) {
            mLayouts[i]->Calc();
        }
    }
    void Draw() {
        for (unsigned i = 0; i < mLayouts.size(); i++) {
            mLayouts[i]->Draw();
        }
    }

    bool mInitialized;
    std::vector<std::tr1::shared_ptr<CLayout> > mLayouts;
    NErrMsg::PaneRectangle mRectangles[4];

    virtual void Update(CLayout* pLayout, const Page& rPage) {
            if (!mInitialized) {
                mInitialized = true;
                for (unsigned i = 0; i < mLayouts.size(); i++) {
                    pLayout->AttachLayout(FormatString("LINE%d", i).c_str(), mLayouts[i].get());
                }
            }
            pLayout->SetText("TITLE", rPage.mTitle.c_str());
            unsigned count = std::min(mLayouts.size(), rPage.mLines.size());
            f32 widths[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            unsigned i = 0;
            for (; i < count; i++) {
                std::tr1::shared_ptr<CLayout> line = mLayouts[i];
                const Line& rLine = rPage.mLines[i];
                bool hasText = rLine.HasText();
                bool hasColumns = false;
                if (!hasText && rLine.HasColumns()) {
                    hasColumns = true;
                }
                line->SetPaneVisible("CAPTION", hasColumns);
                line->SetPaneVisible("COLON", hasColumns);
                line->SetPaneVisible("VALUE", hasColumns);
                line->SetPaneVisible("UNIT", hasColumns);
                line->SetPaneVisible("TEXT", hasText);
                if (hasText) {
                    line->SetText("TEXT", rLine.mText.c_str());
                } else {
                    const wchar_t* texts[4] = {rLine.mFirst.c_str(), nullptr, rLine.mSecond.c_str(), rLine.mThird.c_str()};
                    for (int j = 0; j < 4; j++) {
                        if (j != 1) {
                            nw4r::lyt::TextBox* pText = line->SetText(sColumnPaneNames[j], texts[j]);
                            NErrMsg::PaneRectangle bounds = NErrMsg::GetPaneRectangle(pText);
                            bounds.right = bounds.left + 1.0f;
                            NErrMsg::SetPaneRectangle(pText, bounds);
                            f32 width = GetTextWidth(pText);
                            widths[j] = std::max(width, widths[j]);
                        }
                    }
                }
                line->SetVisible(true);
            }
            for (; i < mLayouts.size(); i++) {
                mLayouts[i]->SetVisible(false);
            }
            f32 gaps[3];
            gaps[0] = mRectangles[1].left - mRectangles[0].right;
            gaps[1] = mRectangles[2].left - mRectangles[1].right;
            gaps[2] = mRectangles[3].left - mRectangles[2].right;
            widths[1] = mRectangles[1].GetWidth();
            NErrMsg::PaneRectangle columns[4];
            columns[0] = mRectangles[0];
            f32 totalWidth = widths[0] + gaps[0] + widths[1] + gaps[1] + widths[2] + gaps[2] + widths[3];
            columns[0].left = -totalWidth * 0.5f;
            columns[0].right = columns[0].left + widths[0];
            for (int j = 1; j < 4; j++) {
                columns[j] = mRectangles[j];
                columns[j].left = columns[j - 1].right + gaps[j - 1];
                columns[j].right = columns[j].left + widths[j];
            }
            for (unsigned k = 0; k < count; k++) {
                std::tr1::shared_ptr<CLayout> line = mLayouts[k];
                for (int j = 0; j < 4; j++) {
                    NErrMsg::SetPaneRectangle(line->FindPane(sColumnPaneNames[j]), columns[j]);
                }
            }
    }
};

class CDlgInfoItem : public IUIListItem {
public:
    CDlgInfoItem(std::tr1::shared_ptr<CDlgInfoItemLine> model, const Page& rPage)
        : mModel(model), mPage(rPage) {}
    virtual ~CDlgInfoItem() {}
    virtual bool IsSelectable() const { return true; }
    virtual void Update(CUIList* pList, CLayout* pLayout) { mModel->Update(pLayout, mPage); }

private:
    std::tr1::shared_ptr<CDlgInfoItemLine> mModel;
    Page mPage;
};

class CDlgInfoImpl {
public:
    explicit CDlgInfoImpl(int player)
        : mPlayer(player), mState(0), mUIManager(new CUIManager), mRootLayout(new BUILayout), mList(new CUIList) {}

    void Init(const std::vector<std::tr1::shared_ptr<Page> >& rPages) {
        std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
        const char* const* pArchives = gSharedLayoutArchives;
        mRootLayout->Load(pArchives, "info_root", fonts, false);
        mList->Load(pArchives, "info_list", fonts, false);
        mUIManager->AddChild(mRootLayout);
        mRootLayout->AttachChild("INFO_LIST", mList);
        mList->SetScrollMode(1);
        mList->GetScrollCallbacks().Add(
            std::tr1::bind(&CDlgInfoImpl::PlayScrollSound, this, std::tr1::placeholders::_1));
        mItemLine.reset(new CDlgInfoItemLine);
        mItemLine->Load(false);
        mUIManager->SetFocusedUI(mList.get());
        for (std::vector<std::tr1::shared_ptr<Page> >::const_iterator it = rPages.begin(); it != rPages.end(); ++it) {
            std::tr1::shared_ptr<CDlgInfoItem> item(new CDlgInfoItem(mItemLine, **it));
            mList->GetItems().push_back(item);
        }
        mList->UpdateSelection();
        mFiber.Start(std::tr1::bind(&CDlgInfoImpl::Run, this), 0x8000);
    }
    void PlayScrollSound(BUI* pUI) { PlaySound(0x16); }
    void Run() {
            YieldFiberStarted();
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

    void CalcLines() { mItemLine->Calc(); }

    void DrawLines() { mItemLine->Draw(); }

    int mPlayer;
    int mState;
    std::tr1::shared_ptr<CUIManager> mUIManager;
    std::tr1::shared_ptr<BUILayout> mRootLayout;
    std::tr1::shared_ptr<CUIList> mList;
    std::tr1::shared_ptr<CDlgInfoItemLine> mItemLine;
    CFiber mFiber;
};

class CDlgInfo : public IDlgInfo, public std::tr1::enable_shared_from_this<CDlgInfo> {
public:
    CDlgInfo() {}
    virtual ~CDlgInfo() {}
    virtual void Calc() {
            CDlgInfoImpl* pImpl = mImpl.get();
            if (mImpl) {
                pImpl->mUIManager->Calc();
                pImpl->CalcLines();
                pImpl->mFiber.Resume();
                if (mImpl->mFiber.IsFinished()) {
                    mImpl.reset();
                    GetCurrentGameMap()->UnregisterDialog(shared_from_this());
                }
            }
    }
    virtual void Draw() {
            CDlgInfoImpl* pImpl = mImpl.get();
            if (mImpl) {
                pImpl->mUIManager->Draw();
                pImpl->DrawLines();
            }
    }
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
                mImpl.reset(new CDlgInfoImpl(player));
                Initialize();
            }
            return true;
    }
    virtual void Clear() {
            std::vector<std::tr1::shared_ptr<Page> >().swap(mPages);
    }
    virtual void SetTitle(int page, const wchar_t* pTitle) {
            GetPage(page)->mTitle = pTitle;
    }
    virtual void SetLine(int page, unsigned int line, const wchar_t* pFirst, const wchar_t* pSecond, const wchar_t* pThird) {
            Line& rLine = GetLine(page, line);
            rLine.mFirst = pFirst;
            rLine.mSecond = pSecond;
            rLine.mThird = pThird;
            rLine.mText.clear();
    }
    virtual void SetText(int page, unsigned int line, const wchar_t* pText) {
            GetLine(page, line).mText = pText;
    }

private:
    void Initialize() { mImpl->Init(mPages); }

    Line& GetLine(int page, unsigned int line) {
            std::tr1::shared_ptr<Page> pPage = GetPage(page);
            if (pPage->mLines.size() <= line) {
                pPage->mLines.resize(line + 1);
            }
            return pPage->mLines[line];
    }

    std::tr1::shared_ptr<Page> GetPage(int page) {
            if (mPages.size() <= page) {
                mPages.resize(page + 1);
            }
            if (!mPages[page]) {
                mPages[page].reset(new Page);
            }
            return mPages[page];
    }

    std::tr1::shared_ptr<CDlgInfoImpl> mImpl;
    std::vector<std::tr1::shared_ptr<Page> > mPages;
};

}

std::tr1::shared_ptr<IDlgInfo> IDlgInfo::Create(bool) {
    return std::tr1::shared_ptr<CDlgInfo>(new CDlgInfo);
}
