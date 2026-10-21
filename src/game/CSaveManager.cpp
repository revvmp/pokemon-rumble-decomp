#include <game/ISaveManager.h>

#include <game/CControllerComposition.h>
#include <game/CFiber.h>
#include <game/CLayout.h>
#include <game/CScopedActivity.h>
#include <game/ControllerTable.h>
#include <game/IDlgMsgWin.h>
#include <game/LayoutResources.h>
#include <game/Message.h>
#include <game/SaveData.h>
#include <game/SystemMessage.h>
#include <memory>
#include <queue>
#include <vector>

#pragma dont_inline on

namespace {

class CSaveManager : public ISaveManager {
public:
    enum WorkType {
        WORK_NONE,
        WORK_LOAD,
        WORK_SAVE,
        WORK_SAVE_WITHOUT_BACKUP,
        WORK_SAVE_COMMIT_ONLY,
        WORK_DELETE
    };

    virtual ~CSaveManager();
    virtual void Calc();
    virtual void Draw();
    virtual void LoadSync();
    virtual void RequestSave();
    virtual void RequestSaveWithoutBackup();
    virtual void RequestSaveCommitOnly();
    virtual void RequestDelete();
    virtual bool IsIdle();

    void Work(WorkType type);

private:
    std::tr1::shared_ptr<CFiber> mFiber;
    std::tr1::shared_ptr<CLayout> mLayout;
    std::queue<WorkType> mQueue;
    std::tr1::shared_ptr<IDlgMsgWin> mMsgWin;
};

#pragma force_active on
void CSaveManager::Work(WorkType type) {
    CScopedActivity activity;
    YieldFiberStarted();

    while (true) {
        bool showLayout = type != WORK_LOAD && type != WORK_SAVE_WITHOUT_BACKUP && type != WORK_SAVE_COMMIT_ONLY;

        if (showLayout) {
            std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
            mLayout.reset(new CLayout);
            mLayout->Load(gSharedLayoutArchives, "saving", fonts, true);
            mLayout->CreateMotion("begin")->Wait();
            mLayout->QueueMotion("loop");
        }

        switch (type) {
        case WORK_LOAD:
            if (CheckNandSpace()) {
                int result = Load(false);
                if (result != 0) {
                    mMsgWin = IDlgMsgWin::Create(false, true);
                    std::vector<IController*> controllers;
                    controllers.push_back(GetController(IController::TYPE_WII_REMOTE, 0));
                    GetControllerTable()->SetControllers(controllers);
                    if (Load(true) != 0) {
                        if (result == 0x65) {
                            break;
                        }
                        mMsgWin->SetDefaultYes(false);
                        mMsgWin->AskYesNo(GetMessageString(0x1001C,
                            L"セーブデータが こわれているため\n"
                            L"ゲームを はじめることが できません。\n"
                            L"こわれたファイルを\n"
                            L"さくじょしても よろしいですか？"));
                        if (mMsgWin->IsYesSelected()) {
                            DeleteSaveFiles();
                            break;
                        } else {
                            ShowSystemMessage(GetMessageString(0x1001B,
                                L"セーブデータが こわれているため\n"
                                L"ゲームを はじめることが できません。\n"
                                L"Wiiメニューの セーブデータがめんで\n"
                                L"ファイルを さくじょしてください。"), true);
                            break;
                        }
                    } else {
                        Save(2);
                        mMsgWin->ShowMessage(GetMessageString(0x10021,
                            L"セーブデータが こわれていたため\n"
                            L"ひとつまえの セーブデータに\n"
                            L"まきもどりました\n"));
                    }
                }
            }
            break;
        case WORK_SAVE:
        case WORK_SAVE_WITHOUT_BACKUP:
        case WORK_SAVE_COMMIT_ONLY:
            switch (type) {
            case WORK_SAVE:
                Save(0);
                break;
            case WORK_SAVE_WITHOUT_BACKUP:
                Save(3);
                break;
            case WORK_SAVE_COMMIT_ONLY:
                Save(4);
                break;
            }
            break;
        case WORK_DELETE:
            DeleteSaveFiles();
            break;
        }

        if (showLayout) {
            mLayout->QueueMotion("end")->Wait();
            mLayout.reset();
        }

        if (mQueue.empty()) {
            break;
        }
        type = mQueue.front();
        mQueue.pop();
    }
}
#pragma force_active off

}
