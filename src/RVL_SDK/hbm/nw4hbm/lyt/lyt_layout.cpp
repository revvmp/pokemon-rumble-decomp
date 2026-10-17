#include "nw4hbm/lyt/lyt_layout.h"
#include "nw4hbm/lyt/lyt_animation.h"
#include "nw4hbm/lyt/lyt_bounding.h"
#include "nw4hbm/lyt/lyt_common.h"
#include "nw4hbm/lyt/lyt_group.h"
#include "nw4hbm/lyt/lyt_pane.h"
#include "nw4hbm/lyt/lyt_picture.h"
#include "nw4hbm/lyt/lyt_resources.h"
#include "nw4hbm/lyt/lyt_textBox.h"
#include "nw4hbm/lyt/lyt_types.h"
#include "nw4hbm/lyt/lyt_window.h"

inline void* operator new(size_t, void* p) { return p; }

namespace nw4hbm {
namespace lyt {
namespace {

void SetTagProcessorImpl(Pane* pPane, ut::WideTagProcessor* pProcessor) {
    if (TextBox* pTextBox = ut::DynamicCast<TextBox*>(pPane)) {
        pTextBox->SetTagProcessor(pProcessor);
    }

    for (Pane::PaneList::Iterator it = pPane->GetChildList().GetBeginIter(); it != pPane->GetChildList().GetEndIter();
         ++it) {
        SetTagProcessorImpl(&*it, pProcessor);
    }
}

template <typename T>
T* CreateObject() {
    void* pMem = Layout::AllocMemory(sizeof(T));

    if (pMem) {
        return new (pMem) T();
    }

    return nullptr;
}

template <typename T, typename TParam>
T* CreateObject(TParam param) {
    void* pMem = Layout::AllocMemory(sizeof(T));

    if (pMem) {
        return new (pMem) T(param);
    }

    return nullptr;
}

template <typename T, typename TParam1, typename TParam2>
T* CreateObject(TParam1 param1, TParam2 param2) {
    void* pMem = Layout::AllocMemory(sizeof(T));

    if (pMem) {
        return new (pMem) T(param1, param2);
    }

    return nullptr;
}

}

MEMAllocator* Layout::mspAllocator;

Layout::Layout() : mpRootPane(nullptr), mpGroupContainer(nullptr), mLayoutSize(0.0f, 0.0f), mOriginType(ORIGINTYPE_TOPLEFT) {}

Layout::~Layout() {
    if (mpGroupContainer) {
        mpGroupContainer->~GroupContainer();
        FreeMemory(mpGroupContainer);
    }

    if (mpRootPane && !mpRootPane->IsUserAllocated()) {
        mpRootPane->~Pane();
        FreeMemory(mpRootPane);
    }

    for (AnimTransformList::Iterator it = mAnimTransList.GetBeginIter(); it != mAnimTransList.GetEndIter();) {
        AnimTransformList::Iterator currIt = it++;
        mAnimTransList.Erase(currIt);
        currIt->~AnimTransform();
        FreeMemory(&*currIt);
    }
}

bool Layout::Build(const void* pLytBinary, ResourceAccessor* pAccessor) {
    const res::BinaryFileHeader* pFileHead = static_cast<const res::BinaryFileHeader*>(pLytBinary);

    if (!detail::TestFileHeader(*pFileHead, SIGNATURE)) {
        return false;
    }

    ResBlockSet resBlockSet = {};
    resBlockSet.pResAccessor = pAccessor;

    Pane* pParentPane = nullptr;
    Pane* pLastPane = nullptr;
    bool bReadRootGroup = false;
    int groupNestLevel = 0;

    const void* pData = detail::ConvertOffsToPtr<void>(pLytBinary, pFileHead->headerSize);

    for (int i = 0; i < pFileHead->dataBlocks; i++) {
        const res::DataBlockHeader* pDataBlockHead = static_cast<const res::DataBlockHeader*>(pData);

        switch (detail::GetSignatureInt(pDataBlockHead->kind)) {
        case res::Layout::SIGNATURE: {
            const res::Layout* pResLyt = static_cast<const res::Layout*>(pData);
            mOriginType = pResLyt->originType != ORIGINTYPE_TOPLEFT ? ORIGINTYPE_CENTER : ORIGINTYPE_TOPLEFT;
            mLayoutSize = pResLyt->layoutSize;
            break;
        }

        case SIGNATURE_TEXTURELIST:
            resBlockSet.pTextureList = static_cast<const res::TextureList*>(pData);
            break;

        case SIGNATURE_FONTLIST:
            resBlockSet.pFontList = static_cast<const res::FontList*>(pData);
            break;

        case SIGNATURE_MATERIALLIST:
            resBlockSet.pMaterialList = static_cast<const res::MaterialList*>(pData);
            break;

        case res::Pane::SIGNATURE:
        case res::Bounding::SIGNATURE:
        case res::Picture::SIGNATURE:
        case res::TextBox::SIGNATURE:
        case res::Window::SIGNATURE: {
            Pane* pPane = BuildPaneObj(detail::GetSignatureInt(pDataBlockHead->kind), pData, resBlockSet);

            if (pPane) {
                if (mpRootPane == nullptr) {
                    mpRootPane = pPane;
                }

                if (pParentPane) {
                    pParentPane->AppendChild(pPane);
                }

                pLastPane = pPane;
            }
            break;
        }

        case SIGNATURE_PANESTART:
            pParentPane = pLastPane;
            break;

        case SIGNATURE_PANEEND:
            pLastPane = pParentPane;
            pParentPane = pLastPane->GetParent();
            break;

        case res::Group::SIGNATURE:
            if (!bReadRootGroup) {
                bReadRootGroup = true;
                mpGroupContainer = CreateObject<GroupContainer>();
            } else if (mpGroupContainer && groupNestLevel == 1) {
                Group* pGroup = CreateObject<Group>(reinterpret_cast<const res::Group*>(pDataBlockHead), mpRootPane);

                if (pGroup) {
                    mpGroupContainer->AppendGroup(pGroup);
                }
            }
            break;

        case SIGNATURE_GROUPSTART:
            groupNestLevel++;
            break;

        case SIGNATURE_GROUPEND:
            groupNestLevel--;
            break;
        }

        pData = detail::ConvertOffsToPtr<void>(pData, pDataBlockHead->size);
    }

    return true;
}

AnimTransform* Layout::CreateAnimTransform(const void* pAnmBinary, ResourceAccessor* pAccessor) {
    const res::BinaryFileHeader* pFileHead = static_cast<const res::BinaryFileHeader*>(pAnmBinary);

    if (!detail::TestFileHeader(*pFileHead)) {
        return nullptr;
    }

    const res::DataBlockHeader* pDataBlockHead =
        detail::ConvertOffsToPtr<res::DataBlockHeader>(pFileHead, pFileHead->headerSize);

    AnimTransform* pAnimTrans = nullptr;

    for (int i = 0; i < pFileHead->dataBlocks; i++) {
        switch (detail::GetSignatureInt(pDataBlockHead->kind)) {
        case SIGNATURE_ANIMATIONINFO:
            switch (detail::GetSignatureInt(pFileHead->signature)) {
            case SIGNATURE_ANIMATION:
            case res::AnimationInfo::SIGNATURE_ANMPANESRT:
            case res::AnimationInfo::SIGNATURE_ANMPANEVIS:
            case res::AnimationInfo::SIGNATURE_ANMVTXCLR:
            case res::AnimationInfo::SIGNATURE_ANMMATCLR:
            case res::AnimationInfo::SIGNATURE_ANMTEXSRT:
            case res::AnimationInfo::SIGNATURE_ANMTEXPAT:
                AnimTransformBasic* pAnimTransBasic = CreateObject<AnimTransformBasic>();

                if (pAnimTransBasic) {
                    pAnimTransBasic->SetResource(reinterpret_cast<const res::AnimationBlock*>(pDataBlockHead),
                                                 pAccessor);
                    pAnimTrans = pAnimTransBasic;
                }
                break;
            }

            if (pAnimTrans) {
                mAnimTransList.PushBack(pAnimTrans);
            }
            break;
        }

        pDataBlockHead = detail::ConvertOffsToPtr<res::DataBlockHeader>(pDataBlockHead, pDataBlockHead->size);
    }

    return pAnimTrans;
}

void Layout::BindAnimation(AnimTransform* pAnimTrans) {
    if (mpRootPane) {
        mpRootPane->BindAnimation(pAnimTrans, true);
    }
}

void Layout::UnbindAnimation(AnimTransform* pAnimTrans) {
    if (mpRootPane) {
        mpRootPane->UnbindAnimation(pAnimTrans, true);
    }
}

void Layout::UnbindAllAnimation() {
    UnbindAnimation(nullptr);
}

void Layout::SetAnimationEnable(AnimTransform* pAnimTrans, bool enable) {
    if (mpRootPane) {
        mpRootPane->SetAnimationEnable(pAnimTrans, enable, true);
    }
}

void Layout::CalculateMtx(const DrawInfo& rInfo) {
    if (mpRootPane) {
        mpRootPane->CalculateMtx(rInfo);
    }
}

void Layout::Draw(const DrawInfo& rInfo) {
    if (mpRootPane) {
        mpRootPane->Draw(rInfo);
    }
}

void Layout::Animate(u32 option) {
    if (mpRootPane) {
        mpRootPane->Animate(option);
    }
}

ut::Rect Layout::GetLayoutRect() const {
    if (mOriginType == ORIGINTYPE_CENTER) {
        return ut::Rect(-mLayoutSize.width / 2.0f, mLayoutSize.height / 2.0f, mLayoutSize.width / 2.0f,
                        -mLayoutSize.height / 2.0f);
    }

    return ut::Rect(0.0f, 0.0f, mLayoutSize.width, mLayoutSize.height);
}

void Layout::SetTagProcessor(ut::WideTagProcessor* pProcessor) {
    SetTagProcessorImpl(mpRootPane, pProcessor);
}

Pane* Layout::BuildPaneObj(s32 kind, const void* pBinary, const ResBlockSet& rBlockSet) {
    switch (kind) {
    case res::Pane::SIGNATURE: {
        const res::Pane* pResPane = static_cast<const res::Pane*>(pBinary);
        Pane* pPane = CreateObject<Pane>(pResPane);
        return pPane;
    }

    case res::Picture::SIGNATURE: {
        const res::Picture* pResPic = static_cast<const res::Picture*>(pBinary);
        Pane* pPane = CreateObject<Picture>(pResPic, rBlockSet);
        return pPane;
    }

    case res::TextBox::SIGNATURE: {
        const res::TextBox* pResTextBox = static_cast<const res::TextBox*>(pBinary);
        Pane* pPane = CreateObject<TextBox>(pResTextBox, rBlockSet);
        return pPane;
    }

    case res::Window::SIGNATURE: {
        const res::Window* pResWnd = static_cast<const res::Window*>(pBinary);
        Pane* pPane = CreateObject<Window>(pResWnd, rBlockSet);
        return pPane;
    }

    case res::Bounding::SIGNATURE: {
        const res::Bounding* pResBounding = static_cast<const res::Bounding*>(pBinary);
        Pane* pPane = CreateObject<Bounding>(pResBounding, rBlockSet);
        return pPane;
    }

    default:
        return nullptr;
    }
}

}
}
