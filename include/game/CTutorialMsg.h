#ifndef GAME_CTUTORIALMSG_H
#define GAME_CTUTORIALMSG_H

#include <game/ICalcDraw.h>
#include <game/ITutorialMsg.h>
#include <game/LayoutResources.h>
#include <nw4r/lyt/lyt_pane.h>
#include <game/CLayout.h>
#include <memory>
#include <string>
#include <vector>
#include <game/CColorBtnTagProc.h>
#include <nw4r/lyt/lyt_textBox.h>

extern const char* spMessagePaneNames[2];

class CTutorialMsg : public ITutorialMsg {
public:
    class CMessage {
    public:
        std::wstring mText;
    };

void Load(bool background) {
    std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
    mLayout.Load(gSharedLayoutArchives, "tutorial_message", fonts, background);
    mVisible = false;
    mMessages.reserve(5);
    mCurrentPane = 0;
    mMessageIndex = 0;
    mpMessagePanes[0] = mLayout.FindPane("text0_c");
    mpMessagePanes[1] = mLayout.FindPane("text1_c");
    mpScrollPanes[0] = mLayout.FindPane("text0_root");
    mpScrollPanes[1] = mLayout.FindPane("text1_root");
    mLayout.CreateMotion("scroll", mpMessagePanes[0], 0.0f, 1.0f);
    mLayout.CalculateMtx(true);
    mStartX = mpMessagePanes[0]->GetTranslate().x;
    mLayout.StopMotion(mpMessagePanes[0]);
    nw4r::ut::Rect rect = mLayout.FindPane("spacing")->GetPaneRect();
    mPaneWidth = rect.right - rect.left;
    mLayout.CreateMotion("quick", mpScrollPanes[0], 0.0f, 1.0f);
    mLayout.CreateMotion("scroll", mpMessagePanes[0], 0.0f, 1.0f);
    mLayout.CalculateMtx(true);
    f32 messageStartX = mpMessagePanes[0]->GetTranslate().x;
    f32 scrollStartX = mpScrollPanes[0]->GetTranslate().x;
    mLayout.CreateMotion("quick", mpScrollPanes[0], 1.0f, 1.0f);
    mLayout.CreateMotion("scroll", mpMessagePanes[0], 1.0f, 1.0f);
    mLayout.CalculateMtx(true);
    f32 messageEndX = mpMessagePanes[0]->GetTranslate().x;
    f32 scrollEndX = mpScrollPanes[0]->GetTranslate().x;
    mScrolling = false;
    mScrollLength = 1.0f;
    mScrollRate = mScrollLength * ((scrollEndX - scrollStartX + (messageEndX - messageStartX)) / (messageEndX - messageStartX));
}


    
    
    
    
    

virtual void Show(const wchar_t* pText) {
    if (!mVisible) {
        mLayout.CreateMotion("begin");
        mVisible = true;
        mLayout.StopMotion(mpMessagePanes[0]);
        mLayout.StopMotion(mpMessagePanes[1]);
        mLayout.StopMotion(mpScrollPanes[0]);
        mLayout.StopMotion(mpScrollPanes[1]);
        mpScrollPanes[0]->SetSRTElement(0, 0.0f);
        mpScrollPanes[1]->SetSRTElement(0, 0.0f);
        mpMessagePanes[0]->SetSRTElement(0, mStartX);
        mpMessagePanes[1]->SetSRTElement(0, mStartX);
        mMotions[0].reset();
        mMotions[1].reset();
        mScrolling = false;
    }
    mMessages.push_back(CMessage());
    mMessages[mMessages.size() - 1].mText = pText;
    if (!mScrolling || !mMotions[mCurrentPane]) {
        if (mMotions[mCurrentPane] && !mMotions[mCurrentPane]->IsEnded(false)) {
            float frame = mMotions[mCurrentPane]->GetFrame();
            mMotions[mCurrentPane] = mLayout.CreateMotion("scroll", mpMessagePanes[mCurrentPane], frame, mScrollRate);
        }
        mMessageIndex = 0;
        mCurrentPane = (mCurrentPane + 1) % 2;
        nw4r::lyt::TextBox* pTextBox = mLayout.SetText(spMessagePaneNames[mCurrentPane], mMessages[0].mText.c_str());
        CColorBtnTagProc::Attach(pTextBox);
        nw4r::ut::Rect rect = pTextBox->GetTextDrawRect();
        mTextWidths[mCurrentPane] = rect.right - rect.left;
        mLayout.CreateMotion("quick", mpScrollPanes[mCurrentPane], 0.0f, 1.0f);
        mMotions[mCurrentPane] = mLayout.CreateMotion("scroll", mpMessagePanes[mCurrentPane], 0.0f, mScrollLength);
        mScrolling = true;
    } else if (mMessages.size() == 1) {
        float frame = mMotions[mCurrentPane]->GetFrame();
        mLayout.StopMotion(mpScrollPanes[mCurrentPane]);
        mMotions[mCurrentPane] = mLayout.CreateMotion("scroll", mpMessagePanes[mCurrentPane], frame, mScrollRate);
        int next = (mCurrentPane + 1) % 2;
        if (mMotions[next] && !mMotions[next]->IsEnded(false)) {
            float frame = mMotions[next]->GetFrame();
            mMotions[next] = mLayout.CreateMotion("scroll", mpMessagePanes[next], frame, mScrollRate);
        }
        mAppendPending = true;
    }
}
virtual void Clear() {
    mMessages.clear();
    mMessageIndex = 0xFFFF;
}
virtual void Close() {
    if (mVisible) {
        mLayout.CreateMotion("end");
        mVisible = false;
    }
}
virtual void Calc() {
    mLayout.Calc();
    if (mScrolling) {
        mLayout.CalculateMtx(true);
        float x = mpMessagePanes[mCurrentPane]->GetTranslate().x;
        x += mpScrollPanes[mCurrentPane]->GetTranslate().x;
        if (mStartX - x > mPaneWidth + mTextWidths[mCurrentPane]) {
            if (mMessages.size() == 0) {
                mScrolling = false;
            } else {
                mCurrentPane = (mCurrentPane + 1) % 2;
                ++mMessageIndex;
                if (mMessageIndex >= mMessages.size()) {
                    mMessageIndex = 0;
                }
                nw4r::lyt::TextBox* pText = mLayout.SetText(spMessagePaneNames[mCurrentPane], mMessages[mMessageIndex].mText.c_str());
                CColorBtnTagProc::Attach(pText);
                nw4r::ut::Rect rect = pText->GetTextDrawRect();
                mTextWidths[mCurrentPane] = rect.right - rect.left;
                mpScrollPanes[mCurrentPane]->SetSRTElement(0, 0.0f);
                if (mAppendPending) {
                    mLayout.CreateMotion("quick", mpScrollPanes[mCurrentPane], 0.0f, 1.0f);
                    mAppendPending = false;
                }
                mMotions[mCurrentPane] = mLayout.CreateMotion("scroll", mpMessagePanes[mCurrentPane], 0.0f, mScrollLength);
            }
        }
    }
}
virtual void Draw() {
    mLayout.Draw();
}

private:
    CLayout mLayout;
    bool mVisible;
    bool mScrolling;
    bool mAppendPending;
    int mCurrentPane;
    unsigned int mMessageIndex;
    f32 mTextWidths[2];
    nw4r::lyt::Pane* mpMessagePanes[2];
    nw4r::lyt::Pane* mpScrollPanes[2];
    std::tr1::shared_ptr<CLayout::CMotionProgram> mMotions[2];
    f32 mStartX;
    std::vector<CMessage> mMessages;
    f32 mScrollLength;
    f32 mPaneWidth;
    f32 mScrollRate;
};


#endif
