#include <game/NPpuVM.h>

using namespace NPpu;

int ScriptBinding_GetThrownValue(CEngine* pEngine) {
    CValue value;
    pEngine->GetThrown(value);
    pEngine->Push(value);
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_Interrupt(CEngine* pEngine) {
    CValue value;
    pEngine->Pop(value);
    pEngine->Interrupt(value);
    pEngine->PushInt(0);
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_Yield(CEngine* pEngine) {
    pEngine->PushInt(0);
    return CEngine::NATIVE_YIELD;
}

int ScriptBinding_Exit(CEngine* pEngine) {
    pEngine->PopInt();
    pEngine->PushInt(0);
    return CEngine::NATIVE_STOP;
}

int ScriptBinding_GetThisObject(CEngine* pEngine) {
    CValue value;
    pEngine->GetThis(value);
    pEngine->Push(value);
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_ShowPPU(CEngine* pEngine) {
    pEngine->PushInt(0);
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_SetDebugLevel(CEngine* pEngine) {
    pEngine->PopInt();
    pEngine->PushInt(0);
    return CEngine::NATIVE_CONTINUE;
}

int ScriptBinding_SetInitialDebugLevel(CEngine* pEngine) {
    pEngine->PopInt();
    pEngine->PushInt(0);
    return CEngine::NATIVE_CONTINUE;
}
