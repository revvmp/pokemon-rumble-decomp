#include <game/CHPBar.h>

#include <game/BWorldObj.h>
#include <game/CPii.h>
#include <game/CPiiPersonalData.h>
#include <game/CPlayer.h>
#include <game/FrameStep.h>
#include <game/IGameSession.h>
#include <game/LayoutResources.h>
#include <game/MathUtil.h>
#include <game/Sound.h>
#include <game/StringFormat.h>
#include <nw4r/lyt/lyt_layout.h>
#include <nw4r/lyt/lyt_pane.h>
#include <nw4r/snd/snd_SoundHandle.h>

namespace {

nw4r::snd::SoundHandle sDamageSoundHandle;
int sDamageSoundCount;

const char* const sPlayerPaneNames[4] = {"P0_P", "P1_P", "P2_P", "P3_P"};

}

class CHPBar::CDamage {
public:
    CDamage(Kind kind, int value) : mCount(1), mMotion() {
        const char* pName;
        if (value >= 0) {
            pName = "hp_heal";
        } else if (kind == KIND_PLAYER) {
            pName = "hp_player_damage";
        } else {
            pName = "hp_enemy_damage";
        }
        mLayout.Load(gSharedLayoutArchives, pName, GetDefaultFontMap(), false);
        if (value < 0) {
            value = -value;
        }
        mLayout.SetText("TEXT", FormatString(L"%d", value).c_str());
        mMotion = mLayout.CreateMotion("show", "TEXT", 0.0f, 1.0f);
        mLayout.QueueMotion("move1");
    }

    int mCount;
    CLayout mLayout;
    std::tr1::shared_ptr<CLayout::CMotionProgram> mMotion;
    u32 mReserved[6];
};

CHPBar::CHPBar(u32 type) :
    mFlags(0), mType(type), mKind(KIND_UNINITIALIZED), mPlayerNo(-1), mAnimationState(0), mHPRate(1.0f), mDelayedHPRate(1.0f),
    mDelayFrames(0), mpBar(nullptr), mBarWidth(0.0f), mpYellow(nullptr), mpGreen(nullptr), mpLevel(nullptr), mHasDamageSound(false) {}

CHPBar::~CHPBar() {
    if (mHasDamageSound) {
        if (--sDamageSoundCount == 0) {
            StopSound(&sDamageSoundHandle, nullptr, 0.0f);
        }
    }
}

void CHPBar::Initialize(int playerNo, CPiiPersonalData* pPersonalData, bool background) {
    mHPRate = mDelayedHPRate = pPersonalData->GetHPRatio();
    std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
    const char* pName;
    if (playerNo >= 0) {
        mKind = KIND_PLAYER;
        mPlayerNo = playerNo;
        pName = "hp_player";
    } else if (pPersonalData->IsBoss()) {
        mKind = KIND_BOSS;
        pName = "hp_boss";
    } else {
        mKind = KIND_ENEMY;
        pName = "hp_enemy";
    }
    mLayout.Load(gSharedLayoutArchives, pName, fonts, background);
    mpBar = mLayout.FindPane("BAR");
    nw4r::lyt::Pane* pSize = mLayout.FindPane("SIZE");
    if (pSize) {
        mBarWidth = pSize->GetSize().width;
    } else {
        mBarWidth = mLayout.FindPane("RED")->GetSize().width;
    }
    mpYellow = mLayout.FindPane("YELLOW");
    mpGreen = mLayout.FindPane("GREEN");
    mpLevel = mLayout.FindPane("LEVEL");
    if (mKind == KIND_PLAYER) {
        for (int i = 0; i < 4; i++) {
            nw4r::lyt::Pane* pPane = mLayout.FindPane(sPlayerPaneNames[i]);
            if (pPane) {
                mLayout.SetVisible(pPane, mPlayerNo == i);
            }
        }
    }
    mLayout.SetVisible(false);
}

void CHPBar::SetAnimationState(int state) {
    if (state == mAnimationState) {
        return;
    }
    switch (state) {
    case 0:
        if (mAnimationState == 1) {
            mLayout.CreateMotion("gauge_out");
        }
        break;
    case 1:
        if (mAnimationState == 2) {
            mLayout.QueueMotion("gauge_out");
            mLayout.QueueMotion("gauge_in");
        } else {
            mLayout.CreateMotion("gauge_in");
        }
        mLayout.QueueMotion("gauge_loop");
        break;
    case 2:
        mLayout.CreateMotion("player_out");
        break;
    case 3:
        if (mAnimationState == 0) {
            if (!(mFlags & FLAG_PLAYER_DOWN)) {
                mLayout.CreateMotion("player_loop");
            } else {
                mLayout.CreateMotion("player_in");
                mLayout.QueueMotion("player_loop");
            }
        } else {
            mLayout.CreateMotion("player_in");
            mLayout.QueueMotion("player_loop");
        }
        break;
    }
    mAnimationState = state;
}

void CHPBar::Update(BWorldObj* pObj) {
    mFlags |= FLAG_UPDATED;
    CPii* pPii = DynamicCast<CPii>(pObj);
    std::tr1::shared_ptr<CPiiPersonalData> personalData;
    if (pPii) {
        if (pPii->BWorldObj::IsVisible()) {
            mPosition = pPii->GetPositionRef();
            mFlags |= FLAG_POSITION_VALID;
        }
        personalData = pPii->GetPersonalData();
    }
    if (personalData) {
        mHPRate = personalData->GetHPRatio();
        int damage = personalData->GetDamageTotal();
        int heal = personalData->GetHealTotal();
        if (damage < 0 || heal > 0) {
            for (std::list<std::tr1::shared_ptr<CDamage> >::iterator it = mDamage.begin(); it != mDamage.end(); ++it) {
                CDamage* pDamage = it->get();
                pDamage->mCount++;
                pDamage->mLayout.AppendMotion(FormatString("move%d", pDamage->mCount).c_str());
            }
            std::tr1::shared_ptr<CDamage> newDamage;
            bool startGauge = true;
            if (damage < 0) {
                newDamage.reset(new CDamage(mKind, damage));
                personalData->ResetDamage();
            } else if (heal > 0) {
                newDamage.reset(new CDamage(mKind, heal));
                personalData->ResetHeal();
                if (pPii && mKind == KIND_PLAYER) {
                    CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(mPlayerNo);
                    if (pPlayer->IsJoined() && pPlayer->GetPendingPersonalData()) {
                        startGauge = false;
                    }
                }
            }
            mDamage.push_back(newDamage);
            mDelayFrames = 60;
            if (startGauge) {
                SetAnimationState(1);
            }
        }
        if (mKind == KIND_PLAYER) {
            if (0.0f < mHPRate && mHPRate < 0.2f) {
                if (!mHasDamageSound) {
                    mHasDamageSound = true;
                    StopSound(&sDamageSoundHandle, nullptr, 0.0f);
                    PlaySound(0x32C, &sDamageSoundHandle, nullptr);
                    sDamageSoundCount++;
                }
            } else if (mHasDamageSound) {
                mHasDamageSound = false;
                if (--sDamageSoundCount == 0) {
                    StopSound(&sDamageSoundHandle, nullptr, 0.0f);
                }
            }
        }
        bool bossHidden = false;
        if (mKind == KIND_BOSS && personalData->IsBossHideHP()) {
            bossHidden = true;
        }
        if (bossHidden) {
            mFlags |= FLAG_BOSS_HIDDEN;
        } else {
            mFlags &= ~FLAG_BOSS_HIDDEN;
        }
        if (pPii && mKind == KIND_PLAYER) {
            if (!(mFlags & FLAG_PLAYER_DOWN)) {
                if (mAnimationState == 0) {
                    if (pPii->IsPlayingMotion(109)) {
                        SetAnimationState(3);
                    } else {
                        SetAnimationState(2);
                        mFlags |= FLAG_PLAYER_DOWN;
                    }
                } else if (!pPii->IsPlayingMotion(109)) {
                    SetAnimationState(2);
                    mFlags |= FLAG_PLAYER_DOWN;
                }
            } else {
                CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(mPlayerNo);
                bool pending = pPlayer->IsJoined() && pPlayer->GetPendingPersonalData();
                if (pending) {
                    SetAnimationState(3);
                } else if (mAnimationState == 3) {
                    SetAnimationState(2);
                } else if (mAnimationState == 2 && mHPRate <= 0.2f) {
                    SetAnimationState(1);
                }
            }
        }
        if (mpLevel) {
            mLayout.SetVisible(mpLevel, false);
        }
    }
}

void CHPBar::SetEnabled(bool enabled) {
    if (!enabled) {
        mFlags |= FLAG_DISABLED;
    } else {
        mFlags &= ~FLAG_DISABLED;
    }
}

bool CHPBar::IsVisible() const {
    return (mFlags & FLAG_VISIBLE) != 0;
}

u32 CHPBar::GetType() const {
    return mType;
}

void CHPBar::Draw() {
    if (mFlags & FLAG_CALCULATED) {
        if (!(mFlags & FLAG_VIEW_STARTED) || mKind != KIND_PLAYER) {
            mLayout.Draw();
        }
        if (mFlags & FLAG_POSITION_VALID) {
            for (std::list<std::tr1::shared_ptr<CDamage> >::iterator it = mDamage.begin(); it != mDamage.end(); ++it) {
                (*it)->mLayout.Draw();
            }
        }
    }
}

void CHPBar::CalcView(const nw4r::math::MTX34& rView, const nw4r::math::MTX44& rProjection) {
    if (!(mFlags & FLAG_UPDATED)) {
        mFlags |= FLAG_VIEW_STARTED;
    }
    mFlags &= ~(FLAG_UPDATED | FLAG_VISIBLE | FLAG_CALCULATED);
    if (!(mFlags & FLAG_POSITION_VALID) && static_cast<u32>(mKind) <= KIND_ENEMY) {
        return;
    }
    nw4r::math::VEC3 position = mPosition;
    PSMTXMultVec(rView, position, position);
    nw4r::math::VEC3TransformCoord(&position, &rProjection, &position);
    if (!(mFlags & FLAG_ON_SCREEN) && static_cast<u32>(mKind) <= KIND_ENEMY) {
        if (!(-1.2f < position.x) || !(position.x < 1.2f) || !(-1.2f < position.y) || !(position.y < 1.2f)) {
            mLayout.SetVisible(false);
            return;
        }
        mFlags |= FLAG_ON_SCREEN;
    }
    const nw4r::ut::Rect& rRect = mLayout.GetViewRect();
    f32 width = rRect.right - rRect.left;
    position.x *= width;
    position.x *= 0.5f;
    f32 height = rRect.bottom - rRect.top;
    position.y = -position.y * height;
    position.y *= 0.5f;
    position.z = 0.0f;
    if (static_cast<u32>(mKind) <= KIND_ENEMY) {
        mLayout.GetLayout()->GetRootPane()->SetTranslate(position);
    }
    mLayout.SetVisible(true);
    if (mKind == KIND_ENEMY && 1.0f == mHPRate) {
        mLayout.SetVisible(mpBar, false);
    } else if (mFlags & (FLAG_DISABLED | FLAG_BOSS_HIDDEN)) {
        mLayout.SetVisible(mpBar, false);
    } else {
        mLayout.SetVisible(mpBar, true);
        if (mHPRate < mDelayedHPRate) {
            f32 speed;
            if (mDelayFrames > 0) {
                speed = (mDelayedHPRate - mHPRate) / mDelayFrames * gFrameStep;
                mDelayFrames -= gFrameStep;
            } else {
                speed = 0.0f;
            }
            speed = Max(speed, 0.004f * gFrameStep);
            mDelayedHPRate = Min(1.0f, Max(mHPRate, mDelayedHPRate - speed));
        } else {
            mDelayedHPRate = mHPRate;
        }
        if (mAnimationState == 1 && mDelayedHPRate == mHPRate) {
            if (mKind != KIND_PLAYER || mHPRate > 0.2f) {
                SetAnimationState(0);
            }
        }
        mpYellow->SetSize(nw4r::lyt::Size(mBarWidth * mDelayedHPRate, mpYellow->GetSize().height));
        f32 greenWidth = mBarWidth * mHPRate;
        mpGreen->SetSize(nw4r::lyt::Size(greenWidth, mpGreen->GetSize().height));
        if (mHPRate != mDelayedHPRate) {
            mFlags |= FLAG_VISIBLE;
        }
    }
    mLayout.Calc();
    if (mFlags & FLAG_POSITION_VALID) {
        std::list<std::tr1::shared_ptr<CDamage> >::iterator it = mDamage.begin();
        while (it != mDamage.end()) {
            CDamage* pDamage = it->get();
            pDamage->mLayout.GetLayout()->GetRootPane()->SetTranslate(position);
            pDamage->mLayout.Calc();
            if (!pDamage->mMotion || pDamage->mMotion->IsEnded()) {
                it = mDamage.erase(it);
            } else {
                ++it;
            }
        }
    }
    if (!mDamage.empty()) {
        mFlags |= FLAG_VISIBLE;
    }
    mFlags |= FLAG_CALCULATED;
}
