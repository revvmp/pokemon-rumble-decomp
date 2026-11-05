#include <algorithm>
#include <game/pokemon/CPii.h>
#include <game/graphics/ScnMdlSimpleDataInline.h>

#include <game/ui/CHPBar.h>
#include <game/ui/CMiniMapMarker.h>
#include <game/script/CPpuEngine.h>
#include <game/graphics/CModel.h>
#include <game/world/BWorld.h>
#include <game/core/CFiber.h>
#include <game/pokemon/CPiiPersonalData.h>
#include <game/pokemon/CPiiBox.h>
#include <game/pokemon/COyaInfo.h>
#include <game/graphics/VideoTiming.h>
#include <game/animation/AnmObjChrRootScale.h>
#include <game/io/CFileData.h>
#include <game/core/FrameStep.h>
#include <game/core/Memory.h>
#include <nw4r/g3d/g3d_anmtexsrt.h>
#include <revolution/gx/GXTexture.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/g3d/res/g3d_resfile.h>

typedef std::__vec_deleter<SD_PiiPersonalData::Data,
                          std::allocator<SD_PiiPersonalData::Data> > PiiSnapshotStorage;

inline void AllocatePiiSnapshot(PiiSnapshotStorage& rData, unsigned long count) {
    rData.allocate(count, PiiSnapshotStorage::pod_type());
}

inline void ConstructPiiSnapshot(PiiSnapshotStorage& rData,
                                const SD_PiiPersonalData::Data* first,
                                const SD_PiiPersonalData::Data* last) {
    rData.construct_at_end(first, last, PiiSnapshotStorage::pod_type());
}

inline void CopyPiiSnapshot(PiiSnapshotStorage& rData, const PiiSnapshotStorage& rOther) {
    struct Range {
        const SD_PiiPersonalData::Data* first;
        const SD_PiiPersonalData::Data* last;
    };
    Range range;
    range.first = rOther.data_;
    range.last = rOther.data_ + rOther.size_;
    unsigned long count = range.last - range.first;
    if (count != 0) {
        AllocatePiiSnapshot(rData, count);
        ConstructPiiSnapshot(rData, range.first, range.last);
    }
}

namespace std {
template <> inline
__vec_constructor<SD_PiiPersonalData::Data, allocator<SD_PiiPersonalData::Data>, true>::
__vec_constructor(const __vec_constructor& rOther) {
    CopyPiiSnapshot(*this, rOther);
}

template <> template <> inline void
__vec_constructor<SD_PiiPersonalData::Data, allocator<SD_PiiPersonalData::Data>, true>::
assign<const SD_PiiPersonalData::Data*>(const SD_PiiPersonalData::Data* first,
                                      const SD_PiiPersonalData::Data* last,
                                      Metrowerks::int2type<false>) {
    size_type capacity;
    size_type n = std::distance(first, last);
    struct Capacity {
        size_type value;
    };
    Capacity currentCapacity;
    currentCapacity.value = this->cap();
    if (n <= currentCapacity.value || grow_by(n - currentCapacity.value, capacity)) {
        const SD_PiiPersonalData::Data* mid(first);
        std::advance(mid, std::min(n, this->size_));
        copy_range(first, mid, this->data_);
        if (n < this->size_) {
            this->erase_at_end(this->size_ - n);
        } else if (this->size_ < n) {
            construct_at_end(mid, last);
        }
    } else {
        assign_reallocate(n, capacity);
        construct_at_end(first, last);
    }
}

template <> inline
__vec_constructor<SD_PiiPersonalData::Data, allocator<SD_PiiPersonalData::Data>, true>&
__vec_constructor<SD_PiiPersonalData::Data, allocator<SD_PiiPersonalData::Data>, true>::
operator=(const __vec_constructor& rOther) {
    if (this != &rOther) {
        struct Range {
            const SD_PiiPersonalData::Data* first;
            const SD_PiiPersonalData::Data* last;
        };
        Range range;
        range.first = rOther.data_;
        range.last = rOther.data_ + rOther.size_;
        assign(range.first, range.last, pod_type());
    }
    return *this;
}
}

CPiiBox::CPiiBox(int type, int capacity) : mType(type), mCapacity(capacity) {}

CPiiBox::~CPiiBox() {}

bool CPiiBox::AddPersonalData(std::tr1::shared_ptr<CPiiPersonalData> ppd)
{
    if (ppd) {
        ppd->SetInTeam(false);
        ppd->SetInWiiRemote(false);
        switch (mType) {
        case 1:
            ppd->SetInTeam(true);
            break;
        case 2:
            ppd->SetInWiiRemote(true);
            break;
        }
    }
    RemoveItem(ppd);
    if (mPersonalDataList.size() + 1 > mCapacity) {
        return false;
    }
    mPersonalDataList.push_front(ppd);
    return true;
}

bool CPiiBox::RemovePersonalData(std::tr1::shared_ptr<CPiiPersonalData> ppd)
{
    return RemoveItem(ppd);
}

bool CPiiBox::RemoveItem(std::tr1::shared_ptr<CPiiPersonalData> ppd)
{
    typedef std::list<std::tr1::shared_ptr<CPiiPersonalData> >::iterator Iterator;
    for (Iterator it = mPersonalDataList.begin(); it != mPersonalDataList.end(); ++it) {
        if (it->get() == ppd.get()) {
            mPersonalDataList.erase(it);
            return true;
        }
    }
    return false;
}

void CPiiBox::Clear()
{
    mPersonalDataList.clear();
}

void CPiiBox::RestoreAll()
{
    typedef std::list<std::tr1::shared_ptr<CPiiPersonalData> >::iterator Iterator;
    for (Iterator it = mPersonalDataList.begin(); it != mPersonalDataList.end(); ++it) {
        (*it)->FillUpHP();
    }
}

void CPiiBox::ShowAllSpecs()
{
    typedef std::list<std::tr1::shared_ptr<CPiiPersonalData> >::iterator Iterator;
    for (Iterator it = mPersonalDataList.begin(); it != mPersonalDataList.end(); ++it) {
        (*it)->SetSpecHidden(false);
    }
}

void CPiiBox::ExportPersonalData(std::vector<SD_PiiPersonalData::Data>& rOut)
{
    typedef std::list<std::tr1::shared_ptr<CPiiPersonalData> >::iterator Iterator;
    std::vector<SD_PiiPersonalData::Data> records;
    records.reserve(mPersonalDataList.size());
    for (Iterator it = mPersonalDataList.begin(); it != mPersonalDataList.end(); ++it) {
        SD_PiiPersonalData::Data data;
        (*it)->Export(data);
        records.push_back(data);
    }
    std::swap(rOut, records);
}

std::tr1::shared_ptr<CPiiBox> CPiiBox::Create(const std::vector<SD_PiiPersonalData::Data>& rData, int type, int capacity, const COyaInfo* pOyaInfo)
{
    typedef std::list<std::tr1::shared_ptr<CPiiPersonalData> >::iterator Iterator;
    std::tr1::shared_ptr<CPiiBox> box(new CPiiBox(type, capacity));
    typedef std::reverse_iterator<std::vector<SD_PiiPersonalData::Data>::const_iterator> DataIterator;
    for (DataIterator it = rData.rbegin(); it != rData.rend(); ++it) {
        std::tr1::shared_ptr<CPiiPersonalData> ppd = CreatePiiPersonalData(pOyaInfo, *it);
        if (!ppd) {
            return std::tr1::shared_ptr<CPiiBox>();
        }
        box->AddPersonalData(ppd);
    }
    int serial = 1;
    typedef std::reverse_iterator<Iterator> ListIterator;
    for (ListIterator it(box->GetPersonalDataList().end()); it != ListIterator(box->GetPersonalDataList().begin()); ++it) {
        if ((*it)->GetCreationTime() == 0) {
            (*it)->SetCreationTime(serial);
            ++serial;
        }
    }
    return box;
}

void CPiiBox::ApplyEnvironment(bool enable, const std::tr1::shared_ptr<CPpuEnv>& rEnvironment)
{
    typedef std::list<std::tr1::shared_ptr<CPiiPersonalData> >::iterator Iterator;
    for (Iterator it = mPersonalDataList.begin(); it != mPersonalDataList.end(); ++it) {
        (*it)->ExecuteInitScript(enable, rEnvironment);
    }
}


std::tr1::bad_function_call* CreateMissingMotionCallbackError() {
    return new std::tr1::bad_function_call;
}
