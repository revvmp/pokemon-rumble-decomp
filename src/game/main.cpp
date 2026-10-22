#include <game/CArchiveFontWithBuf.h>
#include <game/CPpuEngine.h>
#include <game/NResetMgr.h>
#include <game/CPpuEngine.h>
#include <game/NResetMgr.h>
#include <game/CLayout.h>
#include <game/FileList.h>
#include <game/ITitleSession.h>
#include <game/IGameSession.h>
#include <game/IColorConst.h>
#include <game/CIconCollection.h>
#include <game/CStrapReminder.h>
#include <game/HomeButton.h>
#include <stdio.h>
void FinalizeHomeButton();


#include <game/ISession.h>
#include <game/CFiber.h>
#include <game/IDisconMsg.h>
#include <game/ISaveManager.h>
#include <game/IStrapReminder.h>
#include <game/IGameMap.h>
#include <game/IPauseMenu.h>
#include <game/Graphics.h>
#include <game/Pad.h>
#include <game/ControllerTable.h>
#include <game/Sound.h>
#include <revolution/gx.h>

extern "C" void GXInvalidateTexAll();

void DrawHomeButton();
bool IsHomeButtonActive();
void UpdateResourceFiles();
void UpdateHomeButton();


#ifndef GAME_CMAIN_H
#define GAME_CMAIN_H

#include <game/GameLoop.h>
#include <memory>

class CFiber;
class ISession;
class IDisconMsg;
class ISaveManager;
class IStrapReminder;

namespace {

class CMain : public IGameLoopCallback {
public:
    CMain();
    ~CMain();
    virtual bool IsRunning() { return true; }
    virtual void ResetTiming() { SetPadReadDelay(1); }
    virtual void Update() {
    UpdateResourceFiles();
    UpdatePad();
    UpdateControllers();
    UpdateSound();
    UpdateHomeButton();
    if (!IsHomeButtonActive()) {
        if (!mDisconMsg || (mDisconMsg->Calc(), !mDisconMsg->IsActive())) {
            if (IGameMap* pMap = GetActiveGameMap()) {
                if (IPauseMenu* pMenu = pMap->GetPauseMenu()) {
                    pMenu->Calc();
                    pMenu->IsOpen();
                }
            }
        }
    }
    if (ISaveManager* pSave = ISaveManager::GetInstance()) pSave->Calc();
    NResetMgr::Update();
}
    virtual bool ShouldRender() {
    if (mDisconMsg) {
        if (mDisconMsg->IsActive()) return false;
    }
    if (IGameMap* pMap = GetActiveGameMap()) {
        if (IPauseMenu* pMenu = pMap->GetPauseMenu()) {
            if (pMenu->IsOpen()) return false;
        }
    }
    return !IsHomeButtonActive();
}
    virtual void Prepare() {
    mFiber->Resume();
    if (mSession) mSession->Calc();
    if (mStrapReminder) mStrapReminder->Calc();
}
    virtual void Calc() {
    if (mSession) mSession->CalcMtx();
    if (mDisconMsg) mDisconMsg->CalcMtx();
    if (mStrapReminder) mStrapReminder->CalcMtx();
}
    virtual void Draw() {
    if (mSession) mSession->CalcView();
    if (mDisconMsg) mDisconMsg->CalcView();
    if (mStrapReminder) mStrapReminder->CalcView();
}
    virtual void Render() {
    GXSetDither(GX_TRUE);
    Graphics::ResetState();
    Graphics::SetPixelFormat(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
    GXInvalidateVtxCache();
    GXInvalidateTexAll();
    if (mSession) mSession->Draw();
    if (mStrapReminder) mStrapReminder->Draw();
    if (IGameMap* pMap = GetActiveGameMap()) {
        if (IPauseMenu* pMenu = pMap->GetPauseMenu()) pMenu->Draw();
    }
    if (mDisconMsg) mDisconMsg->Draw();
    if (ISaveManager* pSave = ISaveManager::GetInstance()) pSave->Draw();
    DrawHomeButton();
    Graphics::CopyFrameBuffer();
}
    virtual void Present() { Graphics::DisplayFrameBuffer(); }
    void Run();
    void RunSession();
    std::tr1::shared_ptr<FileList> LoadStartupArchives();
    std::tr1::shared_ptr<CLayout::FontMap> LoadFonts();

private:
    std::tr1::shared_ptr<ISession> mSession;
    std::tr1::shared_ptr<CFiber> mFiber;
    std::tr1::shared_ptr<IDisconMsg> mDisconMsg;
    std::tr1::shared_ptr<ISaveManager> mSaveManager;
    std::tr1::shared_ptr<IStrapReminder> mStrapReminder;
};

}

#endif


#include <revolution/kpad.h>
#include <revolution/wpad.h>
#include <nw4r/snd/snd_SoundSystem.h>
#include <game/Sound.h>
#include <game/Message.h>
#include <game/CLayout.h>
#include <revolution/hbm/HBMBase.h>

void PauseGameAudio();
void ResumeGameAudio();
void ClearGameAudioEffects();
void RestoreGameAudioEffects();

void OnHomeMenuOpen() {
    for (int channel = 0; channel < 4; ++channel) {
        KPADEnableDPD(channel);
    }
    PauseGameAudio();
    ClearGameAudioEffects();
    nw4r::snd::SoundSystem::SetMasterVolume(0.0f, 0);
}

bool OnHomeMenuClose(HBMSelectBtnNum selection) {
    for (int channel = 0; channel < 4; ++channel) {
        KPADDisableDPD(channel);
    }
    ResumeGameAudio();
    RestoreGameAudioEffects();
    nw4r::snd::SoundSystem::SetMasterVolume(1.0f, 0);
    return false;
}

void PrepareSystemMessageAudio() {
    for (int channel = 0; channel < 4; ++channel) {
        WPADControlSpeaker(channel, 1, nullptr);
    }
    nw4r::snd::SoundSystem::ClearEffect(nw4r::snd::AUX_A, 250);
    nw4r::snd::SoundSystem::ClearEffect(nw4r::snd::AUX_B, 250);
    nw4r::snd::SoundSystem::ClearEffect(nw4r::snd::AUX_C, 250);
    nw4r::snd::SoundSystem::SetMasterVolume(0.0f, 300);
}

void UpdateSystemMessageAudio() {
    UpdateSound();
}

void ShowLayoutFailureMessage(const CLayout::CInitializeException& rError) {
    ShowSystemMessage(GetMessageString(5, nullptr), true);
}

#include <game/MainRuntime.h>
#include <game/Message.h>
#include <game/SystemMessage.h>
#include <revolution/os.h>

bool UsesAlternateErrorMessage(CPath::Drive drive);

void HandleFileError(CPath::Drive drive, const char* pPath, long error, unsigned long flags) {
    if ((flags & 0x8000) && error == -6007) {
        return;
    }
    switch (drive) {
    case CPath::DEVICE_NAND:

        switch (error) {
        case -2:
            ShowSystemMessage(GetMessageString(65567, nullptr), true);
            break;
        case -3:
            ShowSystemMessage(GetMessageString(65567, nullptr), true);
            break;
        case -4:
            ShowSystemMessage(GetMessageString(65565, nullptr), false);
            break;
        case -9:
            ShowSystemMessage(GetMessageString(5, nullptr), true);
            break;
        case -11:
            ShowSystemMessage(GetMessageString(5, nullptr), true);
            break;
        case -64:
            ShowSystemMessage(GetMessageString(65568, nullptr), false);
            break;
        }
        if (!(flags & 0x2000)) {
            ShowSystemMessage(GetMessageString(5, nullptr), true);
        }
        break;
    case 2 ... 511:

        switch (error) {
        case -5013:
            ShowSystemMessage(GetMessageString(5 + !!UsesAlternateErrorMessage(drive), nullptr), true);
            break;
        case -5011:
            ShowSystemMessage(GetMessageString(65565, nullptr), false);
            break;
        case -5012:
            ShowSystemMessage(GetMessageString(5 + !!UsesAlternateErrorMessage(drive), nullptr), true);
            break;
        case -5063:
            ShowSystemMessage(GetMessageString(65568, nullptr), false);
            break;
        }
        if (!(flags & 0x2000)) {
            if (UsesAlternateErrorMessage(drive)) {
                ShowSystemMessage(GetMessageString(6, nullptr), true);
            } else {
                ShowSystemMessage(GetMessageString(5, nullptr), true);
            }
        }
        break;
    case 512 ... 537:
    case CPath::DEVICE_UNC:

        if (!(flags & 0x2000)) {
            OSPanic("main.cpp", 496, "fatal error\n");
        }
        break;
    default:
        break;
    }
}
inline bool UsesAlternateErrorMessage(CPath::Drive drive) {
    if (drive < 2) {
        return false;
    }
    if (drive >= 512) {
        return false;
    }
    switch (drive) {
    case 5:
    case 6:
        return false;
    default:
        return true;
    }
}

#include <game/MainRuntime.h>
#include <game/Message.h>

const MainMessageFile gMainMessageFiles[9] = {
    {"message_list.bmg", false},
    {"localedef.bmg", false},
    {"pokemon.bmg", false},
    {"pokemon_type.bmg", false},
    {"zkn_type.bmg", false},
    {"main.bmg", true},
    {"script.bmg", true},
    {"wazaname.bmg", false},
    {"wazadesc.bmg", false}
};

extern const DateMessageIDs gMainDateMessageIDs;
const DateMessageIDs gMainDateMessageIDs = {
    131084, 131072, 131124, 131125, 131122, 131123,
    131128, 131103, 131110, 131096, 131126
};

extern const TimeMessageIDs gMainTimeMessageIDs;
const TimeMessageIDs gMainTimeMessageIDs = {
    131137, 131138, 131130, 131131,
    131134, 131132, 131133, 131136
};

extern const char* gMainFontPaths[6];
const char* gMainFontPaths[6] = {
    "5:/font.arc/fot_extensions.brfnt",
    "5:/font.arc/fot_newrodin_pro_eb16.brfnt",
    "5:/font.arc/fot_newrodin_pro_eb.brfnt",
    "5:/font.arc/fot_rocknroll_std.brfnt",
    "5:/font.arc/fot_wanpaku_b.brfnt",
    "5:/font.arc/fot_wanpaku_b_hd.brfnt"
};

#include <game/Locale.h>
#include <revolution/sc.h>
extern const DateMessageIDs gMainDateMessageIDs;
extern const TimeMessageIDs gMainTimeMessageIDs;
extern unsigned char gEuropeanEnglishSystemMessages[];
extern void* const gSystemMessageResources[6];
void InitializeMainMessages() {
    InitMsgResList(32);
    SetDateMessageIDs(gMainDateMessageIDs);
    SetTimeMessageIDs(gMainTimeMessageIDs);
    void* pResource;
    if (GetGameRegion() == REGION_EUR && GetGameLang() == SC_LANG_ENGLISH) {
        pResource = gEuropeanEnglishSystemMessages;
    } else {
        pResource = gSystemMessageResources[GetGameLang()];
    }
    std::tr1::shared_ptr<CMsgRes> messages(new CMsgRes(pResource, true));
    SetMsgRes(messages->GetGroupID(), messages);
}

#include <game/Memory.h>
#include <game/Random.h>
#include <game/Pad.h>
#include <game/ControllerTable.h>
#include <game/Graphics.h>
#include <game/Locale.h>
#include <game/WorkBuffer.h>
#include <nw4r/lyt/lyt_init.h>
#include <nw4r/lyt/lyt_layout.h>
#include <revolution/cnt.h>
#include <revolution/dvd.h>
#include <revolution/fa.h>
#include <revolution/sc.h>
#include <game/MainRuntime.h>


void SetTextLocale(int locale);
void InitializeEffect(unsigned long count, unsigned long emitters, unsigned long groups, unsigned long particles, unsigned long flags);
void FinalizeEffect();

inline CMain::CMain() {
    mFiber.reset(new CFiber(nullptr, std::tr1::bind(&CMain::Run, this), 0x8000));
}

void main() {
    CNTInit();
    DVDInit();
    GameMemory::Initialize();
    Graphics::Initialize(nullptr);
    nw4r::lyt::LytInit();
    nw4r::lyt::Layout::SetAllocator(GetDefaultAllocator());
    SetLayoutInitializeExceptionHandler(ShowLayoutFailureMessage);
    InitRandom();
    InitializePad(GetPadSampleInterval(), 4);
    for (int channel = 0; channel < 4; ++channel) {
        KPADDisableDPD(channel);
    }
    InitControllers();
    switch (GetGameLang()) {
    case SC_LANG_ENGLISH:
    case SC_LANG_GERMAN:
    case SC_LANG_FRENCH:
    case SC_LANG_SPANISH:
    case SC_LANG_ITALIAN:
    case SC_LANG_DUTCH:
        SetTextLocale(0);
        break;
    case SC_LANG_JAPANESE:
        SetTextLocale(1);
        break;
    case SC_LANG_KOREAN:
        SetTextLocale(3);
        break;
    case SC_LANG_TRAD_CHINESE:
        SetTextLocale(4);
        break;
    case SC_LANG_SIMP_CHINESE:
        SetTextLocale(5);
        break;
    }
    SetExtensionControllerEnabled(GetGameRegion() != REGION_USA);
    InitializeMainMessages();
    NResetMgr::Initialize();
    RegisterResetCallback(reinterpret_cast<void (*)()>(CNTShutdown), -33);
    {
        ResourceFileConfig config;
        CPath path;
        path.ParseDevice(CPpuEngine::GetScriptRoot());
        config.SetErrorHandler(HandleFileError);
        config.mUseDriveWorker = (path.GetDevice() >= CPath::DEVICE_MCS_BASE && path.GetDevice() <= CPath::DEVICE_MCS_LAST) || path.GetDevice() == CPath::DEVICE_UNC;
        InitializeResourceFiles(config);
    }
    FAInit(0x20000L);
    InitializeEffect(500, 1000, 1000, 10000, 32);
    InitSound(132288, 0, 192128);
    InitializeWorkBuffer();
    InitializeSystemMessages(gSystemMessageArchive, "fatal_errmsg", "fot_wanpaku_b.brfnt", 10512, 256);
    RegisterSystemMessageCallback(PrepareSystemMessageAudio, 2);
    RegisterSystemMessageCallback(UpdateSystemMessageAudio, 1);
    InitializeCalculationWorker();
    InitLocalizedFiles();
    {
        CMain application;
        RunGameLoop(&application);
    }
    ResetLocalizedFiles();
    ShutdownCalculationWorker();
    ShutdownWorkBuffer();
    ShutdownSound();
    FinalizeEffect();
    ShutdownResourceFiles();
    ShutdownControllers();
    ShutdownRandom();
}

void ResourceFileConfig::SetErrorHandler(const FileErrorHandler& rHandler) {
    FileErrorHandler handler(rHandler);
    mErrorHandler.swap(handler);
}

#include <game/HomeButton.h>
#include <game/CStrapReminder.h>
#include <game/ISaveManager.h>
#include <game/IDisconMsg.h>
#include <game/IColorConst.h>
#include <game/CIconCollection.h>
#include <game/Locale.h>
#include <game/Pad.h>
#include <game/FileList.h>
#include <game/Message.h>
#include <game/Sound.h>
#include <nw4r/ef/ef_resource.h>
#include <stdio.h>
#include <game/MainRuntime.h>

void CMain::Run() {
    std::tr1::shared_ptr<CDisablePadRumble> rumble(new CDisablePadRumble);
    YieldFiberStarted();
    mStrapReminder = CStrapReminder::Create(true);
    const char* pHomeLocale;
    switch (GetGameRegion()) {
    case REGION_JPN:
        pHomeLocale = "ja_JP";
        break;
    case REGION_USA:
        pHomeLocale = "en_US";
        break;
    case REGION_EUR:
        pHomeLocale = GetConsoleLocale();
        break;
    }
    char homeModel[64];
    snprintf(homeModel, sizeof(homeModel), "5:/%s/hbm.arc/man.brres", pHomeLocale);
    {
        CHBMParam home;
        home.SetResourceRoot("2:");
        home.SetSoundRoot("3:");
        home.SetExtensionRoot("4:");
        home.SetNoSaveMessagePath("5:/hbm.arc/home_nosave.csv");
        home.SetMessagePath("5:/hbm.arc/home.csv");
        home.SetNetworkMessagePath("5:/hbm.arc/network.csv");
        home.SetModelPath(homeModel);
        home.SetTextureName("man");
        home.SetFlags(CHBMParam::NoSaveAll | CHBMParam::PreloadResources);
        home.SetOpenCallback(OnHomeMenuOpen);
        home.SetCloseCallback(OnHomeMenuClose);
        InitializeHomeButton(home, true);
    }
    std::tr1::shared_ptr<CDisableHBM> homeButton(new CDisableHBM);
    std::tr1::shared_ptr<FileList> archives = LoadStartupArchives();
    {
        std::tr1::shared_ptr<FileList> game = LoadFileList(nullptr, "game2d.arc", 0, true, nullptr);
        archives->insert(archives->end(), game->begin(), game->end());
        std::tr1::shared_ptr<FileList> items = LoadFileList(nullptr, "item.arc", 0, true, nullptr);
        archives->insert(archives->end(), items->begin(), items->end());
        std::tr1::shared_ptr<FileList> scripts = LoadFileList(nullptr, "script.arc", 0, true, nullptr);
        archives->insert(archives->end(), scripts->begin(), scripts->end());
    }
    for (int i = 0; i < 9; ++i) {
        char path[64];
        snprintf(path, sizeof(path), "5:/%s/msg.arc/%s", GetGameLocale(), gMainMessageFiles[i].mpPath);
        bool convertEscape = gMainMessageFiles[i].mConvertEscape;
        std::tr1::shared_ptr<CFileData> file = LoadFile(path, 0, true);
        AppendLoadedFile(*archives.operator->(), file);
        std::tr1::shared_ptr<CMsgRes> messages(new CMsgRes(file->GetData(), convertEscape));
        SetMsgRes(messages->GetGroupID(), messages);
    }
    std::tr1::shared_ptr<CLayout::FontMap> fonts = LoadFonts();
    SetDefaultFontMap(fonts);
    SetSystemMessageFonts(fonts);
    InitializeColorConstants(true);
    InitIconCollection(true);
    nw4r::ef::Resource* pEffects = nw4r::ef::Resource::GetInstance();
    {
        std::tr1::shared_ptr<CFileData> file;
        file = LoadFile("5:/effect/main.breff", 0, true);
        pEffects->Add(static_cast<u8*>(file->GetData()));
        file = LoadFile("5:/effect/main.breft", 0, true);
        pEffects->AddTexture(static_cast<u8*>(file->GetData()));
        pEffects->RelocateCommand();
    }
    LoadSoundArchive("5:/sound/waza_p.brsar");
    mStrapReminder->Finish();
    mStrapReminder.reset();
    homeButton.reset();
    mSaveManager = ISaveManager::Create();
    mSaveManager->LoadSync();
    mSaveManager.reset();
    rumble.reset();
    mDisconMsg = CreateDisconMsg(true);
    for (;;) {
        RunSession();
    }
}


#include <game/FileList.h>

struct FileLoadParam {
    const char* mpPath;
    u32 mFlags;
};

std::tr1::shared_ptr<FileList> LoadFiles(const FileLoadParam* pFiles, unsigned int count, int background);
inline std::tr1::shared_ptr<FileList> LoadLocalizedFiles(const FileLoadParam* pFiles, unsigned int count, int background) {
    return LoadFiles(pFiles, count, background);
}

std::tr1::shared_ptr<FileList> CMain::LoadStartupArchives() {
    FileLoadParam requests[] = {
        {"5:/pii_1-151_387-492.arc", 0},
        {"5:/item.arc", 0},
        {"5:/game2d.arc", 0},
        {"5:/font.arc", 0},
        {"5:/effect/main.breff", 0},
        {"5:/effect/main.breft", 0},
        {"5:/sound/waza_p.brsar", 0}
    };
    return LoadLocalizedFiles(requests, 7, true);
}

void AppendLoadedFile(FileList& rFiles, const std::tr1::shared_ptr<CFileData>& rFile) {
    rFiles.insert(rFiles.end(), rFile);
}

#include <game/CLayout.h>
#include <game/CResFont.h>
#include <game/CArchiveFontWithBuf.h>
#include <game/Message.h>
#include <game/SystemMessage.h>
#include <nw4r/ut/ut_PairFont.h>
#include <string.h>
extern const char* gMainFontPaths[6];
std::tr1::shared_ptr<CLayout::FontMap> CMain::LoadFonts() {
    std::tr1::shared_ptr<CLayout::FontMap> fonts(new CLayout::FontMap);
    std::tr1::shared_ptr<nw4r::ut::Font> extension;
    for (int i = 0; i < 6; ++i) {
        std::string path(gMainFontPaths[i]);
        std::tr1::shared_ptr<nw4r::ut::Font> font;
        if (path[path.size() - 1] == 'a') {
            std::tr1::shared_ptr<CArchiveFontWithBuf> loaded(new CArchiveFontWithBuf);
            if (!loaded->Load(path.c_str(), true)) {
                ShowSystemMessage(GetMessageString(5, nullptr), true);
                continue;
            }
            font = loaded;
        } else {
            std::tr1::shared_ptr<CResFont> loaded(new CResFont);
            if (!loaded->Load(path.c_str(), true)) {
                ShowSystemMessage(GetMessageString(5, nullptr), true);
                continue;
            }
            font = loaded;
        }
        const char* pName = path.c_str() + path.rfind('/') + 1;
        if (i == 0) {
            extension = font;
            font->SetAlternateChar('?');
            (*fonts)[pName] = font;
        } else if (strcmp(pName, "fot_newrodin_pro_eb16.brfnt") == 0) {
            (*fonts)[pName] = font;
            (*fonts)[std::string("_") + pName] = font;
        } else {
            std::tr1::shared_ptr<nw4r::ut::PairFont> pair(new nw4r::ut::PairFont(extension.get(), font.get()));
            (*fonts)[pName] = pair;
            (*fonts)[std::string("_") + pName] = font;
        }
    }
    return fonts;
}

CResFont::~CResFont() {}


CMain::~CMain() {
    mFiber.reset();
    FinalizeColorConstants();
    ShutdownIconCollection();
    FinalizeHomeButton();
}


#include <game/CResFont.h>

CResFont::CResFont() {}

void CMain::RunSession() {
    mSession = ITitleSession::Create();
    mSession->Run();
    mSession.reset();
    mSession = IGameSession::Create(mDisconMsg.get());
    mSession->Run();
    mSession.reset();
}

