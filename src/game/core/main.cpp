#include <game/ui/SystemMessageData.h>
#define MAIN_EMISSION_LAYOUT

class CResFont;
class CArchiveFontWithBuf;
class CDisableHBM;
class CDisablePadRumble;
class ISession;
class IDisconMsg;
class CMsgRes;
class IStrapReminder;
class ISaveManager;
namespace nw4r {
namespace ut {
class Font;
class PairFont;
}
}
#include <memory>
enum { MainPointerFirst0 = sizeof(std::tr1::shared_ptr<CMsgRes>) };
enum { MainPointerFirst1 = sizeof(std::tr1::shared_ptr<IDisconMsg>) };
enum { MainPointerFirst2 = sizeof(std::tr1::shared_ptr<ISession>) };
namespace std {
namespace tr1 {
template <> CMsgRes *shared_ptr<CMsgRes>::operator->() const;
template <> ISaveManager *shared_ptr<ISaveManager>::operator->() const;
template <> IStrapReminder *shared_ptr<IStrapReminder>::operator->() const;
template <class T> inline T *shared_ptr<T>::operator->() const {
    return ptr_;
}
}
}
enum { MainPointerLast0 = sizeof(std::tr1::shared_ptr<nw4r::ut::Font>) };
enum { MainPointerLast1 = sizeof(std::tr1::shared_ptr<CDisablePadRumble>) };
enum { MainPointerLast2 = sizeof(std::tr1::shared_ptr<CDisableHBM>) };
enum { MainPointerLast3 = sizeof(std::tr1::shared_ptr<CArchiveFontWithBuf>) };
enum { MainPointerLast4 = sizeof(std::tr1::shared_ptr<CResFont>) };
enum { MainPointerLast5 = sizeof(std::tr1::shared_ptr<nw4r::ut::PairFont>) };
class CMsgRes;
namespace std {
namespace tr1 {
template <> template <> shared_ptr<CMsgRes>::shared_ptr(CMsgRes *p);
}
}
#ifndef GAME_CRESFONT_H
#define GAME_CRESFONT_H

#include <game/io/CFileData.h>
#include <nw4r/ut/ut_ResFont.h>
#include <game/core/GameLoop.h>

class CResFont : public nw4r::ut::ResFont {
public:
    bool Load(const char *pPath, bool background);

private:
    std::tr1::shared_ptr<CFileData> mFile;
};

#endif
#include <game/io/CPath.h>
enum { MainFileHandlerSize = sizeof(std::tr1::function<void(CPath::Drive, const char *, long, unsigned long)>) };
#include <game/text/CArchiveFontWithBuf.h>
#include <game/script/CPpuEngine.h>
#include <game/core/NResetMgr.h>
#include <game/ui/CLayout.h>
enum { MainLayoutHandlerSize = sizeof(std::tr1::function<void(const CLayout::CInitializeException &)>) };
#include <game/io/FileList.h>
#include <game/session/ITitleSession.h>
#include <game/session/IGameSession.h>
#include <game/ui/IColorConst.h>
#include <game/ui/CIconCollection.h>
#include <game/ui/CStrapReminder.h>
namespace std {
namespace tr1 {
namespace detail {
template <> class function_base<void (*)()>;
}
}
}
#ifndef GAME_HOME_BUTTON_H
#define GAME_HOME_BUTTON_H

#include <functional>
#include <string>
#include <revolution/hbm/HBMBase.h>
#include <game/ui/DisableHBM.h>

class CHBMParam {
public:
    enum Flags { NoSaveWiiMenu = 1, NoSaveReset = 2, NoSaveAll = 4, PreloadResources = 16 };

    CHBMParam();
    ~CHBMParam();
    void SetResourceRoot(const char *pPath) {
        mResourceRoot = pPath;
    }
    void SetSoundRoot(const char *pPath) {
        mSoundRoot = pPath;
    }
    void SetExtensionRoot(const char *pPath) {
        mExtensionRoot = pPath;
    }
    void SetNoSaveMessagePath(const char *pPath) {
        mNoSaveMessagePath = pPath;
    }
    void SetMessagePath(const char *pPath) {
        mMessagePath = pPath;
    }
    void SetNetworkMessagePath(const char *pPath) {
        mNetworkMessagePath = pPath;
    }
    void SetModelPath(const char *pPath) {
        mModelPath = pPath;
    }
    void SetTextureName(const char *pName) {
        mTextureName = pName;
    }
    void SetFlags(unsigned long flags) {
        mFlags |= flags;
    }
    void SetOpenCallback(const std::tr1::function<void()> &rCallback);
    void SetCloseCallback(const std::tr1::function<bool(HBMSelectBtnNum)> &rCallback) {
        mCloseCallback = rCallback;
    }
    const std::string &GetResourceRoot() const {
        return mResourceRoot;
    }
    const std::string &GetSoundRoot() const {
        return mSoundRoot;
    }
    const std::string &GetExtensionRoot() const {
        return mExtensionRoot;
    }
    const char *GetNoSaveMessagePath() const {
        return mNoSaveMessagePath.c_str();
    }
    const char *GetMessagePath() const {
        return mMessagePath.c_str();
    }
    const char *GetNetworkMessagePath() const {
        return mNetworkMessagePath.c_str();
    }
    const char *GetModelPath() const {
        return mModelPath.c_str();
    }
    const char *GetTextureName() const {
        return mTextureName.c_str();
    }
    bool HasFlags(unsigned long flags) const {
        return (mFlags & flags) != 0;
    }
    const std::tr1::function<void()> &GetOpenCallback() const {
        return mOpenCallback;
    }
    const std::tr1::function<bool(HBMSelectBtnNum)> &GetCloseCallback() const {
        return mCloseCallback;
    }
    float GetFrameRate() const {
        return mFrameRate;
    }

private:
    unsigned long mFlags;
    float mFrameRate;
    std::string mResourceRoot;
    std::string mSoundRoot;
    std::string mExtensionRoot;
    std::string mNoSaveMessagePath;
    std::string mMessagePath;
    std::string mNetworkMessagePath;
    std::string mModelPath;
    std::string mTextureName;
    std::tr1::function<void()> mOpenCallback;
    std::tr1::function<bool(HBMSelectBtnNum)> mCloseCallback;
};

inline CHBMParam::Flags operator|(CHBMParam::Flags left, CHBMParam::Flags right) {
    return static_cast<CHBMParam::Flags>(static_cast<unsigned long>(left) | static_cast<unsigned long>(right));
}

bool InitializeHomeButton(const CHBMParam &rParam, bool allowYield);

#endif

#include <stdio.h>
void FinalizeHomeButton();

#include <game/session/ISession.h>
#include <game/core/CFiber.h>
#include <game/ui/IDisconMsg.h>
#include <game/io/ISaveManager.h>
#include <game/ui/IStrapReminder.h>
#include <game/world/IGameMap.h>
#include <game/ui/IPauseMenu.h>
#include <game/graphics/Graphics.h>
#include <game/input/Pad.h>
#include <game/input/ControllerTable.h>
#include <game/audio/Sound.h>
#include <revolution/gx.h>

extern "C" void GXInvalidateTexAll();

void DrawHomeButton();
bool IsHomeButtonActive();
void UpdateResourceFiles();
void UpdateHomeButton();

#ifndef GAME_CMAIN_H
#define GAME_CMAIN_H

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
    virtual bool IsRunning() {
        return true;
    }
    virtual void ResetTiming() {
        SetPadReadDelay(1);
    }
    virtual void Update() {
        UpdateResourceFiles();
        UpdatePad();
        UpdateControllers();
        UpdateSound();
        UpdateHomeButton();
        if (!IsHomeButtonActive()) {
            if (!mDisconMsg || (mDisconMsg->Calc(), !mDisconMsg->IsActive())) {
                if (IGameMap *pMap = GetActiveGameMap()) {
                    if (IPauseMenu *pMenu = pMap->GetPauseMenu()) {
                        pMenu->Calc();
                        pMenu->IsOpen();
                    }
                }
            }
        }
        if (ISaveManager *pSave = ISaveManager::GetInstance())
            pSave->Calc();
        NResetMgr::Update();
    }
    virtual bool ShouldRender() const {
        if (mDisconMsg) {
            if (mDisconMsg->IsActive())
                return false;
        }
        if (IGameMap *pMap = GetActiveGameMap()) {
            if (IPauseMenu *pMenu = pMap->GetPauseMenu()) {
                if (pMenu->IsOpen())
                    return false;
            }
        }
        return !IsHomeButtonActive();
    }
    virtual void Prepare() {
        mFiber->Resume();
        if (mSession)
            mSession->Calc();
        if (mStrapReminder)
            mStrapReminder->Calc();
    }
    virtual void Calc() {
        if (mSession)
            mSession->CalcMtx();
        if (mDisconMsg)
            mDisconMsg->CalcMtx();
        if (mStrapReminder)
            mStrapReminder->CalcMtx();
    }
    virtual void Draw() {
        if (mSession)
            mSession->CalcView();
        if (mDisconMsg)
            mDisconMsg->CalcView();
        if (mStrapReminder)
            mStrapReminder->CalcView();
    }
    virtual void Render() {
        GXSetDither(GX_TRUE);
        Graphics::ResetState();
        Graphics::SetPixelFormat(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
        GXInvalidateVtxCache();
        GXInvalidateTexAll();
        if (mSession)
            mSession->Draw();
        if (mStrapReminder)
            mStrapReminder->Draw();
        if (IGameMap *pMap = GetActiveGameMap()) {
            if (IPauseMenu *pMenu = pMap->GetPauseMenu())
                pMenu->Draw();
        }
        if (mDisconMsg)
            mDisconMsg->Draw();
        if (ISaveManager *pSave = ISaveManager::GetInstance())
            pSave->Draw();
        DrawHomeButton();
        Graphics::CopyFrameBuffer();
    }
    virtual void Present() {
        Graphics::DisplayFrameBuffer();
    }
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
#include <game/text/Message.h>

#ifndef GAME_RESOURCEFILECONFIG_H
#define GAME_RESOURCEFILECONFIG_H

typedef std::tr1::function<void(CPath::Drive, const char *, long, unsigned long)> FileErrorHandler;

struct ResourceFileConfig {
    ResourceFileConfig();
    ~ResourceFileConfig();
    void SetErrorHandler(const FileErrorHandler &rHandler);
    FileErrorHandler mErrorHandler;
    int mReadPriority;
    int mDecodePriority;
    unsigned long mStackSize;
    bool mUseDriveWorker;
};

void HandleFileError(CPath::Drive drive, const char *pPath, long error, unsigned long flags);
void InitializeResourceFiles(const ResourceFileConfig &rConfig);
void ShutdownResourceFiles();

#endif

#include <game/core/MainRuntime.h>
#include <nw4r/ut/ut_PairFont.h>
namespace std {
namespace tr1 {
namespace detail {}
}
}
namespace std {
namespace tr1 {
namespace detail {}
}
}
namespace std {
namespace tr1 {
template <> template <> shared_ptr<nw4r::ut::PairFont>::shared_ptr(nw4r::ut::PairFont *p);
template <> template <> shared_ptr<CResFont>::shared_ptr(CResFont *p);
template <> template <> shared_ptr<CArchiveFontWithBuf>::shared_ptr(CArchiveFontWithBuf *p);
template <> template <> shared_ptr<CLayout::FontMap>::shared_ptr(CLayout::FontMap *p);
}
}
namespace std {
namespace tr1 {
template <>
template <>
shared_ptr<nw4r::ut::Font> &shared_ptr<nw4r::ut::Font>::operator=(const shared_ptr<nw4r::ut::PairFont> &r);
template <>
template <>
shared_ptr<nw4r::ut::Font> &shared_ptr<nw4r::ut::Font>::operator=(const shared_ptr<CResFont> &r);
template <>
template <>
shared_ptr<nw4r::ut::Font> &shared_ptr<nw4r::ut::Font>::operator=(const shared_ptr<CArchiveFontWithBuf> &r);
}
}
namespace std {
namespace tr1 {
template <> template <> weak_ptr<CLayout::FontMap>::weak_ptr(const shared_ptr<CLayout::FontMap> &r, void *);
}
}
namespace std {
template <> struct pair<const string, tr1::shared_ptr<nw4r::ut::Font> > {
    typedef const string first_type;
    typedef tr1::shared_ptr<nw4r::ut::Font> second_type;
    pair();
    pair(const first_type &rFirst, const second_type &rSecond) : first(rFirst), second(rSecond) {}
    template <typename U, typename V>
    pair(const pair<U, V> &rOther,
         typename Metrowerks::restrict_to<Metrowerks::is_convertible<U, first_type>::value &&
                                          Metrowerks::is_convertible<V, second_type>::value>::type * = nullptr)
        : first(tr1::get<0>(rOther)), second(tr1::get<1>(rOther)) {}
    ~pair();
    first_type first;
    second_type second;
};
}
namespace std {
namespace detail {
typedef pair<const string, tr1::shared_ptr<nw4r::ut::Font> > MainFontValue;
typedef map_member_compare<string, tr1::shared_ptr<nw4r::ut::Font>, less<string> > MainFontCompare;
typedef tree_imp<MainFontValue, MainFontCompare, allocator<MainFontValue> > MainFontTree;
template <>
template <>
tree_node_base *MainFontTree::find_leaf(const string &rKey, node *&rpPred, bool &rLeft, bool &rLeftmost);
}
}
enum { MainTypeSize0 = sizeof(std::tr1::detail::shared_ptr_deleter<CMsgRes, std::detail::default_delete<CMsgRes> >) };
enum { MainFiberType = sizeof(std::tr1::detail::shared_ptr_deleter<CFiber, std::detail::default_delete<CFiber> >) };

enum {
    MainTypeSize1 = sizeof(
        std::tr1::detail::shared_ptr_deleter<nw4r::ut::PairFont, std::detail::default_delete<nw4r::ut::PairFont> >)
};
enum { MainTypeSize2 = sizeof(std::tr1::detail::shared_ptr_deleter<CResFont, std::detail::default_delete<CResFont> >) };
enum {
    MainTypeSize3 = sizeof(
        std::tr1::detail::shared_ptr_deleter<CArchiveFontWithBuf, std::detail::default_delete<CArchiveFontWithBuf> >)
};
enum {
    MainTypeSize4 =
        sizeof(std::tr1::detail::shared_ptr_deleter<CLayout::FontMap, std::detail::default_delete<CLayout::FontMap> >)
};
enum {
    MainTypeSize5 = sizeof(std::tr1::detail::shared_ptr_deleter<CDisableHBM, std::detail::default_delete<CDisableHBM> >)
};
namespace std {
namespace tr1 {
namespace detail {
template <> class function_base<bool (*)(HBMSelectBtnNum)> {
public:
    virtual ~function_base();
    virtual function_base *clone() const = 0;
    virtual function_base *clone(void *pBuffer) const = 0;
    virtual bool operator()(HBMSelectBtnNum argument) = 0;
};
}
}
}
enum {
    MainTypeSize6 =
        sizeof(std::tr1::detail::function_imp<bool (*)(HBMSelectBtnNum), bool (*)(HBMSelectBtnNum), false, false>)
};
namespace std {
namespace tr1 {
namespace detail {
template <> class function_base<void (*)()> {
public:
    virtual ~function_base() {}
    virtual function_base *clone() const = 0;
    virtual function_base *clone(void *pBuffer) const = 0;
    virtual void operator()() = 0;
};
}
}
}
enum { MainTypeSize7 = sizeof(std::tr1::detail::function_imp<void (*)(), void (*)(), false, false>) };
enum {
    MainTypeSize8 =
        sizeof(std::tr1::detail::shared_ptr_deleter<CDisablePadRumble, std::detail::default_delete<CDisablePadRumble> >)
};
enum {
    MainTypeSize9 =
        sizeof(std::tr1::detail::function_imp<
               void (*)(),
               std::tr1::detail::bound_func<void, std::tr1::detail::mem_fn_0<void, CMain>, std::tr1::tuple<CMain *> >,
               false, false>)
};
namespace std {
namespace tr1 {
namespace detail {
template <> class function_base<void (*)(CPath::Drive, const char *, long, unsigned long)> {
public:
    virtual ~function_base();
    virtual function_base *clone() const = 0;
    virtual function_base *clone(void *pBuffer) const = 0;
    virtual void operator()(CPath::Drive first, const char *second, long third, unsigned long fourth) = 0;
};
}
}
}
enum {
    MainTypeSize10 =
        sizeof(std::tr1::detail::function_imp<void (*)(CPath::Drive, const char *, long, unsigned long),
                                              void (*)(CPath::Drive, const char *, long, unsigned long), false, false>)
};
namespace std {
namespace tr1 {
namespace detail {
template <> class function_base<void (*)(const CLayout::CInitializeException &)> {
public:
    virtual ~function_base();
    virtual function_base *clone() const = 0;
    virtual function_base *clone(void *pBuffer) const = 0;
    virtual void operator()(const CLayout::CInitializeException &argument) = 0;
};
}
}
}
enum {
    MainTypeSize11 =
        sizeof(std::tr1::detail::function_imp<void (*)(const CLayout::CInitializeException &),
                                              void (*)(const CLayout::CInitializeException &), false, false>)
};

inline void CHBMParam::SetOpenCallback(const std::tr1::function<void()> &rCallback) {
    mOpenCallback = rCallback;
}
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

void ShowLayoutFailureMessage(const CLayout::CInitializeException &rError) {
    ShowSystemMessage(GetMessageString(5, nullptr), true);
}

#include <game/ui/SystemMessage.h>
#include <revolution/os.h>

bool UsesAlternateErrorMessage(CPath::Drive drive);

void HandleFileError(CPath::Drive drive, const char *pPath, long error, unsigned long flags) {
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
    int number = drive;
    if (drive < 2 || number >= 512) {
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

const MainMessageFile gMainMessageFiles[9] = {
    {"message_list.bmg", false}, {"localedef.bmg", false}, {"pokemon.bmg", false},
    {"pokemon_type.bmg", false}, {"zkn_type.bmg", false},  {"main.bmg", true},
    {"script.bmg", true},        {"wazaname.bmg", false},  {"wazadesc.bmg", false}};

struct FileLoadParam {
    const char *mpPath;
    u32 mFlags;
};

std::tr1::shared_ptr<FileList> LoadFiles(const FileLoadParam *pFiles, unsigned int count, int background);
inline std::tr1::shared_ptr<FileList> LoadLocalizedFiles(const FileLoadParam *pFiles, unsigned int count,
                                                         int background) {
    return LoadFiles(pFiles, count, background);
}

inline std::tr1::shared_ptr<FileList> CMain::LoadStartupArchives() {
    FileLoadParam requests[] = {
        {"5:/pii_1-151_387-492.arc", 0}, {"5:/item.arc", 0},          {"5:/game2d.arc", 0},        {"5:/font.arc", 0},
        {"5:/effect/main.breff", 0},     {"5:/effect/main.breft", 0}, {"5:/sound/waza_p.brsar", 0}};
    return LoadLocalizedFiles(requests, 7, true);
}

extern const char *gMainFontPaths[6];
const char *gMainFontPaths[6] = {"5:/font.arc/fot_extensions.brfnt",      "5:/font.arc/fot_newrodin_pro_eb16.brfnt",
                                 "5:/font.arc/fot_newrodin_pro_eb.brfnt", "5:/font.arc/fot_rocknroll_std.brfnt",
                                 "5:/font.arc/fot_wanpaku_b.brfnt",       "5:/font.arc/fot_wanpaku_b_hd.brfnt"};

#include <game/text/Locale.h>
#include <revolution/sc.h>
extern const DateMessageIDs gMainDateMessageIDs;
extern const TimeMessageIDs gMainTimeMessageIDs;
extern void *const gSystemMessageResources[6];
void InitializeMainMessages() {
    InitMsgResList(32);
    SetDateMessageIDs(gMainDateMessageIDs);
    SetTimeMessageIDs(gMainTimeMessageIDs);
    void *pResource;
    if (GetGameRegion() == REGION_EUR && GetGameLang() == SC_LANG_ENGLISH) {
        pResource = &gEuropeanEnglishSystemMessages;
    } else {
        pResource = gSystemMessageResources[GetGameLang()];
    }
    std::tr1::shared_ptr<CMsgRes> messages(new CMsgRes(pResource, true));
    SetMsgRes(messages->GetGroupID(), messages);
}

namespace std {
namespace tr1 {
template <> CMsgRes *shared_ptr<CMsgRes>::operator->() const {
    return ptr_;
}
}
}
namespace std {
namespace tr1 {
template <> template <> shared_ptr<CMsgRes>::shared_ptr(CMsgRes *p) : ptr_(p) {
    Metrowerks::move_ptr<CMsgRes> hold(p);
    s_ = new detail::shared_ptr_deleter<CMsgRes, std::detail::default_delete<CMsgRes> >(p);
    hold.release();
    enable_from_this(p, p);
}
}
}

extern void *const gSystemMessageResources[6];
void *const gSystemMessageResources[6] = {&gJapaneseSystemMessages, &gEnglishSystemMessages, &gGermanSystemMessages,
                                          &gFrenchSystemMessages,   &gSpanishSystemMessages, &gItalianSystemMessages};

extern const DateMessageIDs gMainDateMessageIDs;
const DateMessageIDs gMainDateMessageIDs = {131084, 131072, 131124, 131125, 131122, 131123,
                                            131128, 131103, 131110, 131096, 131126};

extern const TimeMessageIDs gMainTimeMessageIDs;
const TimeMessageIDs gMainTimeMessageIDs = {131137, 131138, 131130, 131131, 131134, 131132, 131133, 131136};

#include <game/core/Memory.h>
#include <game/core/Random.h>
#include <game/graphics/WorkBuffer.h>
#include <nw4r/lyt/lyt_init.h>
#include <nw4r/lyt/lyt_layout.h>
#include <revolution/cnt.h>
#include <revolution/dvd.h>
#include <revolution/fa.h>

void SetTextLocale(int locale);
void InitializeEffect(unsigned long count, unsigned short emitters, unsigned short groups, unsigned long particles,
                      unsigned long flags);
void FinalizeEffect();

inline CMain::CMain() {
    mFiber.reset(new CFiber(nullptr, std::tr1::bind(&CMain::Run, this), 0x8000));
}

namespace std {
namespace tr1 {
template <> template <> function<void()>::function(void (*f)(), detail::unused_argument *);
template <>
template <>
function<bool(HBMSelectBtnNum)>::function(bool (*f)(HBMSelectBtnNum), detail::unused_argument *);
}
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
        config.mUseDriveWorker =
            (CPath::DEVICE_MCS_BASE <= path.GetDevice() && path.GetDevice() < CPath::DEVICE_MCS_LAST + 1) ||
            path.GetDevice() == CPath::DEVICE_UNC;
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

__declspec(noinline) void ResourceFileConfig::SetErrorHandler(const FileErrorHandler &rHandler) {
    FileErrorHandler handler(rHandler);
    mErrorHandler.swap(handler);
}

#include <nw4r/ef/ef_resource.h>

namespace std {
namespace tr1 {
template <> template <> shared_ptr<CDisableHBM>::shared_ptr(CDisableHBM *p);
template <> template <> shared_ptr<CDisablePadRumble>::shared_ptr(CDisablePadRumble *p);
}
}
void CMain::Run() {
    std::tr1::shared_ptr<CDisablePadRumble> rumble(new CDisablePadRumble);
    YieldFiberStarted();
    mStrapReminder = CStrapReminder::Create(true);
    const char *pHomeLocale;
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
    nw4r::ef::Resource *pEffects = nw4r::ef::Resource::GetInstance();
    {
        std::tr1::shared_ptr<CFileData> file;
        file = LoadFile("5:/effect/main.breff", 0, true);
        pEffects->Add(static_cast<u8 *>(file->GetData()));
        file = LoadFile("5:/effect/main.breft", 0, true);
        pEffects->AddTexture(static_cast<u8 *>(file->GetData()));
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

void AppendLoadedFile(FileList &rFiles, const std::tr1::shared_ptr<CFileData> &rFile) {
    rFiles.insert(rFiles.end(), rFile);
}

#include <game/text/CResFont.h>
#include <string.h>
extern const char *gMainFontPaths[6];

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
        const char *pName = path.c_str() + path.rfind('/') + 1;
        if (i == 0) {
            extension = font;
            font->SetAlternateChar('?');
            bool emissionProbe = false;
            if (emissionProbe) {
                static_cast<CLayout::FontMap *>(nullptr)->operator[](path);
            }
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

namespace std {
namespace tr1 {
template <>
template <>
weak_ptr<CLayout::FontMap>::weak_ptr(const shared_ptr<CLayout::FontMap> &r, void *) : ptr_(r.ptr_), s_(r.s_) {
    if (s_)
        s_->attach_weak();
}
}
}
namespace std {
namespace tr1 {
template <> IStrapReminder *shared_ptr<IStrapReminder>::operator->() const {
    return ptr_;
}
template <> ISaveManager *shared_ptr<ISaveManager>::operator->() const {
    return ptr_;
}
}
}
void CMain::RunSession() {
    mSession = ITitleSession::Create();
    mSession->Run();
    mSession.reset();
    mSession = IGameSession::Create(mDisconMsg.get());
    mSession->Run();
    mSession.reset();
}
CMain::~CMain() {
    mFiber.reset();
    FinalizeColorConstants();
    ShutdownIconCollection();
    FinalizeHomeButton();
}

namespace std {
namespace tr1 {
template <>
template <>
shared_ptr<nw4r::ut::Font> &shared_ptr<nw4r::ut::Font>::operator=(const shared_ptr<nw4r::ut::PairFont> &r) {
    shared_ptr(r).swap(*this);
    return *this;
}
template <> template <> shared_ptr<nw4r::ut::PairFont>::shared_ptr(nw4r::ut::PairFont *p) : ptr_(p) {
    Metrowerks::move_ptr<nw4r::ut::PairFont> hold(p);
    s_ = new detail::shared_ptr_deleter<nw4r::ut::PairFont, std::detail::default_delete<nw4r::ut::PairFont> >(p);
    hold.release();
    enable_from_this(p, p);
}
}
}
namespace std {
namespace detail {
template <>
template <>
tree_node_base *MainFontTree::find_leaf(const string &rKey, node *&rpPred, bool &rLeft, bool &rLeftmost) {
    rpPred = nullptr;
    node *pLeaf = static_cast<node *>(end_node());
    node *pNode = static_cast<node *>(root());
    rLeft = true;
    rLeftmost = true;
    while (pNode != nullptr) {
        pLeaf = pNode;
        if (comp()(rKey, pNode->value_)) {
            pNode = static_cast<node *>(pNode->left_);
            rLeft = true;
        } else {
            rpPred = pLeaf;
            pNode = static_cast<node *>(pNode->right_);
            rLeft = false;
            rLeftmost = false;
        }
    }
    return pLeaf;
}
}
}
namespace std {
pair<const string, tr1::shared_ptr<nw4r::ut::Font> >::~pair() {}
}
namespace std {
namespace tr1 {
template <>
template <>
shared_ptr<nw4r::ut::Font> &shared_ptr<nw4r::ut::Font>::operator=(const shared_ptr<CResFont> &r) {
    shared_ptr(r).swap(*this);
    return *this;
}
template <> template <> shared_ptr<CResFont>::shared_ptr(CResFont *p) : ptr_(p) {
    Metrowerks::move_ptr<CResFont> hold(p);
    s_ = new detail::shared_ptr_deleter<CResFont, std::detail::default_delete<CResFont> >(p);
    hold.release();
    enable_from_this(p, p);
}
template <>
template <>
shared_ptr<nw4r::ut::Font> &shared_ptr<nw4r::ut::Font>::operator=(const shared_ptr<CArchiveFontWithBuf> &r) {
    shared_ptr(r).swap(*this);
    return *this;
}
template <> template <> shared_ptr<CArchiveFontWithBuf>::shared_ptr(CArchiveFontWithBuf *p) : ptr_(p) {
    Metrowerks::move_ptr<CArchiveFontWithBuf> hold(p);
    s_ = new detail::shared_ptr_deleter<CArchiveFontWithBuf, std::detail::default_delete<CArchiveFontWithBuf> >(p);
    hold.release();
    enable_from_this(p, p);
}
template <> template <> shared_ptr<CLayout::FontMap>::shared_ptr(CLayout::FontMap *p) : ptr_(p) {
    Metrowerks::move_ptr<CLayout::FontMap> hold(p);
    s_ = new detail::shared_ptr_deleter<CLayout::FontMap, std::detail::default_delete<CLayout::FontMap> >(p);
    hold.release();
    enable_from_this(p, p);
}
}
}
namespace std {
namespace tr1 {
template <> template <> shared_ptr<CDisableHBM>::shared_ptr(CDisableHBM *p) : ptr_(p) {
    Metrowerks::move_ptr<CDisableHBM> hold(p);
    s_ = new detail::shared_ptr_deleter<CDisableHBM, std::detail::default_delete<CDisableHBM> >(p);
    hold.release();
    enable_from_this(p, p);
}
template <>
template <>
function<bool(HBMSelectBtnNum)>::function(bool (*f)(HBMSelectBtnNum), detail::unused_argument *) : f_(nullptr) {
    if (f != nullptr) {
        typedef detail::function_imp<bool (*)(HBMSelectBtnNum), bool (*)(HBMSelectBtnNum), false, false> imp;
        f_ = new (buf_) imp(f);
    }
}
detail::function_base<bool (*)(HBMSelectBtnNum)>::~function_base() {}
template <> template <> function<void()>::function(void (*f)(), detail::unused_argument *) : f_(nullptr) {
    if (!detail::is_null(f)) {
        typedef detail::function_imp<void (*)(), void (*)(), false, false> imp;
        if (sizeof(imp) <= sizeof(buf_))
            f_ = new (buf_) imp(f);
        else
            f_ = new imp(f);
    }
}
template <> template <> shared_ptr<CDisablePadRumble>::shared_ptr(CDisablePadRumble *p) : ptr_(p) {
    Metrowerks::move_ptr<CDisablePadRumble> hold(p);
    s_ = new detail::shared_ptr_deleter<CDisablePadRumble, std::detail::default_delete<CDisablePadRumble> >(p);
    hold.release();
    enable_from_this(p, p);
}
}
}
namespace std {
namespace tr1 {
namespace detail {
function_base<void (*)(CPath::Drive, const char *, long, unsigned long)>::~function_base() {}

}
}
}

namespace std {
namespace tr1 {
namespace detail {
function_base<void (*)(const CLayout::CInitializeException &)>::~function_base() {}
}
}
}
ResourceFileConfig::~ResourceFileConfig() {}

namespace std {
inline pair<const string, tr1::shared_ptr<nw4r::ut::Font> >::pair() : first(), second() {}
}
