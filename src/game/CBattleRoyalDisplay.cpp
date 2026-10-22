#include <game/detail/CBattleRoyalDisplay.h>

std::tr1::shared_ptr<IBattleRoyalDisplay> IBattleRoyalDisplay::Create(bool background) {
    std::tr1::shared_ptr<CBattleRoyalDisplay> pDisplay(new CBattleRoyalDisplay);
    CBattleRoyalDisplay* pBattleRoyalDisplay = pDisplay.get();
    pBattleRoyalDisplay->Load(GetDefaultFontMap(), background);
    return pDisplay;
}

namespace {

int kMinSeconds = 0;

void CBattleRoyalDisplay::Load(const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, bool background) {
    mLayout.Load(gSharedLayoutArchives, "battleroyal", rFonts, background);
    mLayout.SetText("ato_c", GetMessageText(393454, gEmptyText));
    mLayout.SetText("hiki_c", GetMessageText(393455, gEmptyText));
    mLayout.SetText("title", GetMessageText(393456, gEmptyText));
    mTimePhase.SetTarget(&mLayout, "TIME");
    mEnemyPhase.SetTarget(&mLayout, "ENEMY");
}

int CBattleRoyalDisplay::GetEnemyCount() {
    return mEnemyCount;
}

void CBattleRoyalDisplay::DecEnemyCount() {
    if (mEnemyCount > 0) {
        mEnemyCount--;
    }
}

void CBattleRoyalDisplay::SetEnemyCount(int count) {
    mEnemyCount = count;
}

void CBattleRoyalDisplay::Hide() {
    mTimePhase.Hide();
    mEnemyPhase.Hide();
}

void CBattleRoyalDisplay::Show() {
    mTimePhase.Show();
    mEnemyPhase.Show();
}

void CBattleRoyalDisplay::Draw() {
    if (mTimePhase.IsVisible() || mEnemyPhase.IsVisible()) {
        mLayout.Draw();
    }
}

void CBattleRoyalDisplay::Calc() {
    mTimePhase.Calc();
    mEnemyPhase.Calc();

    int seconds = GetCurrentGameMap()->GetTimerFrame() / 60;
    seconds = std::max(kMinSeconds, seconds);
    mLayout.SetText("time", FormatString(L"%d", seconds).c_str());
    mLayout.SetText("count", FormatString(L"%d", mEnemyCount).c_str());

    f32 length = mLayout.GetMotionLength("loop");
    f32 remaining = 60 - GetCurrentGameMap()->GetTimerFrame() % 60;
    mLayout.CreateMotion("loop", "ROTATE_ANM", length * (remaining / 60.0f), 1.0f);

    if (mTimePhase.IsActive() == true && mFinished) {
        mFinished = false;
        mLayout.CreateMotion("get", "TIME", 0.0f, 1.0f);
        mLayout.QueueMotion("loop", "TIME", 0.0f, 1.0f);
    }

    mLayout.Calc();
}

void CBattleRoyalDisplay::Finish() {
    mFinished = true;
}

}

std::wstring FormatString(const wchar_t* pFormat, ...) {
    std::wstring result;
    va_list args;
    va_start(args, pFormat);
    VFormatString(result, pFormat, args);
    va_end(args);
    return result;
}
