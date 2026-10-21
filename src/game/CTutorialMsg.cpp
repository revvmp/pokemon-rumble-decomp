const char* spMessagePaneNames[2] = {"text0", "text1"};
#include <game/LayoutResources.h>
#include <nw4r/lyt/lyt_pane.h>

#include <game/CTutorialMsg.h>


std::tr1::shared_ptr<ITutorialMsg> CreateTutorialMsg(bool background) {
    std::tr1::shared_ptr<CTutorialMsg> message(new CTutorialMsg);
    message->Load(background);
    return message;
}



