#include <game/BUILayout.h>
#include <game/CScopedActivity.h>
#include <game/DCIM.h>
#include <game/FaAsync.h>
#include <game/IDlgMsgWin.h>
#include <game/IDisconMsg.h>
#include <game/IGameSession.h>
#include <game/Locale.h>
#include <game/Memory.h>
#include <game/Message.h>
#include <game/MessageFormat.h>
#include <game/Sound.h>
#include <revolution/fa/pf_volume.h>
#include <revolution/os.h>
#include <MSL_C/ctype.h>
#include <stdio.h>
#include <wchar.h>

extern "C" s32 FASetupFSI(s8 drive, s16 mode);
extern "C" s32 FAGetdev(s8 drive, PF_DEV_INF* pInfo);
extern "C" s32 FARegistCB(u8 device, void (*pInserted)(s8), void (*pRemoved)(s8));
extern "C" s32 FADetach(s8 drive);
extern "C" s32 FAIsWriteProtected(s8 drive);
extern "C" s32 FABuffering(s8 drive, s32 mode);
extern "C" s32 FASync(s8 drive, s32 mode);
extern "C" s32 FAFclose(FAFile* pFile);
extern "C" s32 FARemove(const char* pPath);
extern "C" const char* OSGetAppGamename();

inline int isupper(int c) {
    return (c < 0 || c >= 256) ? 0 : _current_locale.ctype_cmpt_ptr->ctype_map_ptr[c] & 0x200;
}

std::wstring FormatPhotoMessage(const wchar_t* pFormat, const wchar_t* pDirectory, const wchar_t* pFileName);

static bool sCardPresent;
static bool sCardChanged;

struct CPhotoDeviceQuery {
    s32 mResult;
    PF_DEV_INF mInfo;
    s8 mDrive;
};

void OnPhotoCardInserted(s8 drive) {
    sCardPresent = true;
    sCardChanged = true;
}

void OnPhotoCardRemoved(s8 drive) {
    sCardPresent = false;
}

void* QueryPhotoDevice(void* pArgument) {
    CPhotoDeviceQuery* pQuery = static_cast<CPhotoDeviceQuery*>(pArgument);
    pQuery->mResult = FASetupFSI(pQuery->mDrive, 6);
    pQuery->mResult = FAGetdev(pQuery->mDrive, &pQuery->mInfo);
    return reinterpret_cast<void*>(pQuery->mResult);
}


inline size_t CopyPhotoText(wchar_t* pDestination, const wchar_t* pSource, size_t capacity) {
    wcsncpy(pDestination, pSource, capacity - 1);
    pDestination[capacity - 1] = 0;
    return wcslen(pSource);
}

inline const wchar_t* GetPhotoSpaceMessage(u32 clusters, const PF_DEV_INF& rInfo, wchar_t* pBuffer) {
    int blocks = (clusters * rInfo.bps * rInfo.spc + 0x1ffff) >> 17;
    if (blocks == 1) {
        return GetMessageText(393481, gEmptyText);
    }
    std::wstring message = FormatMessage(GetMessageText(393482, gEmptyText), blocks);
    CopyPhotoText(pBuffer, message.c_str(), 256);
    return pBuffer;
}

void SaveScreenshotToRemoteMemory(IDlgMsgWin* pDialog, BUILayout* pLayout, const void* pJpeg, u32 size) {
    std::tr1::shared_ptr<IDisconMsg::CDisable> discon;
    const wchar_t* pMessage = nullptr;
    std::tr1::shared_ptr<u8> cacheMemory(static_cast<u8*>(GameMemory::Allocate(133120, 32)), GameMemory::Free);
    wchar_t messageBuffer[256];
    PF_CACHE_PAGE cachePages[260] __attribute__((aligned(32)));
    PF_CACHE_SETTING cache = {cachePages, reinterpret_cast<u8 (*)[512]>(cacheMemory.get()), 256, 4, 64, 1};
    FADrvTbl drive = {nullptr, &cache, '0', 0};
    YieldFiber();
    if (pDialog->AskYesNo(GetMessageText(393462, gEmptyText))) {
        pMessage = GetMessageText(393483, gEmptyText);
        if (!FARegistCB(0, OnPhotoCardInserted, OnPhotoCardRemoved) && !WaitFaAttach(0, nullptr, 0, &drive) && (drive.stat & 1) && isupper(drive.drive)) {
            sCardChanged = false;
            sCardPresent = (drive.stat & 0x10) != 0;
            do {
                while (!sCardPresent) {
                    pDialog->ShowMessage(GetMessageText(393477, gEmptyText));
                    if (!pDialog->AskYesNo(GetMessageText(393462, gEmptyText))) {
                        pMessage = nullptr;
                        goto detachPhotoCard;
                    }
                }
                sCardChanged = false;
                if (WaitFaMount(drive.drive)) {
                    if (FAErrnum() == 5) {
                        pMessage = GetMessageText(393480, gEmptyText);
                    } else if (FAErrnum() == 8) {
                        pMessage = GetMessageText(393479, gEmptyText);
                    }
                    break;
                }
                s32 protection = FAIsWriteProtected(drive.drive);
                if (protection == -1) {
                    break;
                }
                if (protection == 1) {
                    pMessage = GetMessageText(393478, gEmptyText);
                    break;
                }
                pLayout->GetLayout()->CreateMotion("begin");
                pLayout->GetLayout()->AppendMotion("loop");
                pLayout->SetText("text", GetMessageText(393470, gEmptyText));
                pLayout->SetText("caution", GetMessageText(393471, gEmptyText));
                pMessage = GetMessageText(393486, gEmptyText);
                discon.reset(new IDisconMsg::CDisable(IGameSession::GetInstance()->GetDisconMsg()));
                do {
                    CPhotoDeviceQuery query;
                    query.mDrive = drive.drive;
                    {
                        std::tr1::shared_ptr<u8> stack(static_cast<u8*>(GameMemory::Allocate(4096, 32)), GameMemory::Free);
                        OSThread thread;
                        if (!OSCreateThread(&thread, QueryPhotoDevice, &query, stack.get() + 4096, 4096, 24, 1)) {
                            {
                                for (int frame = 60; frame > 0; --frame) {
                                    YieldFiber();
                                }
                                pLayout->GetLayout()->CreateMotion("end")->Wait();
                            }
                            goto finishPhotoWrite;
                        }
                        OSResumeThread(&thread);
                        while (!OSIsThreadTerminated(&thread)) {
                            YieldFiber();
                        }
                    }
                    if (query.mResult) {
                        {
                            for (int frame = 60; frame > 0; --frame) {
                                YieldFiber();
                            }
                            pLayout->GetLayout()->CreateMotion("end")->Wait();
                        }
                        break;
                    }
                    u32 clusters = (size + query.mInfo.bps * query.mInfo.spc - 1) / (query.mInfo.bps * query.mInfo.spc);
                    if (FABuffering(drive.drive, 1)) {
                        {
                            for (int frame = 60; frame > 0; --frame) {
                                YieldFiber();
                            }
                            pLayout->GetLayout()->CreateMotion("end")->Wait();
                        }
                        break;
                    }
                    int directory;
                    int file;
                    char name[32];
                    DcfIndexResult result = FindNextDcfIndex(&directory, &file, false);
                    switch (result) {
                        case DcfIoError:
                        {
                            for (int frame = 60; frame > 0; --frame) {
                                YieldFiber();
                            }
                            pLayout->GetLayout()->CreateMotion("end")->Wait();
                        }
                        pMessage = GetMessageText(393483, gEmptyText);
                        break;
                        case DcfIndexFull:
                        {
                            for (int frame = 60; frame > 0; --frame) {
                                YieldFiber();
                            }
                            pLayout->GetLayout()->CreateMotion("end")->Wait();
                        }
                        pMessage = GetMessageText(393488, gEmptyText);
                        break;
                        case DcfRootMissing:
                        if (query.mInfo.ecl < clusters + 2) {
                            pMessage = GetPhotoSpaceMessage(clusters + 2, query.mInfo, messageBuffer);
                            {
                                for (int frame = 60; frame > 0; --frame) {
                                    YieldFiber();
                                }
                                pLayout->GetLayout()->CreateMotion("end")->Wait();
                            }
                            break;
                        }
                        if (WaitFaMkdir("/DCIM")) {
                            {
                                for (int frame = 60; frame > 0; --frame) {
                                    YieldFiber();
                                }
                                pLayout->GetLayout()->CreateMotion("end")->Wait();
                            }
                            break;
                        }
                        case DcfCreateDirectory:
                        if (query.mInfo.ecl < clusters + 1) {
                            pMessage = GetPhotoSpaceMessage(clusters + 1, query.mInfo, messageBuffer);
                            {
                                for (int frame = 60; frame > 0; --frame) {
                                    YieldFiber();
                                }
                                pLayout->GetLayout()->CreateMotion("end")->Wait();
                            }
                            break;
                        }
                        if (FAChdir("/DCIM")) {
                            {
                                for (int frame = 60; frame > 0; --frame) {
                                    YieldFiber();
                                }
                                pLayout->GetLayout()->CreateMotion("end")->Wait();
                            }
                            break;
                        }
                        snprintf(name, sizeof(name), "%03dNIN01", directory);
                        if (WaitFaMkdir(name)) {
                            {
                                for (int frame = 60; frame > 0; --frame) {
                                    YieldFiber();
                                }
                                pLayout->GetLayout()->CreateMotion("end")->Wait();
                            }
                            break;
                        }
                        case DcfUseDirectory:
                        if (query.mInfo.ecl < clusters) {
                            pMessage = GetPhotoSpaceMessage(clusters, query.mInfo, messageBuffer);
                            {
                                for (int frame = 60; frame > 0; --frame) {
                                    YieldFiber();
                                }
                                pLayout->GetLayout()->CreateMotion("end")->Wait();
                            }
                            break;
                        }
                        if (FAChdir("/DCIM")) {
                            {
                                for (int frame = 60; frame > 0; --frame) {
                                    YieldFiber();
                                }
                                pLayout->GetLayout()->CreateMotion("end")->Wait();
                            }
                            break;
                        }
                        snprintf(name, sizeof(name), "%03dNIN01", directory);
                        if (FAChdir(name)) {
                            {
                                for (int frame = 60; frame > 0; --frame) {
                                    YieldFiber();
                                }
                                pLayout->GetLayout()->CreateMotion("end")->Wait();
                            }
                            break;
                        }
                        {
                            for (int frame = 60; frame > 0; --frame) {
                                YieldFiber();
                            }
                            pLayout->GetLayout()->CreateMotion("end")->Wait();
                        }
                        pLayout->GetLayout()->CreateMotion("begin");
                        pLayout->GetLayout()->AppendMotion("loop");
                        pLayout->SetText("text", GetMessageText(393485, gEmptyText));
                        pLayout->SetText("caution", GetMessageText(393471, gEmptyText));
                        if (!sCardPresent) {
                            {
                                for (int frame = 60; frame > 0; --frame) {
                                    YieldFiber();
                                }
                                pLayout->GetLayout()->CreateMotion("end")->Wait();
                            }
                            break;
                        }
                        if (sCardChanged) {
                            {
                                for (int frame = 60; frame > 0; --frame) {
                                    YieldFiber();
                                }
                                pLayout->GetLayout()->CreateMotion("end")->Wait();
                            }
                            pMessage = GetMessageText(393491, gEmptyText);
                            break;
                        }
                        {
                            NResetMgr::CDisable reset;
                            snprintf(name, sizeof(name), "%.3s_%04d.JPG", OSGetAppGamename(), file);
                            FAFile* pFile;
                            if (WaitFaCreate(name, 0, &pFile) || !pFile) {
                                {
                                    for (int frame = 60; frame > 0; --frame) {
                                        YieldFiber();
                                    }
                                    pLayout->GetLayout()->CreateMotion("end")->Wait();
                                }
                                break;
                            }
                            u32 written;
                            if (WaitFaFwrite(const_cast<void*>(pJpeg), 1, size, pFile, &written) || written != size) {
                                FAFclose(pFile);
                                FARemove(name);
                                {
                                    for (int frame = 60; frame > 0; --frame) {
                                        YieldFiber();
                                    }
                                    pLayout->GetLayout()->CreateMotion("end")->Wait();
                                }
                                break;
                            }
                            if (FAFclose(pFile)) {
                                {
                                    for (int frame = 60; frame > 0; --frame) {
                                        YieldFiber();
                                    }
                                    pLayout->GetLayout()->CreateMotion("end")->Wait();
                                }
                                break;
                            }
                        }
                        {
                            for (int frame = 60; frame > 0; --frame) {
                                YieldFiber();
                            }
                            pLayout->GetLayout()->CreateMotion("end")->Wait();
                        }
                        {
                            wchar_t directoryName[32];
                            wchar_t fileName[32];
                            if (GetGameLang() == 0) {
                                swprintf(directoryName, 32, L"\x00a5" L"DCIM\x00a5" L"%03dNIN01", directory);
                            } else {
                                swprintf(directoryName, 32, L"\\DCIM\\%03dNIN01", directory);
                            }
                            swprintf(fileName, 32, L"%.3s_%04d.JPG", OSGetAppGamename(), file);
                            std::wstring message = FormatPhotoMessage(GetMessageText(393463, gEmptyText), directoryName, fileName);
                            CopyPhotoText(messageBuffer, message.c_str(), 256);
                            pMessage = messageBuffer;
                            PlaySound(9);
                        }
                        break;
                    }
                } while (false);
                finishPhotoWrite:
                FASync(drive.drive, 0);
                discon.reset();
                YieldFiber();
            } while (false);
            detachPhotoCard:
            FADetach(drive.drive);
        }
    }
    if (pMessage) {
        if (FAErrnum() == -1) {
            pMessage = GetMessageText(393490, gEmptyText);
        } else if (!sCardPresent) {
            pMessage = GetMessageText(393477, gEmptyText);
        }
        pDialog->ShowMessage(pMessage);
    }
}

std::wstring FormatPhotoMessage(const wchar_t* pFormat, const wchar_t* pDirectory, const wchar_t* pFileName) {
    return (NIntl::basic_formatter<wchar_t>(pFormat) % pDirectory % pFileName).str();
}

void OpenRemoteMemoryDialog(IDlgMsgWin* pDialog, BUILayout* pLayout) {
    std::tr1::shared_ptr<IDisconMsg::CDisable> discon;
    const wchar_t* pMessage = nullptr;
    std::tr1::shared_ptr<u8> cacheMemory(static_cast<u8*>(GameMemory::Allocate(4096, 32)), GameMemory::Free);
    PF_CACHE_PAGE cachePages[8] ATTRIBUTE_ALIGN(32);
    wchar_t messageBuffer[256];
    PF_CACHE_SETTING cache = {cachePages, reinterpret_cast<u8 (*)[512]>(cacheMemory.get()), 4, 4, 1, 1};
    FADrvTbl drive = {nullptr, &cache, '0', 0};
    YieldFiber();
    pDialog->ShowMessage(GetMessageText(393464, gEmptyText));
    pDialog->ShowMessage(GetMessageText(393465, gEmptyText));
    pDialog->SetDefaultYes(false);
    if (pDialog->AskYesNo(GetMessageText(393466, gEmptyText))) {
        pMessage = GetMessageText(393483, gEmptyText);
        if (!FARegistCB(0, OnPhotoCardInserted, OnPhotoCardRemoved) && !WaitFaAttach(0, nullptr, 0, &drive) && (drive.stat & 1) && isupper(drive.drive)) {
            do {
                if (!(drive.stat & 0x10)) {
                    pMessage = GetMessageText(393477, gEmptyText);
                    break;
                }
                sCardPresent = true;
                sCardChanged = false;
                s32 mounted = WaitFaMount(drive.drive);
                if (!sCardPresent) {
                    pMessage = GetMessageText(393477, gEmptyText);
                    break;
                }
                if (mounted) {
                    if (FAErrnum() == 5) {
                        pMessage = GetMessageText(393480, gEmptyText);
                    } else if (FAErrnum() == 8) {
                        pMessage = GetMessageText(393479, gEmptyText);
                    }
                    break;
                }
                s32 protection = FAIsWriteProtected(drive.drive);
                if (protection == -1) {
                    goto unmountPhotoCard;
                }
                if (protection == 1) {
                    pMessage = GetMessageText(393478, gEmptyText);
                    goto unmountPhotoCard;
                }
                if (FABuffering(drive.drive, 1)) {
                    goto unmountPhotoCard;
                }
                pLayout->GetLayout()->CreateMotion("begin");
                pLayout->GetLayout()->AppendMotion("loop");
                pLayout->SetText("text", GetMessageText(393470, gEmptyText));
                pLayout->SetText("caution", GetMessageText(393471, gEmptyText));
                discon.reset(new IDisconMsg::CDisable(IGameSession::GetInstance()->GetDisconMsg()));
                {
                    int count = 0;
                    FADta directories;
                    FADta files;
                    FADta deleteFiles;
                    if (FAChdir("/DCIM")) {
                        {
                            for (int frame = 60; frame > 0; --frame) {
                                YieldFiber();
                            }
                            pLayout->GetLayout()->CreateMotion("end")->Wait();
                        }
                        pMessage = GetMessageText(393472, gEmptyText);
                        goto unmountPhotoCard;
                    }
                    int result = WaitFaFsfirst("???NIN01", 0x10, &directories);
                    if (result && FAErrnum() != 2) {
                        {
                            for (int frame = 60; frame > 0; --frame) {
                                YieldFiber();
                            }
                            pLayout->GetLayout()->CreateMotion("end")->Wait();
                        }
                        goto unmountPhotoCard;
                    }
                    while (!result) {
                        if (!(directories.Attribute & 0x0f) && isdigit(directories.FileName[0]) && isdigit(directories.FileName[1]) && isdigit(directories.FileName[2])) {
                            if (FAChdir(reinterpret_cast<const char*>(directories.FileName))) {
                                {
                                    for (int frame = 60; frame > 0; --frame) {
                                        YieldFiber();
                                    }
                                    pLayout->GetLayout()->CreateMotion("end")->Wait();
                                }
                                goto unmountPhotoCard;
                            }
                            char pattern[16];
                            snprintf(pattern, sizeof(pattern), "%.3s_????.JPG", OSGetAppGamename());
                            int fileResult = WaitFaFsfirst(pattern, 0x7f, &files);
                            if (fileResult && FAErrnum() != 2) {
                                {
                                    for (int frame = 60; frame > 0; --frame) {
                                        YieldFiber();
                                    }
                                    pLayout->GetLayout()->CreateMotion("end")->Wait();
                                }
                                goto unmountPhotoCard;
                            }
                            while (!fileResult) {
                                if (!(files.Attribute & 0x1f) && isdigit(files.FileName[4]) && isdigit(files.FileName[5]) && isdigit(files.FileName[6]) && isdigit(files.FileName[7])) {
                                    ++count;
                                }
                                fileResult = WaitFaFsnext(&files);
                                if (fileResult && FAErrnum() != 2) {
                                    {
                                        for (int frame = 60; frame > 0; --frame) {
                                            YieldFiber();
                                        }
                                        pLayout->GetLayout()->CreateMotion("end")->Wait();
                                    }
                                    goto unmountPhotoCard;
                                }
                            }
                            if (FAChdir("..")) {
                                {
                                    for (int frame = 60; frame > 0; --frame) {
                                        YieldFiber();
                                    }
                                    pLayout->GetLayout()->CreateMotion("end")->Wait();
                                }
                                goto unmountPhotoCard;
                            }
                        }
                        YieldFiber();
                        result = WaitFaFsnext(&directories);
                        if (result && FAErrnum() != 2) {
                            {
                                for (int frame = 60; frame > 0; --frame) {
                                    YieldFiber();
                                }
                                pLayout->GetLayout()->CreateMotion("end")->Wait();
                            }
                            goto unmountPhotoCard;
                        }
                    }
                    {
                        for (int frame = 60; frame > 0; --frame) {
                            YieldFiber();
                        }
                        pLayout->GetLayout()->CreateMotion("end")->Wait();
                    }
                    if (!count) {
                        for (int frame = 60; frame > 0; --frame) {
                            YieldFiber();
                        }
                        pMessage = GetMessageText(393472, gEmptyText);
                        goto unmountPhotoCard;
                    }
                    const wchar_t* pQuestion;
                    if (count == 1) {
                        pQuestion = GetMessageText(393467, gEmptyText);
                    } else if (count < 1000) {
                        std::wstring message = FormatMessage(GetMessageText(393468, gEmptyText), count);
                        CopyPhotoText(messageBuffer, message.c_str(), 256);
                        pQuestion = messageBuffer;
                    } else {
                        pQuestion = GetMessageText(393469, gEmptyText);
                    }
                    pDialog->SetDefaultYes(false);
                    discon.reset();
                    YieldFiber();
                    if (!pDialog->AskYesNo(pQuestion)) {
                        pMessage = nullptr;
                        goto unmountPhotoCard;
                    }
                    if (!sCardPresent) {
                        pMessage = GetMessageText(393477, gEmptyText);
                        goto unmountPhotoCard;
                    }
                    if (sCardChanged) {
                        pMessage = GetMessageText(393489, gEmptyText);
                        goto unmountPhotoCard;
                    }
                    NResetMgr::CDisable reset;
                    pLayout->GetLayout()->CreateMotion("begin");
                    pLayout->GetLayout()->AppendMotion("loop");
                    pLayout->SetText("text", GetMessageText(393474, gEmptyText));
                    pLayout->SetText("caution", GetMessageText(393471, gEmptyText));
                    discon.reset(new IDisconMsg::CDisable(IGameSession::GetInstance()->GetDisconMsg()));
                    pMessage = GetMessageText(393476, gEmptyText);
                    if (FAChdir("/DCIM")) {
                        {
                            for (int frame = 60; frame > 0; --frame) {
                                YieldFiber();
                            }
                            pLayout->GetLayout()->CreateMotion("end")->Wait();
                        }
                        goto unmountPhotoCard;
                    }
                    int removed = 0;
                    int failed = 0;
                    result = WaitFaFsfirst("???NIN01", 0x10, &directories);
                    if (result && FAErrnum() != 2) {
                        {
                            for (int frame = 60; frame > 0; --frame) {
                                YieldFiber();
                            }
                            pLayout->GetLayout()->CreateMotion("end")->Wait();
                        }
                        goto unmountPhotoCard;
                    }
                    while (!result) {
                        if (!(directories.Attribute & 0x0f) && isdigit(directories.FileName[0]) && isdigit(directories.FileName[1]) && isdigit(directories.FileName[2])) {
                            if (FAChdir(reinterpret_cast<const char*>(directories.FileName))) {
                                {
                                    for (int frame = 60; frame > 0; --frame) {
                                        YieldFiber();
                                    }
                                    pLayout->GetLayout()->CreateMotion("end")->Wait();
                                }
                                goto unmountPhotoCard;
                            }
                            char pattern[16];
                            snprintf(pattern, sizeof(pattern), "%.3s_????.JPG", OSGetAppGamename());
                            int fileResult = WaitFaFsfirst(pattern, 0x7f, &deleteFiles);
                            if (fileResult && FAErrnum() != 2) {
                                {
                                    for (int frame = 60; frame > 0; --frame) {
                                        YieldFiber();
                                    }
                                    pLayout->GetLayout()->CreateMotion("end")->Wait();
                                }
                                goto unmountPhotoCard;
                            }
                            while (!fileResult) {
                                if (!(deleteFiles.Attribute & 0x1f) && isdigit(deleteFiles.FileName[4]) && isdigit(deleteFiles.FileName[5]) && isdigit(deleteFiles.FileName[6]) && isdigit(deleteFiles.FileName[7])) {
                                    s32 removedFile;
                                    {
                                        NResetMgr::CDisable fileReset;
                                        removedFile = WaitFaRemove(reinterpret_cast<const char*>(deleteFiles.FileName));
                                    }
                                    if (removedFile == 0) {
                                        ++removed;
                                    } else {
                                        if (!sCardPresent || sCardChanged) {
                                            {
                                                for (int frame = 60; frame > 0; --frame) {
                                                    YieldFiber();
                                                }
                                                pLayout->GetLayout()->CreateMotion("end")->Wait();
                                            }
                                            goto unmountPhotoCard;
                                        }
                                        ++failed;
                                    }
                                }
                                fileResult = WaitFaFsnext(&deleteFiles);
                                if (fileResult && FAErrnum() != 2) {
                                    {
                                        for (int frame = 60; frame > 0; --frame) {
                                            YieldFiber();
                                        }
                                        pLayout->GetLayout()->CreateMotion("end")->Wait();
                                    }
                                    goto unmountPhotoCard;
                                }
                            }
                            if (FAChdir("..")) {
                                {
                                    for (int frame = 60; frame > 0; --frame) {
                                        YieldFiber();
                                    }
                                    pLayout->GetLayout()->CreateMotion("end")->Wait();
                                }
                                goto unmountPhotoCard;
                            }
                            s32 removedDirectory;
                            {
                                NResetMgr::CDisable directoryReset;
                                removedDirectory = WaitFaRmdir(reinterpret_cast<const char*>(directories.FileName));
                            }
                            if (removedDirectory && (!sCardPresent || sCardChanged)) {
                                {
                                    for (int frame = 60; frame > 0; --frame) {
                                        YieldFiber();
                                    }
                                    pLayout->GetLayout()->CreateMotion("end")->Wait();
                                }
                                goto unmountPhotoCard;
                            }
                        }
                        YieldFiber();
                        result = WaitFaFsnext(&directories);
                        if (result && FAErrnum() != 2) {
                            {
                                for (int frame = 60; frame > 0; --frame) {
                                    YieldFiber();
                                }
                                pLayout->GetLayout()->CreateMotion("end")->Wait();
                            }
                            goto unmountPhotoCard;
                        }
                    }
                    {
                        for (int frame = 60; frame > 0; --frame) {
                            YieldFiber();
                        }
                        pLayout->GetLayout()->CreateMotion("end")->Wait();
                    }
                    if (count == removed) {
                        pMessage = GetMessageText(393475, gEmptyText);
                    }
                }
                unmountPhotoCard:
                WaitFaUnmount(drive.drive, 0);
                discon.reset();
                YieldFiber();
            } while (false);
            FADetach(drive.drive);
        }
    }
    if (pMessage) {
        if (FAErrnum() == -1) {
            pMessage = GetMessageText(393490, gEmptyText);
        } else if (!sCardPresent) {
            pMessage = GetMessageText(393477, gEmptyText);
        }
        pDialog->ShowMessage(pMessage);
    }
}
