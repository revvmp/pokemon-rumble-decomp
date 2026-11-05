#include <MSL_CPP/alloc_ptr.h>
#include <vector>
namespace NPpu { class CCode; }
typedef char ScriptCodeReferenceStorage[sizeof(std::tr1::shared_ptr<NPpu::CCode>)];

#include <nw4r/ut/ut_algorithm.h>
#include <game/script/CPpuEngine.h>
#include <nw4r/g3d/res/g3d_resanmchr.h>
#include <nw4r/g3d/res/g3d_resfile.h>
#include <game/world/BWorld.h>
#include <game/world/BWorldObj.h>
#include <game/io/CFileData.h>
#include <game/world/IGameMap.h>
#include <game/session/IGameSession.h>
#include <game/pokemon/COyaInfo.h>
#include <game/pokemon/CPii.h>
#include <game/pokemon/CPiiPersonalData.h>
#include <string>
#include <string.h>
#include <game/script/CPpuEnv.h>

void YieldFiber();
std::tr1::shared_ptr<NPpu::CCode> LoadScriptCode(NPpu::CEnvironment* pEnv, const char* pName, int wait);

std::tr1::weak_ptr<CPpuEnv> CPpuEngine::sDefaultEnv;
MapIndex CPpuEngine::sDefaultStageDescTarget;
std::tr1::shared_ptr<std::string> CPpuEngine::sScriptRoot;

static bool sTemporaryIDReady;

static void InitTemporaryID() {
    if (!sTemporaryIDReady) {
        sTemporaryIDReady = true;
        NPpu::CEngine::SetNextID(CPpuEngine::TEMPORARY_ID_BASE);
    }
}

CPpuEngine::CPpuEngine() : mFlags(0), mOwnerObj(nullptr), mStageDescTarget(sDefaultStageDescTarget), mCachedPPDValue(0) {}

CPpuEngine::~CPpuEngine() {}

#pragma push
#pragma scheduling off
BWorld* CPpuEngine::GetWorld() const {
    if (mOwnerObj) {
        return mOwnerObj->GetWorld();
    }
    return GetCurrentGameMap()->GetWorld();
}
#pragma pop

NPpu::CEnvironment* CPpuEngine::GetEnv() const {
    return GetEnvironment();
}

void CPpuEngine::SetDefaultEnvironment(const std::tr1::shared_ptr<CPpuEnv>& rEnv) {
    sDefaultEnv = rEnv;
}

void CPpuEngine::SetStageDescTarget(const MapIndex& rTarget) {
    mStageDescTarget = rTarget;
}

void CPpuEngine::SetDefaultStageDescTarget(const MapIndex& rTarget) {
    sDefaultStageDescTarget = rTarget;
}

static inline u32 GetNodeCount(const nw4r::g3d::ResAnmChr& rAnm) {
    return rAnm.ofs_to_obj<nw4r::g3d::ResDic>(rAnm.ref().toChrDataDic).GetNumData();
}

std::tr1::shared_ptr<CFileData> CPpuEngine::LoadFile(const char* pName, bool background) {
    std::tr1::shared_ptr<CFileData> file = LoadResourceFile(pName, 0, background);
    if (!file || !file->IsLoaded()) {
        return std::tr1::shared_ptr<CFileData>();
    }
    mFiles.insert(file);
    return file;
}

bool CPpuEngine::LoadPlacement(const char* pFileName, const char* pAnmName, const char* pNodePrefix, bool background) {
    std::tr1::shared_ptr<CFileData> file = LoadResourceFile(pFileName, 0, background);
    if (!file || !file->IsLoaded()) {
        return false;
    }
    const nw4r::g3d::ResAnmChr anm = nw4r::g3d::ResFile(file->GetData()).GetResAnmChr(pAnmName);
    if (!anm.IsValid()) {
        return false;
    }
    size_t prefixLength = strlen(pNodePrefix);
    u32 nodeCount = GetNodeCount(anm);
    std::vector<nw4r::math::MTX34> placements;
    placements.reserve(nodeCount);
    for (u32 i = 0; i < nodeCount; i++) {
        const nw4r::g3d::ResAnmChrNodeData& rNode = *anm.GetNodeAnm(i);
        if (strncmp(reinterpret_cast<const char*>(rNode.name + reinterpret_cast<u32>(&rNode)), pNodePrefix, prefixLength) == 0) {
            nw4r::g3d::ChrAnmResult result;
            anm.GetAnmResult(&result, i, 0.0f);
            nw4r::math::MTX34 mtx;
            result.GetMtx(&mtx);
            placements.push_back(mtx);
        }
    }
    mPlacements.swap(placements);
    return true;
}

void CPpuEngine::SetPPD(std::tr1::shared_ptr<CPiiPersonalData> ppd) {
    mPPD = ppd;
}

void CPpuEngine::LeaveInitPart() {
    mFlags |= FLAG_LEFT_INIT_PART;
}

const char* CPpuEngine::GetScriptRoot() {
    bool probe = false;
    if (probe) sScriptRoot.~shared_ptr();
    if (sScriptRoot) {
        return sScriptRoot->c_str();
    }
    return "5:";
}

std::tr1::shared_ptr<CFileData> CPpuEngine::LoadScriptFile(const char* pName, u32 flags) {
    std::tr1::shared_ptr<CFileData> file = LoadResourceFile((std::string("script.arc/") + pName).c_str(), 0, flags, GetScriptRoot());
    if (file && file->IsLoaded()) {
        return file;
    }
    return std::tr1::shared_ptr<CFileData>();
}

bool CPpuEngine::Load(const char* pName, int mode, const std::tr1::shared_ptr<CPpuEnv>& rEnv) {
    std::tr1::shared_ptr<CPpuEnv> env(rEnv ? rEnv : sDefaultEnv.lock());
    if (!env) {
        return false;
    }
    std::tr1::shared_ptr<NPpu::CCode> code = LoadScriptCode(env.get(), pName, mode);
    if (!code) {
        return false;
    }
    mStageDescTarget = sDefaultStageDescTarget;
    Init(code.get(), env.get(), FRAME_CAPACITY, BASE_COUNT, HANDLER_CAPACITY);
    return true;
}

bool CPpuEngine::Load(BWorldObj* pOwner, const char* pName, int mode, const std::tr1::shared_ptr<CPpuEnv>& rEnv) {
    SetID(pOwner->GetId());
    if (!Load(pName, mode, rEnv)) {
        return false;
    }
    mOwnerObj = pOwner;
    NPpu::CValue owner;
    owner.mInt = pOwner->GetId();
    owner.mType = NPpu::CValue::TYPE_INT;
    SetThis(owner);
    Calc();
    return true;
}

bool CPpuEngine::Execute(const char* pName, int mode, const std::tr1::shared_ptr<CPpuEnv>& rEnv) {
    InitTemporaryID();
    if (!Load(pName, mode, rEnv)) {
        return false;
    }
    while (true) {
        Calc();
        switch (GetState()) {
        case STATE_NONE:
        case STATE_IDLE:
        case STATE_END:
            return false;
        case STATE_WAIT:
            if (mode == 1) {
                YieldFiber();
            }
            break;
        case STATE_READY:
            return true;
        }
    }
}

void CPpuEngine::Calc() {
    if (GetState() == STATE_NONE) {
        return;
    }
    while (true) {
        switch (Run()) {
        case STATE_WAIT:
            if (mFlags & FLAG_LEFT_INIT_PART) {
                return;
            }
            break;
        case STATE_NONE:
        case STATE_IDLE:
        case STATE_READY:
        case STATE_END:
            if (mOwnerObj != nullptr) {
                std::tr1::shared_ptr<BWorldObj> owner(mOwnerObj->shared_from_this());
                BWorld* pWorld = mOwnerObj->GetWorld();
                if (pWorld != nullptr) {
                    pWorld->RemoveObject(owner);
                }
                mOwnerObj = nullptr;
            }
            return;
        }
    }
}

BWorldObj* CPpuEngine::PopWorldObj() {
    u32 id = PopInt();
    if (id & 0x80000000) {
        return nullptr;
    }
    if (mOwnerObj != nullptr && id == mOwnerObj->GetId()) {
        return mOwnerObj;
    }
    return GetWorld()->GetObject(id);
}

CEffect* CPpuEngine::PopEffect() {
    u32 id = PopInt();
    return GetWorld()->GetEffect(id);
}

const PiiSpeciesData* CPpuEngine::PopSpeciesData() {
    int id = PopInt();
    if (id == 0) {
        return nullptr;
    }
    return reinterpret_cast<const PiiSpeciesData*>(id);
}

std::tr1::shared_ptr<CPiiPersonalData> CPpuEngine::PopPersonalData() {
    int key = GetArgInt(0);
    if (key == mCachedPPDValue) {
        Drop(1);
        return mCachedPPD.lock();
    }
    std::tr1::shared_ptr<CPiiPersonalData> personalData;
    if (key & 0x80000000) {
        Drop(1);
        personalData = IGameSession::GetInstance()->GetOyaInfo()->FindPersonalData(key & 0x7FFFFFFF);
    } else {
        CPii* pPii = DynamicCast<CPii>(PopWorldObj());
        if (pPii) {
            personalData = pPii->GetPersonalData();
        }
    }
    mCachedPPDValue = key;
    mCachedPPD = personalData;
    return personalData;
}

u32 CPpuEngine::PeekWorldObjType(int argIndex) {
    NPpu::CValue value;
    GetArg(argIndex, value);
    u32 id = value.ToInt();
    if (id != 0) {
        BWorldObj* pObj = GetWorld()->GetObject(id);
        if (pObj != nullptr) {
            return pObj->GetType();
        }
    }
    return 0;
}

template <>
inline std::list<int>& std::detail::map_base<int, std::list<int>, std::less<int>, std::allocator<std::pair<const int, std::list<int> > > >::operator[](const int& rKey) {
    return tree_.find_or_insert(rKey).second;
}

int CPpuEngine::InsertCallback(int messageID, int functionID, int position) {
    if (messageID < 0) {
        return 0;
    }
    if (functionID == 0) {
        return 0;
    }
    std::list<int>& rCallbacks = mCallbacks[messageID];
    if (position == 0) {
        rCallbacks.push_front(functionID);
    } else if (position == -1) {
        rCallbacks.push_back(functionID);
    } else {
        std::list<int>::iterator it;
        for (it = rCallbacks.begin(); it != rCallbacks.end(); ++it) {
            if (position == *it) {
                break;
            }
        }
        rCallbacks.insert(it, functionID);
    }
    return 1;
}

int CPpuEngine::EraseCallback(int messageID, int functionID) {
    if (messageID < 0) {
        return 0;
    }
    std::map<int, std::list<int> >::iterator it = mCallbacks.find(messageID);
    if (it == mCallbacks.end()) {
        return 0;
    }
    if (functionID == 0) {
        mCallbacks.erase(it);
        return 1;
    }
    std::list<int>& rCallbacks = it->second;
    for (std::list<int>::iterator callback = rCallbacks.begin(); callback != rCallbacks.end(); ++callback) {
        if (functionID == *callback) {
            rCallbacks.erase(callback);
            if (rCallbacks.empty()) {
                mCallbacks.erase(it);
            }
            return 1;
        }
    }
    return 0;
}

bool CPpuEngine::HasCallback(int messageID) const {
    return mCallbacks.find(messageID) != mCallbacks.end();
}

namespace {

class HandlerIterator {
public:
    typedef int difference_type;
    typedef int value_type;
    typedef int* pointer;
    typedef int& reference;
    typedef std::bidirectional_iterator_tag iterator_category;

    struct Identity {
        int operator()(int value) const { return value; }
    };
    explicit HandlerIterator(std::list<int>::iterator it, Identity policy = Identity()) : mIterator(it) {}
    int operator*() const { return mPolicy(*mIterator); }
    HandlerIterator& operator++() {
        typedef std::__list_deleter<int, std::allocator<int> >::node* NodePointer;
        NodePointer volatile& rNode = *reinterpret_cast<NodePointer volatile*>(&mIterator);
        rNode = rNode->next_;
        return *this;
    }
    bool operator!=(const HandlerIterator& rOther) const { return mIterator != rOther.mIterator; }

private:
    std::list<int>::iterator mIterator;
    Identity mPolicy;
};

inline void AllocateHandlers(std::__vec_deleter<int, std::allocator<int> >& rStorage, unsigned long count) {
    rStorage.allocate(count, std::__vec_deleter<int, std::allocator<int> >::pod_type());
}
}

namespace std {

template <> template <>
void __vec_constructor<int, allocator<int>, true>::construct_at_end(HandlerIterator first, HandlerIterator last, Metrowerks::int2type<false>) {
    int* p = this->data_ + this->size_;
    for (; first != last; ++p) {
        volatile char constructionTag = 0;
        const int& value = *first;
        this->alloc().construct(p, value);
        ++first;
        ++this->size_;
    }
}
template <> template <>
void __vec_constructor<int, allocator<int>, true>::construct_at_end(HandlerIterator first, HandlerIterator last) {
    construct_at_end(first, last, Metrowerks::int2type<false>());
}
template <> template <>
void vector<int>::assign(HandlerIterator first, HandlerIterator last, Metrowerks::restrict_to<true>::type*) {
    size_type n = std::distance(first, last);
    if (n != 0) {
        AllocateHandlers(*this, n);
        base::construct_at_end(first, last);
    }
}
}

namespace {

struct HandlerList : public std::vector<int> {
    HandlerList(std::list<int>::iterator first, std::list<int>::iterator last, HandlerIterator::Identity& firstPolicy, HandlerIterator::Identity& lastPolicy) : std::vector<int>() {
        *reinterpret_cast<char*>(&lastPolicy) = 0;
        *reinterpret_cast<char*>(&firstPolicy) = 0;
        assign(HandlerIterator(first, firstPolicy), HandlerIterator(last, lastPolicy));
    }
};

inline bool CallHandler(CPpuEngine& rEngine, int functionID, const CPpuEngine::Message& rMessage) {
    NPpu::CValue args[3];
    args[0].SetInt(rMessage.mSender);
    args[1].SetInt(rMessage.mMessageID);
    args[2] = rMessage.mArgument;
    NPpu::CValue result;
    rEngine.Call(&result, functionID, 3, args);
    return result.ToInt() != 0;
}

}

#pragma push
#pragma opt_loop_invariants off
int CPpuEngine::SendMessage(const Message& rMessage) {
    std::map<int, std::list<int> >::iterator it = mCallbacks.find(rMessage.mMessageID);
    if (it == mCallbacks.end()) {
        return 0;
    }
    std::list<int>& rCallbacks = it->second;
    if (rCallbacks.size() == 1) {
        CallHandler(*this, rCallbacks.front(), rMessage);
    } else if (rCallbacks.size() < 16) {
        int functions[16];
        std::copy(rCallbacks.begin(), rCallbacks.end(), functions);
        for (u32 i = 0; i < rCallbacks.size(); i++) {
            if (CallHandler(*this, functions[i], rMessage)) {
                break;
            }
        }
    } else {
        HandlerIterator::Identity firstPolicy;
        HandlerIterator::Identity lastPolicy;
        HandlerList functions(rCallbacks.begin(), rCallbacks.end(), firstPolicy, lastPolicy);
        for (std::vector<int>::iterator fn = functions.begin(); fn != functions.end(); ++fn) {
            if (CallHandler(*this, *fn, rMessage)) {
                break;
            }
        }
    }
    return 1;
}
#pragma pop

bool RunScript(const char* pName, int mode, const std::tr1::shared_ptr<CPpuEnv>& rEnvironment) {
    std::tr1::shared_ptr<CPpuEngine> engine(new CPpuEngine);
    return engine->Execute(pName, mode, rEnvironment);
}
