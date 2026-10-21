#include <game/CPpuRegister.h>
#include <game/NPpuVM.h>
#include <vector>

CPpuRegister::CPpuRegister() {}
CPpuRegister::~CPpuRegister() {}

void CPpuRegister::Reserve(u32 capacity) {
    if (!mValues) mValues.reset(new std::vector<NPpu::Slice>);
    mValues->reserve(capacity);
}

NPpu::CValue CPpuRegister::GetValue(unsigned int index) const {
    if (mValues && index < mValues->size()) return (*mValues)[index];
    return NPpu::CValue();
}

void CPpuRegister::Set(u32 index, const NPpu::CValue& rValue) {
    if (!mValues) mValues.reset(new std::vector<NPpu::Slice>);
    u32 size = mValues->size();
    if (size <= index) {
        mValues->resize(index + 1);
        for (u32 i = size; i < index; ++i) (*mValues)[i].SetInt(0);
    }
    (*mValues)[index] = rValue;
}

void CPpuRegister::Reset() {
    mValues.reset();
}

void CPpuRegister::Swap(CPpuRegister* pOther) {
    if (pOther) {
        std::tr1::shared_ptr<std::vector<NPpu::Slice> > values = pOther->mValues;
        pOther->mValues = mValues;
        mValues = values;
    }
}
