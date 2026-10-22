#include <game/CEnemyInfo.h>
#include <game/CPiiPersonalData.h>
#include <game/CPiiTex.h>
#include <game/IColorConst.h>
#include <game/IGameSession.h>
#include <game/LayoutResources.h>
#include <game/Message.h>
#include <game/SystemMessage.h>

extern int gFrameStep;

std::tr1::shared_ptr<IEnemyInfo> CEnemyInfo::Create(bool background)
{
    std::tr1::shared_ptr<CEnemyInfo> info(new CEnemyInfo);
    CEnemyInfo* pInfo = info.get();
    pInfo->Initialize(background);
    return info;
}


void CEnemyInfo::Draw()
{
    mLayout.Draw();
}

void CEnemyInfo::CalcView()
{
    if (mPiiTex) {
        mPiiTex->CalcView();
    }
}

void CEnemyInfo::CalcMtx()
{
    if (mPiiTex) {
        mPiiTex->CalcMtx();
    }
}

void CEnemyInfo::Calc()
{
    if (mFrames > gFrameStep) {
        mFrames -= gFrameStep;
    } else if (mFrames > 0) {
        Hide();
    }
    mLayout.Calc();
    if (mPiiTex) {
        mPiiTex->Calc();
    }
}
inline void CEnemyInfo::Hide()
{
    if (mFrames > 0) {
        mFrames = 0;
        mFlash = false;
        mLayout.CreateMotion("end");
        nw4r::lyt::Pane* first = mPanes[4];
        if (first) {
            mLayout.CreateMotion("end", mPanes[4], 0.0f, 1.0f);
            mLayout.AppendMotion("flash_end", first, 1000.0f, 1.0f);
        }
        nw4r::lyt::Pane* second = mPanes[5];
        if (second) {
            mLayout.CreateMotion("end", mPanes[5], 0.0f, 1.0f);
            mLayout.AppendMotion("flash_end", second, 1000.0f, 1.0f);
        }
    }
}
inline void CEnemyInfo::Show(const std::tr1::shared_ptr<CPiiPersonalData>& rData, bool flash, int frames)
{
    const PiiSpeciesData* species = rData->GetSpeciesData();
    int firstType = species->GetPrimaryType();
    int secondType = species->GetSecondaryType();
    bool begin = mFrames == 0;
    bool endFlash = !flash && mFlash;
    mFlash = flash;
    mFrames = frames;
    mLayout.SetText("NAME", species->GetName());
    mLayout.SetText("TYPE0", GetMessageString(firstType + 0x40000, nullptr));
    mLayout.SetText("TYPE1", GetMessageString(secondType + 0x40000, nullptr));
    GetColorConstants()->ApplyVertexColor(mPanes[0], firstType + 11, 4);
    GetColorConstants()->ApplyVertexColor(mPanes[1], secondType + 11, 4);
    mLayout.SetVisible(mPanes[3], firstType != secondType);
    if (begin) {
        mLayout.CreateMotion("begin");
    }
    nw4r::lyt::Pane* first = mPanes[4];
    nw4r::lyt::Pane* second = mPanes[5];
    if (flash) {
        if (firstType == secondType) {
            if (first) {
                mLayout.CreateMotion("flash_begin", mPanes[4], 0.0f, 1.0f);
                mLayout.AppendMotion("flash_loop", first, 0.0f, 1.0f);
            }
            if (second) {
                mLayout.CreateMotion("flash_end", second, 1000.0f, 1.0f);
            }
        } else {
            if (second) {
                mLayout.CreateMotion("flash_begin", mPanes[5], 0.0f, 1.0f);
                mLayout.AppendMotion("flash_loop", second, 0.0f, 1.0f);
            }
            if (first) {
                mLayout.CreateMotion("flash_end", first, 1000.0f, 1.0f);
            }
        }
    }
    if (endFlash) {
        if (mPanes[4]) {
            mLayout.CreateMotion("flash_end", mPanes[4], 1000.0f, 1.0f);
        }
        if (mPanes[5]) {
            mLayout.CreateMotion("flash_end", mPanes[5], 1000.0f, 1.0f);
        }
    }
    mPiiTex.reset(new CPiiTex(rData, 256, 256));
    mLayout.SetTexture("PII", mPiiTex->GetTexMap(), -1);
    int speciesId = species->GetSpecies();
    bool collected = IGameSession::GetInstance()->GetCollectionType(speciesId) == 3;
    if (begin || collected != mCollected) {
        mCollected = collected;
        const char* motion = collected ? "pii_already" : "pii_not_yet";
        mLayout.CreateMotion("init", "PII", 0.0f, 1.0f);
        mLayout.QueueMotion(motion, "PII", 0.0f, 1.0f);
    }
}

inline void CEnemyInfo::Initialize(bool background)
{
    std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
    mLayout.Load(gSharedLayoutArchives, "enemy_info", fonts, background);
    mLayout.SetVisible(true);
    static const char* paneNames[] = {"COLOR0", "COLOR1", "TYPE0", "TYPE1", "FLASH1", "FLASH2"};
    for (int i = 0; i < 6; ++i) {
        mPanes[i] = mLayout.FindPane(paneNames[i]);
    }
    mLayout.CreateMotion("init");
    mFrames = 0;
    mFlash = false;
}

