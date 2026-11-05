#include <game/ui/detail/CBattleRoyalDisplay.h>

typedef std::wstring (*BattleFormatFunction)(const wchar_t*, ...);
template <typename T> inline BattleFormatFunction PrepareBattleNumberFormat();
namespace {

void CBattleRoyalDisplay::Load(const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, bool background) {
    mLayout.Load(gSharedLayoutArchives, "battleroyal", rFonts, background);
    mLayout.SetText("ato_c", GetMessageString(393454, nullptr));
    mLayout.SetText("hiki_c", GetMessageString(393455, nullptr));
    mLayout.SetText("title", GetMessageString(393456, nullptr));
    mTimePhase.SetTarget(&mLayout, "TIME");
    mEnemyPhase.SetTarget(&mLayout, "ENEMY");
}

inline int CBattleRoyalDisplay::GetEnemyCount() {
    return mEnemyCount;
}

inline void CBattleRoyalDisplay::DecEnemyCount() {
    if (mEnemyCount > 0) {
        mEnemyCount--;
    }
}

inline void CBattleRoyalDisplay::SetEnemyCount(int count) {
    mEnemyCount = count;
}

inline void CBattleRoyalDisplay::Hide() {
    mTimePhase.Hide();
    mEnemyPhase.Hide();
}

inline void CBattleRoyalDisplay::Show() {
    mTimePhase.Show();
    mEnemyPhase.Show();
}

inline void CBattleRoyalDisplay::Draw() {
    if (mTimePhase.IsVisible() || mEnemyPhase.IsVisible()) {
        mLayout.Draw();
    }
}

inline void CBattleRoyalDisplay::Calc() {
    PrepareBattleNumberFormat<int>();
    mTimePhase.Calc();
    mEnemyPhase.Calc();

    int seconds = GetCurrentGameMap()->GetTimerFrame() / 60;
    seconds = std::max(0, seconds);
    mLayout.SetText("time", FormatString(L"%d", seconds).c_str());
    mLayout.SetText("count", FormatString(L"%d", mEnemyCount).c_str());

    f32 length = mLayout.GetMotionLength("loop");
    f32 remaining = 60 - GetCurrentGameMap()->GetTimerFrame() % 60;
    mLayout.CreateMotion("loop", "ROTATE_ANM", length * (remaining / 60.0f), 1.0f);

    u32 timeActive = mTimePhase.IsActive();
    if (timeActive == 1 && mFinished) {
        mFinished = false;
        mLayout.CreateMotion("get", "TIME", 0.0f, 1.0f);
        mLayout.QueueMotion("loop", "TIME", 0.0f, 1.0f);
    }

    mLayout.Calc();
}

inline void CBattleRoyalDisplay::Finish() {
    mFinished = true;
}

}

std::tr1::shared_ptr<IBattleRoyalDisplay> IBattleRoyalDisplay::Create(bool background) {
    std::tr1::shared_ptr<CBattleRoyalDisplay> pDisplay(new CBattleRoyalDisplay);
    CBattleRoyalDisplay* pBattleRoyalDisplay = pDisplay.get();
    pBattleRoyalDisplay->Load(GetDefaultFontMap(), background);
    return pDisplay;
}

template <typename T> struct BattleNumberFormatDefinition {
    friend std::wstring FormatString(const wchar_t* pFormat, ...) {
        std::wstring result;
        va_list args;
        va_start(args, pFormat);
        VFormatString(result, pFormat, args);
        va_end(args);
        return result;
    }
};

template <typename T> inline BattleFormatFunction PrepareBattleNumberFormat() {
    BattleNumberFormatDefinition<T> definition;
    return FormatString;
}
