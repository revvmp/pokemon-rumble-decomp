#include <revolution/gx.h>
#include <revolution/gx/GXTransform.h>
#include <revolution/sc.h>
#include <revolution/vi.h>
#include <revolution/mtx.h>
#include <nw4r/g3d/g3d_state.h>
#include <revolution/gx/GXVert.h>
#include <nw4r/ut/ut_Color.h>
#include <revolution/gx/GXTexture.h>
#include <nw4r/ut/ut_algorithm.h>
#include <game/HomeButton.h>
#include <game/RenderPrimitive.h>
#include <game/CScopedActivity.h>
#include <game/CFileData.h>
#include <game/Memory.h>
#include <memory>
#include <algorithm>
#include <revolution/tpl.h>
#include <game/Pad.h>
#include <game/Locale.h>
#include <nw4r/g3d/res/g3d_resfile.h>
#include <game/VideoTiming.h>
#include <nw4r/g3d/g3d_state.h>
#include <revolution/sc.h>
#include <stdio.h>
#include <string.h>

std::tr1::shared_ptr<CFileData> OpenFileData(const char* pPath, unsigned long flags);
void WaitFileData(CFileData* pFile, bool allowYield);
std::tr1::shared_ptr<unsigned char> DecompressLZ(const void* pData);
std::tr1::shared_ptr<unsigned char> DecompressHuffman(const void* pData);

namespace NResetMgr {
bool IsEnabled();
bool RequestMenu();
bool RequestReset(int mode);
bool RequestLaunch(const char* pName);
bool IsResetRequested();
bool IsPowerOffRequested();
bool ConsumeResetRequest();
}

namespace {


struct InitSoundData {
    std::tr1::shared_ptr<unsigned char> mArchive;
    std::tr1::shared_ptr<unsigned char> mWork;
};

struct InitHomeButtonInfoData {
    std::tr1::shared_ptr<unsigned char> mLayout;
    std::tr1::shared_ptr<unsigned char> mExtensionLayout;
    std::tr1::shared_ptr<unsigned char> mSpeaker;
    std::tr1::shared_ptr<CFileData> mMessage;
    std::tr1::shared_ptr<CFileData> mExtensionMessage;
    std::tr1::shared_ptr<CFileData> mConfig;
    std::tr1::shared_ptr<unsigned char> mWork;
    std::tr1::shared_ptr<CFileData> mModel;
};

KPADStatus sReadStatus[4];
KPADStatus sFilteredStatus[4];
std::tr1::shared_ptr<CHBMParam> spParam;
std::tr1::shared_ptr<CFileData> spIcon;
std::tr1::shared_ptr<InitSoundData> spSound;
std::tr1::shared_ptr<InitHomeButtonInfoData> spInfo;
HBMDataInfo sInfo;
HBMDataInfoEx sInfoEx;
HBMControllerData sController;
Vec2 sPointer[4];
int sActive;
int sWaitingToOpen;
signed char sOpenFadePhase;
unsigned long sOpenStartTick;
unsigned char sOpenFadeAlpha;
bool sInitialized;
int sDisableCount;
int sOpenBlocked;
std::tr1::shared_ptr<NResetMgr::CDisable> spResetGuard;

void InitializeHomeButtonInfo(HBMDataInfo* pInfo, HBMDataInfoEx* pInfoEx, bool allowYield);
void InitializeSoundData(bool allowYield);
}

namespace {
GXRenderModeObj sNormalModes[2] = {
    {VI_TVMODE_NTSC_INT, 608, 456, 456, 25, 12, 670, 456, VI_XFBMODE_DF, 0, GX_FALSE,
     {{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6}}, {8,8,10,12,10,8,8}},
    {VI_TVMODE_PAL_INT, 608, 456, 542, 25, 16, 670, 542, VI_XFBMODE_DF, 0, GX_FALSE,
     {{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6}}, {8,8,10,12,10,8,8}}
};
GXRenderModeObj sWideModes[2] = {
    {VI_TVMODE_NTSC_INT, 608, 456, 456, 17, 12, 686, 456, VI_XFBMODE_DF, 0, GX_FALSE,
     {{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6}}, {8,8,10,12,10,8,8}},
    {VI_TVMODE_PAL_INT, 608, 456, 542, 19, 16, 682, 542, VI_XFBMODE_DF, 0, GX_FALSE,
     {{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6},{6,6}}, {8,8,10,12,10,8,8}}
};
}


bool IsHomeButtonActive();

namespace {
int OnHomeButtonSound(int event, int sound) { return 0; }
}

namespace {
std::tr1::shared_ptr<CFileData> OpenHomeButtonFile(const char* pPath, unsigned long flags) {
    return OpenFileData(pPath, flags);
}
}

void SetHomeButtonProjection() {
    int wide = SCGetAspectRatio() == SC_ASPECT_RATIO_16x9;
    int pal = VIGetTvFormat() == VI_PAL;
    const GXRenderModeObj* pCurrentMode = nw4r::g3d::G3DState::GetRenderModeObj();
    const GXRenderModeObj* pHomeButtonMode;
    float width;
    float height;
    float widthScale = 1.0f;
    float heightScale = 1.0f;
    if (!wide) {
        pHomeButtonMode = &sNormalModes[pal];
        height = 456.0f;
        width = 608.0f;
    } else {
        pHomeButtonMode = &sWideModes[pal];
        height = 456.0f;
        width = 832.0f;
        widthScale = 0.95585084f;
    }
    width = widthScale * (width * pCurrentMode->viWidth / pHomeButtonMode->viWidth);
    height = heightScale * (height * pCurrentMode->viHeight / pHomeButtonMode->viHeight);
    Mtx44 projection;
    C_MTXOrtho(projection, height / 2.0f, -height / 2.0f, -width / 2.0f, width / 2.0f, 0.0f, 500.0f);
    GXSetProjection(projection, GX_ORTHOGRAPHIC);
}

namespace {
void DrawHomeButtonNotice(unsigned char alpha) {
    Mtx matrix;
    PSMTXIdentity(matrix);
    GXLoadPosMtxImm(matrix, GX_PNMTX1);
    GXSetCurrentMtx(GX_PNMTX1);
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT5, GX_VA_POS, GX_POS_XY, GX_S16, 0);
    GXSetVtxAttrFmt(GX_VTXFMT5, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0);
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    GXSetNumTevStages(1);
    GXColor color = {255, 255, 255, 0};
    color.a = alpha;
    GXSetTevColor(GX_TEVREG0, color);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
    GXTexObj texture;
    TPLGetGXTexObjFromPalette(static_cast<TPLPalette*>(spIcon->GetData()), &texture, 0);
    GXLoadTexObj(&texture, GX_TEXMAP0);
    GXBegin(GX_QUADS, GX_VTXFMT5, 4);
    GXPosition2s16(-256, 132);
    GXTexCoord2s16(0, 1);
    GXPosition2s16(-256, 188);
    GXTexCoord2s16(0, 0);
    GXPosition2s16(-200, 188);
    GXTexCoord2s16(1, 0);
    GXPosition2s16(-200, 132);
    GXTexCoord2s16(1, 1);
}
}

namespace {
void InitializeHomeButtonInfo(HBMDataInfo* pInfo, HBMDataInfoEx* pInfoEx, bool allowYield) {
    if (!spInfo) {
    spInfo.reset(new InitHomeButtonInfoData);
    InitHomeButtonInfoData* pData = spInfo.get();
    pInfo->region = GetConsoleLang();
    std::string layoutPath;
    std::string extensionPath;
    switch (pInfo->region) {
    case 0:
        layoutPath = spParam->GetResourceRoot() + "/HomeButton3/LZ77_homeBtn.arc";
        extensionPath = spParam->GetExtensionRoot() + "/HomeButton3/LZ77_homeBtn_nwm_jpn.arc";
        break;
    case 1:
        layoutPath = spParam->GetResourceRoot() + "/HomeButton3/LZ77_homeBtn_ENG.arc";
        extensionPath = spParam->GetExtensionRoot() + "/HomeButton3/LZ77_homeBtn_nwm_eng.arc";
        break;
    case 2:
        layoutPath = spParam->GetResourceRoot() + "/HomeButton3/LZ77_homeBtn_GER.arc";
        extensionPath = spParam->GetExtensionRoot() + "/HomeButton3/LZ77_homeBtn_nwm_ger.arc";
        break;
    case 3:
        layoutPath = spParam->GetResourceRoot() + "/HomeButton3/LZ77_homeBtn_FRA.arc";
        extensionPath = spParam->GetExtensionRoot() + "/HomeButton3/LZ77_homeBtn_nwm_fra.arc";
        break;
    case 4:
        layoutPath = spParam->GetResourceRoot() + "/HomeButton3/LZ77_homeBtn_SPA.arc";
        extensionPath = spParam->GetExtensionRoot() + "/HomeButton3/LZ77_homeBtn_nwm_spa.arc";
        break;
    case 5:
        layoutPath = spParam->GetResourceRoot() + "/HomeButton3/LZ77_homeBtn_ITA.arc";
        extensionPath = spParam->GetExtensionRoot() + "/HomeButton3/LZ77_homeBtn_nwm_ita.arc";
        break;
    case 6:
        layoutPath = spParam->GetResourceRoot() + "/HomeButton3/LZ77_homeBtn_NED.arc";
        extensionPath = spParam->GetExtensionRoot() + "/HomeButton3/LZ77_homeBtn_nwm_ned.arc";
        break;
    default:
        pInfo->region = 0;
        layoutPath = spParam->GetResourceRoot() + "/HomeButton3/LZ77_homeBtn.arc";
        extensionPath = spParam->GetExtensionRoot() + "/HomeButton3/LZ77_homeBtn_nwm_jpn.arc";
        break;
    }
    std::tr1::shared_ptr<CFileData> layout = OpenHomeButtonFile(layoutPath.c_str(), 0x4000);
    std::tr1::shared_ptr<CFileData> extension = OpenHomeButtonFile(extensionPath.c_str(), 0x4000);
    std::tr1::shared_ptr<CFileData> speaker = OpenHomeButtonFile((spParam->GetResourceRoot() + "/HomeButton3/Huf8_SpeakerSe.arc").c_str(), 0x4000);
    std::tr1::shared_ptr<CFileData> message;
    if (spParam->HasFlags(CHBMParam::NoSaveWiiMenu | CHBMParam::NoSaveReset | CHBMParam::NoSaveAll))
        message = OpenHomeButtonFile(spParam->GetNoSaveMessagePath(), 0);
    else
        message = OpenHomeButtonFile(spParam->GetMessagePath(), 0);
    std::tr1::shared_ptr<CFileData> networkMessage = OpenHomeButtonFile(spParam->GetNetworkMessagePath(), 0);
    std::tr1::shared_ptr<CFileData> config = OpenHomeButtonFile((spParam->GetResourceRoot() + "/HomeButton3/config.txt").c_str(), 0);
    WaitFileData(layout.operator->(), allowYield);
    pData->mLayout = DecompressLZ(layout->GetData());
    pInfo->layoutBuf = pData->mLayout.get();
    WaitFileData(extension.operator->(), allowYield);
    pData->mExtensionLayout = DecompressLZ(extension->GetData());
    pInfoEx->layoutBuf = pData->mExtensionLayout.get();
    WaitFileData(speaker.operator->(), allowYield);
    pData->mSpeaker = DecompressHuffman(speaker->GetData());
    pInfo->spkSeBuf = pData->mSpeaker.get();
    WaitFileData(message.operator->(), allowYield);
    pData->mMessage = message;
    pInfo->msgBuf = pData->mMessage->GetData();
    WaitFileData(networkMessage.operator->(), allowYield);
    pData->mExtensionMessage = networkMessage;
    pInfoEx->msgBuf = pData->mExtensionMessage->GetData();
    WaitFileData(config.operator->(), allowYield);
    pData->mConfig = config;
    pInfo->configBuf = pData->mConfig->GetData();
    pInfo->configBufSize = pData->mConfig->GetSize();
    pInfo->sound_callback = OnHomeButtonSound;
    pInfo->backFlag = 0;
    pInfo->cursor = 0;
    pInfo->adjust.x = 1.0f;
    pInfo->adjust.y = 1.0f;
    pInfo->frameDelta = spParam->GetFrameRate() / 60.0f;
    pData->mWork.reset(static_cast<unsigned char*>(GameMemory::AllocateMEM2(0x80000, 32)), GameMemory::Free);
    pInfo->mem = pData->mWork.get();
    pInfo->memSize = 0x80000;
    pInfo->pAllocator = nullptr;
    pInfo->messageFlag = 0;
    if (spParam->HasFlags(CHBMParam::NoSaveWiiMenu))
        pInfo->messageFlag |= HBMMSG_NOSAVE_WIIMENU;
    if (spParam->HasFlags(CHBMParam::NoSaveReset))
        pInfo->messageFlag |= HBMMSG_NOSAVE_RESET;
    if (spParam->HasFlags(CHBMParam::NoSaveAll))
        pInfo->messageFlag = HBMMSG_NOSAVE_ALL;
    pData->mModel = OpenHomeButtonFile(spParam->GetModelPath(), 0);
    WaitFileData(pData->mModel.operator->(), false);
    nw4r::g3d::ResFile model(pData->mModel->GetData());
    nw4r::g3d::ResTex texture(model.GetResTex(spParam->GetTextureName()));
    float minLod;
    float maxLod;
    GXBool mipMap;
    texture.GetTexObjParam(&pInfoEx->texImage, &pInfoEx->texImageWidth, &pInfoEx->texImageHeight, &pInfoEx->texImageFormat, &minLod, &maxLod, &mipMap);
    }
}
}

namespace {
#pragma auto_inline off
void InitializeSoundData(bool allowYield) {
    if (!spSound) {
        spSound.reset(new InitSoundData);
        InitSoundData* pSound = spSound.get();
        std::tr1::shared_ptr<CFileData> file = OpenHomeButtonFile((spParam->GetSoundRoot() + "/HomeButtonSe/Huf8_HomeButtonSe.arc").c_str(), 0x4000);
        WaitFileData(file.operator->(), allowYield);
        pSound->mArchive = DecompressHuffman(file->GetData());
        pSound->mWork.reset(static_cast<unsigned char*>(GameMemory::AllocateMEM2(100096, 32)), GameMemory::Free);
    }
}
#pragma auto_inline reset
}

namespace {
bool MoveClassicCursor(unsigned long buttons, Vec2* pPosition) {
    const float step = 9.0f / 256.0f;
    const float diagonalStep = step * 0.7071f;
    switch (buttons & (WPAD_CL_BUTTON_UP | WPAD_CL_BUTTON_LEFT | WPAD_CL_BUTTON_DOWN | WPAD_CL_BUTTON_RIGHT)) {
    case WPAD_CL_BUTTON_UP:
        pPosition->y -= step;
        break;
    case WPAD_CL_BUTTON_LEFT:
        pPosition->x -= step;
        break;
    case WPAD_CL_BUTTON_DOWN:
        pPosition->y += step;
        break;
    case WPAD_CL_BUTTON_RIGHT:
        pPosition->x += step;
        break;
    case WPAD_CL_BUTTON_UP | WPAD_CL_BUTTON_LEFT:
        pPosition->y -= diagonalStep;
        pPosition->x -= diagonalStep;
        break;
    case WPAD_CL_BUTTON_UP | WPAD_CL_BUTTON_RIGHT:
        pPosition->y -= diagonalStep;
        pPosition->x += diagonalStep;
        break;
    case WPAD_CL_BUTTON_DOWN | WPAD_CL_BUTTON_LEFT:
        pPosition->y += diagonalStep;
        pPosition->x -= diagonalStep;
        break;
    case WPAD_CL_BUTTON_DOWN | WPAD_CL_BUTTON_RIGHT:
        pPosition->y += diagonalStep;
        pPosition->x += diagonalStep;
        break;
    default:
        return false;
    }
    pPosition->x = nw4r::ut::Clamp(pPosition->x, -1.0f, 1.0f);
    pPosition->y = nw4r::ut::Clamp(pPosition->y, -1.0f, 1.0f);
    return true;
}
}

CHBMParam::CHBMParam() : mFlags(0), mFrameRate(60.0f) {}

CHBMParam::~CHBMParam() {}

bool InitializeHomeButton(const CHBMParam& rParam, bool allowYield) {
    if (sInitialized) {
        return true;
    }
    spParam.reset(new CHBMParam(rParam));
    spIcon = OpenHomeButtonFile((spParam->GetResourceRoot() + "/HomeButton3/homeBtnIcon.tpl").c_str(), 0);
    WaitFileData(spIcon.operator->(), allowYield);
    TPLBind(static_cast<TPLPalette*>(spIcon->GetData()));
    if (spParam->HasFlags(0x10)) {
        InitializeHomeButtonInfo(&sInfo, &sInfoEx, allowYield);
        InitializeSoundData(allowYield);
    }
    sInitialized = true;
    return true;
}

void FinalizeHomeButton() {
    if (sInitialized) {
        if (IsHomeButtonActive()) {
            HBMDelete();
            HBMDeleteSound();
            sActive = 0;
            spResetGuard.reset();
        }
        spSound.reset();
        spInfo.reset();
        spIcon.reset();
        spParam.reset();
        sInitialized = false;
    }
}

void UpdateHomeButton() {
    if (!sInitialized)
        return;
    int errors[4];
    for (int i = 0; i < 4; ++i) {
        HBMKPadData& rCon = sController.wiiCon[i];
        KPADStatus& rRead = sReadStatus[i];
        KPADStatus& rFiltered = sFilteredStatus[i];
        unsigned int deviceType;
        errors[i] = GetWiiPadDevice(i, &deviceType);
        rCon.use_devtype = deviceType;
        rRead = *static_cast<KPADStatus*>(GetWiiPad(i));
        rFiltered = rRead;
        if (spParam->HasFlags(8) && rCon.use_devtype == WPAD_DEV_CLASSIC)
            rCon.use_devtype = 0;
        if (errors[i] != 0 && errors[i] != -2 && errors[i] != -3 && errors[i] != -4 && errors[i] != -7) {
            rCon.kpad = nullptr;
            memset(&rFiltered, 0, sizeof(KPADStatus));
        } else {
            rCon.kpad = &rFiltered;
            bool padMoved = MoveClassicCursor(rFiltered.ex_status.cl.hold, &rCon.pos);
            float dx = nw4r::ut::Clamp(rCon.kpad->ex_status.cl.lstick.x / (256.0f / 9.0f), -1.0f, 1.0f);
            float dy = nw4r::ut::Clamp(rCon.kpad->ex_status.cl.lstick.y / (256.0f / 9.0f), -1.0f, 1.0f);
            bool stickMoved;
            if (dx == 0.0f && dy == 0.0f) {
                stickMoved = false;
            } else {
                rCon.pos.x = nw4r::ut::Clamp(rCon.pos.x + dx, -1.0f, 1.0f);
                rCon.pos.y = nw4r::ut::Clamp(rCon.pos.y - dy, -1.0f, 1.0f);
                stickMoved = true;
            }
            if (!(padMoved | stickMoved) && rCon.kpad->dpd_valid_fg > 0)
                rCon.pos = rCon.kpad->pos;
        }
    }
    if (!sActive && !sWaitingToOpen) {
        bool opened = false;
        for (int i = 0; i < 4; ++i) {
            if (errors[i] == 0 && (sFilteredStatus[i].trig == WPAD_BUTTON_HOME ||
                (!spParam->HasFlags(8) && sFilteredStatus[i].ex_status.cl.trig == WPAD_CL_BUTTON_HOME))) {
                bool canOpen = false;
                if (!sDisableCount)
                    canOpen = NResetMgr::IsEnabled();
                if (canOpen) {
                    opened = true;
                    sActive = 1;
                    spResetGuard.reset(new NResetMgr::CDisable);
                    if (spParam->GetOpenCallback())
                        spParam->GetOpenCallback()();
                } else if (!sOpenBlocked) {
                    sWaitingToOpen = 1;
                }
                break;
            }
        }
        if (sWaitingToOpen) {
            sOpenFadePhase = 0;
            sWaitingToOpen = 1;
            sOpenStartTick = OSGetTick();
            sOpenFadeAlpha = 0;
        }
        if (opened) {
            for (int i = 0; i < 4; ++i) {
                sController.wiiCon[i].pos.x = 0.0f;
                sController.wiiCon[i].pos.y = 0.0f;
                sController.wiiCon[i].use_devtype = 0;
            }
            InitializeHomeButtonInfo(&sInfo, &sInfoEx, false);
            sInfo.adjust.x = SCGetAspectRatio() == SC_ASPECT_RATIO_16x9 ? 832.0f / 608.0f : 1.0f;
            sInfo.adjust.y = 1.0f;
            HBMCreate(&sInfo);
            HBMCreateInfoEx(&sInfoEx);
            HBMInit();
            HBMSetAdjustFlag(TRUE);
            InitializeSoundData(false);
            HBMCreateSound(spSound->mArchive.get(), spSound->mWork.get(), 100096);
            gFrameTimingReset = true;
            for (int i = 0; i < 4; ++i)
                WPADControlSpeaker(i, WPAD_SPEAKER_ON, nullptr);
        }
    }
    if (sActive) {
        HBMUpdateSound();
        if (HBMCalc(&sController) >= 0) {
            OSReport("Select Btn:%d\n", HBMGetSelectBtnNum());
            OSReport("Reassigned:%d\n", HBMIsReassignedControllers());
            HBMSelectBtnNum selected = HBMGetSelectBtnNum();
            HBMDelete();
            HBMDeleteSound();
            if (!spParam->HasFlags(CHBMParam::PreloadResources)) {
                spSound.reset();
                spInfo.reset();
            }
            sActive = 0;
            spResetGuard.reset();
            if (selected == HBM_SELECT_HOMEBTN) {
                for (int i = 0; i < 4; ++i) {
                    WPADControlSpeaker(i, WPAD_SPEAKER_ON, nullptr);
                    WPADControlSpeaker(i, WPAD_SPEAKER_PLAY, nullptr);
                }
            }
            bool handled = false;
            if (spParam->GetCloseCallback())
                handled = spParam->GetCloseCallback()(selected);
            if (!handled) {
                switch (selected) {
                case 1:
                    OSReport("Return to WiiMenu.\n");
                    NResetMgr::RequestMenu();
                    break;
                case 2:
                    OSReport("Reset.\n");
                    NResetMgr::RequestReset(0);
                    break;
                case 4:
                    OSReport("Jump to NetworkManual.\n");
                    NResetMgr::RequestLaunch("lmv");
                    break;
                }
            }
        }
    }
    const GXRenderModeObj* pMode = nw4r::g3d::G3DState::GetRenderModeObj();
    for (int i = 0; i < 4; ++i) {
        if (errors[i] == 0 && sController.wiiCon[i].kpad && !sActive) {
            if (sReadStatus[i].dev_type == WPAD_DEV_CLASSIC)
                sPointer[i] = sController.wiiCon[i].pos;
            else
                sPointer[i] = sController.wiiCon[i].kpad->pos;
            sPointer[i].x *= 0.5f * pMode->fbWidth;
            sPointer[i].y *= 0.5f * pMode->xfbHeight;
            sPointer[i].x *= sInfo.adjust.x;
            sPointer[i].y *= sInfo.adjust.y;
        }
    }
    if (IsHomeButtonActive() && NResetMgr::IsResetRequested()) {
        NResetMgr::ConsumeResetRequest();
        HBMStartBlackOut();
    }
    if (IsHomeButtonActive() && NResetMgr::IsPowerOffRequested())
        HBMStartBlackOut();
}

void DrawHomeButton() {
    if (!sInitialized)
        return;
    Mtx matrix;
    PSMTXIdentity(matrix);
    GXLoadPosMtxImm(matrix, GX_PNMTX1);
    SetHomeButtonProjection();
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT4, GX_VA_POS, GX_POS_XY, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT4, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0);
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetNumChans(1);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXSetBlendMode(GX_BM_NONE, GX_BL_ZERO, GX_BL_ZERO, GX_LO_CLEAR);
    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXSetCurrentMtx(GX_PNMTX1);
    if (sActive) {
        HBMDraw();
        const GXRenderModeObj* pMode = nw4r::g3d::G3DState::GetRenderModeObj();
        float width = pMode->fbWidth;
        float height = pMode->efbHeight;
        SetupOverlayRendering();
        SetupScreenProjection();
        nw4r::ut::Color color(0, 0, 0, 255);
        DrawScreenQuad(0.0f, 0.0f, width, 11.0f, -0.1f, color);
        DrawScreenQuad(0.0f, height - 5.0f, width, 5.0f, -0.1f, color);
    }
    if (!sActive && sWaitingToOpen) {
        float elapsed = OSTicksToMilliseconds(OSGetTick() - sOpenStartTick);
        switch (sOpenFadePhase) {
        case 0:
            sOpenFadeAlpha = 255.9f * (elapsed / 250.0f);
            if (elapsed >= 250.0f) {
                sOpenStartTick = OSGetTick();
                sOpenFadePhase = 1;
                sOpenFadeAlpha = 255;
            }
            break;
        case 1:
            if (elapsed >= 1000.0f) {
                sOpenStartTick = OSGetTick();
                sOpenFadePhase = 2;
            }
            break;
        case 2:
            sOpenFadeAlpha = 255.9f * ((250.0f - elapsed) / 250.0f);
            if (elapsed >= 250.0f) {
                sOpenFadeAlpha = 0;
                sWaitingToOpen = 0;
            }
            break;
        }
        DrawHomeButtonNotice(sOpenFadeAlpha);
    }
    nw4r::g3d::G3DState::Invalidate(0x7FF);
}

bool IsHomeButtonActive() { return sInitialized && sActive != 0; }

CDisableHBM::CDisableHBM() { ++sDisableCount; }

CDisableHBM::~CDisableHBM() {
    sDisableCount = std::max(0, --sDisableCount);
    if (sDisableCount == 0) {
        NResetMgr::IsEnabled();
    }
}
