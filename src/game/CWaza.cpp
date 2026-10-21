#include <game/CWaza.h>
#include <game/CPpuEngine.h>

CWaza::CWaza(u32 type) : BWorldObj(type | 0x10000), mWazaID(0) {}

CWaza::~CWaza() {}

void CWaza::Calc() {
    if (mPpuEngine) {
        mPpuEngine->Calc();
    }
    BSoundSource::SetPosition(mPosition);
}

void CWaza::OnLeaveWorld() {
    if (mPpuEngine) {
        mPpuEngine->Reset();
    }
}

CPpuEngine* CWaza::GetPpuEngine() const {
    return mPpuEngine.get();
}
