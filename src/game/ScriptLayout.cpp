#include <nw4r/ut/ut_algorithm.h>
#include <game/NPpuVM.h>
#include <game/IGameMap.h>
#include <game/ILayoutManager.h>
#include <game/CLayout.h>
#include <game/FileList.h>
#include <game/Font.h>
#include <game/CColorBtnTagProc.h>
#include <game/CPpuEngine.h>
#include <game/BWorld.h>
#include <game/BWorldLightSet.h>
#include <revolution/enc.h>
#include <stdlib.h>
#include <nw4r/lyt/lyt_textBox.h>

static inline const char* PopString(NPpu::CEngine* pEngine) {
    NPpu::Slice value;
    pEngine->Pop(value);
    return pEngine->ToString(value);
}

int LayoutMotion(NPpu::CEngine* pEngine, int mode) {
    int count = pEngine->FetchArgCount();
    float rate = 1.0f;
    if (count > 4) rate = pEngine->PopFloat();
    float start = 0.0f;
    if (count > 3) start = pEngine->PopFloat();
    const char* pPaneName = nullptr;
    if (count > 2) {
        NPpu::Slice value;
        pEngine->Pop(value);
        if (value.mType != NPpu::Slice::TYPE_REFERENCE || value.GetReference() != nullptr)
            pPaneName = pEngine->ToString(value);
    }
    const char* pMotionName = PopString(pEngine);
    int id = pEngine->PopInt();
    std::tr1::shared_ptr<CLayout> owner = GetCurrentGameMap()->GetLayoutManager()->Get(id);
    if (owner) {
        nw4r::lyt::Pane* pPane = nullptr;
        if (pPaneName) pPane = owner->FindPane(pPaneName);
        switch (mode) {
        case 0: owner->CreateMotion(pMotionName, pPane, start, rate); break;
        case 1: owner->QueueMotion(pMotionName, pPane, start, rate); break;
        case 2: owner->AppendMotion(pMotionName, pPane, start, rate); break;
        }
        pEngine->PushInt(1);
    } else {
        pEngine->PushNull();
    }
    return 0;
}

static inline std::tr1::shared_ptr<FileList> LoadArchive(void** ppEntry, const char* pPath, bool background) {
    return LoadFileList(ppEntry, pPath, 0, background, nullptr);
}

int NewLayout(NPpu::CEngine* pEngine) {
    const char* pLayoutName = PopString(pEngine);
    const char* pArchive = PopString(pEngine);
    std::tr1::shared_ptr<CLayout> owner(new CLayout);
    void* pEntry;
    std::tr1::shared_ptr<FileList> files = LoadArchive(&pEntry, pArchive, false);
    owner->Load(static_cast<const char* const*>(pEntry), pLayoutName, GetDefaultFontMap(), false);
    pEngine->PushInt(GetCurrentGameMap()->GetLayoutManager()->Add(owner));
    return 0;
}

int method_layoutDestroy(NPpu::CEngine* pEngine) {
    int id = pEngine->PopInt();
    pEngine->PushInt(GetCurrentGameMap()->GetLayoutManager()->Remove(id));
    return 0;
}

int method_layoutPlay(NPpu::CEngine* pEngine) { return LayoutMotion(pEngine, 0); }
int method_layoutRequest(NPpu::CEngine* pEngine) { return LayoutMotion(pEngine, 1); }
int method_layoutRequestNext(NPpu::CEngine* pEngine) { return LayoutMotion(pEngine, 2); }

int method_layoutSetString(NPpu::CEngine* pEngine) {
    NPpu::Slice text;
    pEngine->Pop(text);
    const char* pName = PopString(pEngine);
    int id = pEngine->PopInt();
    std::tr1::shared_ptr<CLayout> owner = GetCurrentGameMap()->GetLayoutManager()->Get(id);
    if (owner) {
        if (text.IsString()) {
            const char* pText = pEngine->ToString(text);
            s32 size;
            ENCConvertStringSjisToUnicode(nullptr, &size, reinterpret_cast<const u8*>(pText), nullptr);
            wchar_t* pBuffer = static_cast<wchar_t*>(__alloca((size + 1) * sizeof(wchar_t)));
            pBuffer[size] = L'\0';
            ENCConvertStringSjisToUnicode(reinterpret_cast<u16*>(pBuffer), &size, reinterpret_cast<const u8*>(pText), nullptr);
            CColorBtnTagProc::Attach(owner->SetText(pName, pBuffer));
        } else {
            CColorBtnTagProc::Attach(owner->SetText(pName, text.IsWString() ? pEngine->ToWString(text) : L""));
        }
        pEngine->PushInt(1);
    } else {
        pEngine->PushNull();
    }
    return 0;
}

int method_layoutSetVisible(NPpu::CEngine* pEngine) {
    bool visible = pEngine->PopInt() != 0;
    const char* pName = PopString(pEngine);
    int id = pEngine->PopInt();
    std::tr1::shared_ptr<CLayout> owner = GetCurrentGameMap()->GetLayoutManager()->Get(id);
    if (owner) owner->SetPaneVisible(pName, visible);
    pEngine->PushNull();
    return 0;
}

int method_layoutGetLength(NPpu::CEngine* pEngine) {
    const char* pName = PopString(pEngine);
    int id = pEngine->PopInt();
    std::tr1::shared_ptr<CLayout> owner = GetCurrentGameMap()->GetLayoutManager()->Get(id);
    pEngine->PushInt(owner ? owner->GetMotionLength(pName) : 0);
    return 0;
}

int NewWorldLightSet(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    std::tr1::shared_ptr<BWorldLightSet> owner(new BWorldLightSet(BWorldLightSet::TYPE));
    owner->SwapRegister(pEngine->GetRegister());
    BWorld* pWorld = pEngine->GetWorld();
    if (pWorld->Add(owner)) pEngine->PushInt(owner->GetId());
    else pEngine->PushNull();
    return 0;
}
