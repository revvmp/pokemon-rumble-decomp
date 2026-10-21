#include <game/CPpuEngine.h>
#include <game/CPii.h>
#include <game/CPlayerPii.h>
#include <game/CPlayer.h>

#pragma dont_inline on

int FaceHeadForward(NPpu::CEngine* pBase) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    CPii* pPii = DynamicCast<CPii>(pEngine->PopWorldObj());
    if (pPii) pPii->FaceHeadForward();
    pBase->PushNull();
    return 0;
}
int FaceHeadTowardZX(NPpu::CEngine* pBase) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    float z = pBase->PopFloat();
    float x = pBase->PopFloat();
    CPii* pPii = DynamicCast<CPii>(pEngine->PopWorldObj());
    if (pPii) pPii->FaceHeadToward(nw4r::math::VEC3(z, pPii->GetPosition().y, x));
    pBase->PushNull();
    return 0;
}
int FaceHeadTowardXYZ(NPpu::CEngine* pBase) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    float z = pBase->PopFloat();
    float y = pBase->PopFloat();
    float x = pBase->PopFloat();
    CPii* pPii = DynamicCast<CPii>(pEngine->PopWorldObj());
    if (pPii) pPii->FaceHeadToward(nw4r::math::VEC3(x, y, z));
    pBase->PushNull();
    return 0;
}
int FaceHeadTowardObj(NPpu::CEngine* pBase) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    BWorldObj* pTarget = pEngine->PopWorldObj();
    CPii* pPii = DynamicCast<CPii>(pEngine->PopWorldObj());
    if (pPii) pPii->FaceHeadToward(pTarget);
    pBase->PushNull();
    return 0;
}
int FaceHeadTowardObjNode(NPpu::CEngine* pBase) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    const char* pNode = pBase->PopString();
    BModelObj* pTarget = DynamicCast<BModelObj>(pEngine->PopWorldObj());
    CPii* pPii = DynamicCast<CPii>(pEngine->PopWorldObj());
    if (pPii) pPii->FaceHeadToward(pTarget, pNode);
    pBase->PushNull();
    return 0;
}
int GetPlayerIndex(NPpu::CEngine* pBase) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    BWorldObj* pObj = pEngine->PopWorldObj();
    int index = -1;
    if (pObj) {
        CPlayerPii* pPii = DynamicCast<CPlayerPii>(pObj);
        if (pPii) index = pPii->GetPlayer()->GetPlayerNo();
    }
    pEngine->PushInt(index);
    return 0;
}
