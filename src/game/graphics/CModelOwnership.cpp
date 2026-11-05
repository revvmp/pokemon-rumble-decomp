#define CMODEL_HELPER_EMISSION_LAYOUT

#pragma section code_type ".text.model_root_bool"
#pragma section code_type ".text.model_late_owners"
#pragma section code_type ".text.model_late_holders"
#pragma section code_type ".text.model_mat_deleter"
#pragma section code_type ".text.model_tex_deleter"
#pragma section code_type ".text.model_shp_deleter"
#pragma section code_type ".text.model_buffers"
#pragma section code_type ".text"
#include <memory>
namespace nw4r { namespace g3d { class AnmObj; } }
enum { kModelAnimationPointerSize = sizeof(std::tr1::shared_ptr<nw4r::g3d::AnmObj>) };
#include <game/graphics/CModel.h>
#include <game/graphics/ScnMdlSimpleDataInline.h>
#include <game/core/CFiber.h>
#include <game/io/CFileData.h>
#include <game/animation/AnmObjChrRootScale.h>
#include <game/core/FrameStep.h>
#include <game/core/Memory.h>
#include <nw4r/g3d/res/g3d_resfile.h>
#include <nw4r/g3d/res/g3d_resmdl.h>
#include <nw4r/g3d/res/g3d_resnode.h>
#include <algorithm>
#include <math.h>

namespace std {
namespace tr1 {

template <> __declspec(section ".text.model_root_bool")
shared_ptr<nw4r::g3d::AnmObjChrRootScale>::operator int bool_type::*() const {
    return ptr_ ? &bool_type::dummy_ : nullptr;
}
namespace detail {
template <> __declspec(section ".text.model_late_owners")
const void* shared_ptr_deleter<nw4r::g3d::AnmObjShpRes, void (*)(nw4r::g3d::G3dObj*)>::get_deleter(const std::type_info& rType) const {
    return rType == typeid(void (*)(nw4r::g3d::G3dObj*)) ? &mStorage.second() : nullptr;
}
template <> __declspec(section ".text.model_late_owners")
void shared_ptr_deleter<nw4r::g3d::AnmObjShpRes, void (*)(nw4r::g3d::G3dObj*)>::dispose() {
    mStorage.second()(mStorage.first());
}
template <> __declspec(section ".text.model_late_owners")
const void* shared_ptr_deleter<nw4r::g3d::AnmObjTexPatRes, void (*)(nw4r::g3d::G3dObj*)>::get_deleter(const std::type_info& rType) const {
    return rType == typeid(void (*)(nw4r::g3d::G3dObj*)) ? &mStorage.second() : nullptr;
}
template <> __declspec(section ".text.model_late_owners")
void shared_ptr_deleter<nw4r::g3d::AnmObjTexPatRes, void (*)(nw4r::g3d::G3dObj*)>::dispose() {
    mStorage.second()(mStorage.first());
}
template <> __declspec(section ".text.model_late_owners")
const void* shared_ptr_deleter<nw4r::g3d::AnmObjMatClrRes, void (*)(nw4r::g3d::G3dObj*)>::get_deleter(const std::type_info& rType) const {
    return rType == typeid(void (*)(nw4r::g3d::G3dObj*)) ? &mStorage.second() : nullptr;
}
template <> __declspec(section ".text.model_late_owners")
void shared_ptr_deleter<nw4r::g3d::AnmObjMatClrRes, void (*)(nw4r::g3d::G3dObj*)>::dispose() {
    mStorage.second()(mStorage.first());
}

template <> __declspec(section ".text.model_late_holders")
shared_ptr_holder<nw4r::g3d::AnmObjShpRes, void (*)(nw4r::g3d::G3dObj*)>::~shared_ptr_holder() {
    if (ptr_.first()) {
        ptr_.second()(ptr_.first());
    }
}
template <> __declspec(section ".text.model_late_holders")
shared_ptr_holder<nw4r::g3d::AnmObjTexPatRes, void (*)(nw4r::g3d::G3dObj*)>::~shared_ptr_holder() {
    if (ptr_.first()) {
        ptr_.second()(ptr_.first());
    }
}
template <> __declspec(section ".text.model_late_holders")
shared_ptr_holder<nw4r::g3d::AnmObjMatClrRes, void (*)(nw4r::g3d::G3dObj*)>::~shared_ptr_holder() {
    if (ptr_.first()) {
        ptr_.second()(ptr_.first());
    }
}
template <> __declspec(section ".text.model_mat_deleter")
shared_ptr_deleter<nw4r::g3d::AnmObjMatClrRes, void (*)(nw4r::g3d::G3dObj*)>::~shared_ptr_deleter() {}
template <> __declspec(section ".text.model_tex_deleter")
shared_ptr_deleter<nw4r::g3d::AnmObjTexPatRes, void (*)(nw4r::g3d::G3dObj*)>::~shared_ptr_deleter() {}
template <> __declspec(section ".text.model_shp_deleter")
shared_ptr_deleter<nw4r::g3d::AnmObjShpRes, void (*)(nw4r::g3d::G3dObj*)>::~shared_ptr_deleter() {}
}
}
template <> __declspec(section ".text.model_buffers")
__vec_deleter<tr1::shared_ptr<CFileData>, allocator<tr1::shared_ptr<CFileData> >&>::~__vec_deleter() {
    if (data_) {
        clear();
        alloc().deallocate(data_, cap());
    }
}
template <> __declspec(section ".text.model_buffers")
__vec_deleter<tr1::shared_ptr<nw4r::g3d::AnmObj>, allocator<tr1::shared_ptr<nw4r::g3d::AnmObj> >&>::~__vec_deleter() {
    if (data_) {
        clear();
        alloc().deallocate(data_, cap());
    }
}
}
