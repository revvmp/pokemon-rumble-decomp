const char* spMessagePaneNames[2] = {"text0", "text1"};
#include <game/ui/LayoutResources.h>
#include <nw4r/lyt/lyt_pane.h>

#include <game/ui/CTutorialMsg.h>


std::tr1::shared_ptr<ITutorialMsg> ITutorialMsg::Create(bool background) {
    std::tr1::shared_ptr<CTutorialMsg> message(new CTutorialMsg);
    message->Load(background);
    return message;
}



