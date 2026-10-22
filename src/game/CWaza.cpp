#include <game/CWaza.h>
#include <game/CPpuEngine.h>
#include <game/BModelObj.h>
#include <game/CModel.h>
#include <game/CEffect.h>
#include <game/IGameSession.h>
#include <game/WazaDescData.h>

extern const EffectInfo gEffectInfos[696];

CWaza::CWaza(u32 type) : BWorldObj(type | 0x10000), mWazaID(0) {}

CWaza::~CWaza() {}

void CWaza::ClearBlownEnemysCount() {
    std::map<u32, u32> empty;
    mBlownEnemysCount.swap(empty);
}

u32 CWaza::GetBlownEnemysCount(u32 id) {
    std::map<u32, u32>::iterator it = mBlownEnemysCount.find(id);
    if (it == mBlownEnemysCount.end()) {
        return 0;
    }
    return it->second;
}

void CWaza::IncBlownEnemysCount(u32 id) {
    std::map<u32, u32>::iterator it = mBlownEnemysCount.find(id);
    if (it == mBlownEnemysCount.end()) {
        mBlownEnemysCount[id] = 1;
    } else {
        ++it->second;
    }
}

bool CWaza::GetAttachmentPosition(nw4r::math::VEC3& rPosition) {
    BWorldObj* pParent = GetParent();
    if (pParent == nullptr) {
        rPosition = mPosition;
        return false;
    }
    rPosition = pParent->GetPositionRef();
    IGameSession* pSession = GetGameSession();
    if (pSession == nullptr) {
        return false;
    }
    int effectID = pSession->GetWazaDescData()->GetCell(mWazaID, 1)->ToInt();
    if (!(0 <= effectID && effectID < 696)) {
        return false;
    }
    const EffectInfo& rEffect = gEffectInfos[effectID];
    if (rEffect.mNodeName == nullptr) {
        return false;
    }
    BModelObj* pModelObj = DynamicCast<BModelObj>(pParent);
    if (pModelObj == nullptr) {
        return false;
    }
    if (!pModelObj->GetModel()) {
        return false;
    }
    nw4r::g3d::ScnMdlSimple* pModel = pModelObj->GetModel()->mModelData.GetModel();
    if (pModel == nullptr) {
        return false;
    }
    nw4r::g3d::ResNode node = pModel->GetResMdl().GetResNode(rEffect.mNodeName);
    if (!node.IsValid()) {
        return false;
    }
    nw4r::math::MTX34 matrix;
    pModel->GetScnMtxPos(&matrix, nw4r::g3d::ScnObj::MTX_WORLD, node.GetID());
    rPosition = nw4r::math::VEC3(matrix._03, matrix._13, matrix._23);
    return true;
}

void CWaza::Setup(int id, std::tr1::shared_ptr<BWorldObj> parent) {
    mWazaID = id;
    SetParent(parent);
    if (parent) {
        SetRotation(parent->GetRotation());
        nw4r::math::VEC3 position;
        GetAttachmentPosition(position);
        SetPosition(position);
    }
}

bool CWaza::LoadScript(int mode, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnvironment) {
    mPpuEngine.reset(new CPpuEngine);
    return mPpuEngine->Load(this, "CWaza.pkc", mode, rEnvironment);
}

void CWaza::Calc() {
    if (mPpuEngine) {
        mPpuEngine->Calc();
    }
    BSoundSource::SetPosition(mPosition);
}

void CWaza::OnLeaveWorld(BWorld* pWorld) {
    if (mPpuEngine) {
        mPpuEngine->Reset();
    }
}

CPpuEngine* CWaza::GetPpuEngine() const {
    return mPpuEngine.get();
}
