#include <nw4r/ut/ut_algorithm.h>
#include <nw4r/lyt/lyt_layout.h>
#include <nw4r/lyt/lyt_animation.h>
#include <nw4r/lyt/lyt_bounding.h>
#include <nw4r/lyt/lyt_group.h>
#include <nw4r/lyt/lyt_picture.h>
#include <nw4r/lyt/lyt_textBox.h>
#include <nw4r/lyt/lyt_utils.h>
#include <nw4r/lyt/lyt_window.h>
#include <nw4r/ut/ut_RuntimeTypeInfo.h>
#include <nw4r/ut/ut_TagProcessorBase.h>
namespace nw4r {

namespace lyt {

MEMAllocator *Layout::mspAllocator;

namespace {

void SetTagProcessorImpl(Pane *pPane, ut::TagProcessorBase<wchar_t> *pTagProcessor) {
    TextBox *pTextBox = ut::DynamicCast<TextBox *>(pPane);
    if (pTextBox) {
        pTextBox->SetTagProcessor(pTagProcessor);
    }
    for (ut::LinkList<Pane, 4>::Iterator it = pPane->GetChildList()->GetBeginIter();
         it != pPane->GetChildList()->GetEndIter(); ++it) {
        SetTagProcessorImpl(&*it, pTagProcessor);
    }
}

bool IsIncludeAnimationGroupRef(
    GroupContainer *pGroupContainer, const AnimationGroupRef *groupRefs, u16 bindGroupNum, bool bDescendingBind,
    Pane *pTargetPane
) {
    for (u16 grpIdx = 0; grpIdx < bindGroupNum; grpIdx++) {
        Group *pGroup = pGroupContainer->FindGroupByName(groupRefs[grpIdx].GetName());
        for (ut::LinkList<detail::PaneLink, 0>::Iterator paneList = pGroup->GetPaneList()->GetBeginIter();
             paneList != pGroup->GetPaneList()->GetEndIter(); paneList++) {
            const Pane *t = paneList->mTarget;
            if (t == pTargetPane) {
                return true;
            }
            if (bDescendingBind) {
                for (const Pane *pParentPane = pTargetPane->GetParent(); pParentPane != nullptr;
                     pParentPane = pParentPane->GetParent()) {
                    if (t == pParentPane) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

}

Layout::Layout() : mAnimTransList(), mpRootPane(nullptr), mpGroupContainer(nullptr) {}

Layout::~Layout() {
    DeleteObj(mpGroupContainer);
    if (mpRootPane && !mpRootPane->IsUserAllocated()) {
        DeleteObj(mpRootPane);
    }
    ut::LinkList<AnimTransform, 4>::Iterator it = mAnimTransList.GetBeginIter();
    while (it != mAnimTransList.GetEndIter()) {
        ut::LinkList<AnimTransform, 4>::Iterator currIt = it++;
        mAnimTransList.Erase(currIt);
        DeleteObj(&*currIt);
    }
}

bool Layout::Build(const void *lytResBuf, ResourceAccessor *pResAcsr) {
    const res::BinaryFileHeader *pFileHead = (const res::BinaryFileHeader *)(lytResBuf);
    if (!detail::TestFileHeader(*pFileHead, 'RLYT')) {
        return false;
    }
    if (!detail::TestFileVersion(*pFileHead)) {
        return false;
    }

    ResBlockSet resBlockSet = {nullptr, nullptr, nullptr, pResAcsr};
    Pane *pParentPane = nullptr;
    Pane *pLastPane = nullptr;
    bool bReadRootGroup = false;
    int groupNestLevel = 0;

    const void *dataPtr = ((u8 *)lytResBuf + pFileHead->headerSize);
    for (int i = 0; i < pFileHead->dataBlocks; i++) {
        const res::DataBlockHeader *pDataBlockHead = (const res::DataBlockHeader *)dataPtr;
        switch (pDataBlockHead->kind) {
            case 'lyt1':
            {
                const res::Layout *pResLyt = ((const res::Layout *)dataPtr);
                mLayoutSize = pResLyt->layoutSize;
            } break;
            case 'txl1':
                resBlockSet.pTextureList = (const res::TextureList *)dataPtr;
                break;
            case 'fnl1':
                resBlockSet.pFontList = (const res::FontList *)dataPtr;
                break;
            case 'mat1':
                resBlockSet.pMaterialList = (const res::MaterialList *)dataPtr;
                break;
            case 'wnd1':
            case 'pan1':
            case 'pic1':
            case 'txt1':
            case 'bnd1':
            {
                Pane *pPane = BuildPaneObj(pDataBlockHead->kind, dataPtr, resBlockSet);
                if (pPane) {
                    if (mpRootPane == nullptr) {
                        mpRootPane = pPane;
                    }
                    if (pParentPane) {
                        pParentPane->AppendChild(pPane);
                    }
                    pLastPane = pPane;
                }
            } break;
            case 'usd1':
                pLastPane->SetExtUserDataList((const res::ExtUserDataList *)dataPtr);
                break;
            case 'pas1':
                pParentPane = pLastPane;
                break;
            case 'pae1':
                pLastPane = pParentPane;
                pParentPane = pLastPane->GetParent();
                break;
            case 'grp1':
                if (!bReadRootGroup) {
                    bReadRootGroup = true;
                    mpGroupContainer = NewObj<GroupContainer>();
                } else {
                    if (mpGroupContainer && groupNestLevel == 1) {
                        Group *pGroup = NewObj<Group>((const res::Group *)dataPtr, mpRootPane);
                        if (pGroup) {
                            mpGroupContainer->AppendGroup(pGroup);
                        }
                    }
                }
                break;
            case 'grs1':
                groupNestLevel++;
                break;
            case 'gre1':
                groupNestLevel--;
                break;
            default: break;
        }
        dataPtr = ((u8 *)dataPtr + pDataBlockHead->size);
    }
    return true;
}

AnimTransform *Layout::CreateAnimTransform() {
    AnimTransformBasic *pAnimTrans = NewObj<AnimTransformBasic>();
    if (pAnimTrans) {
        mAnimTransList.PushBack(pAnimTrans);
    }
    return pAnimTrans;
}

AnimTransform *Layout::CreateAnimTransform(const void *animResBuf, ResourceAccessor *pResAcsr) {
    return CreateAnimTransform(AnimResource(animResBuf), pResAcsr);
}

AnimTransform *Layout::CreateAnimTransform(const AnimResource &animRes, ResourceAccessor *pResAcsr) {
    const res::AnimationBlock *pAnimBlock = animRes.GetResourceBlock();
    if (!pAnimBlock) {
        return nullptr;
    }

    AnimTransform *pAnimTrans = CreateAnimTransform();
    if (pAnimTrans) {
        pAnimTrans->SetResource(pAnimBlock, pResAcsr);
    }

    return pAnimTrans;
}

void Layout::BindAnimation(AnimTransform *pAnimTrans) {
    if (!mpRootPane) {
        return;
    }
    mpRootPane->BindAnimation(pAnimTrans, true, false);
}

void Layout::UnbindAnimation(AnimTransform *pAnimTrans) {
    if (!mpRootPane) {
        return;
    }
    mpRootPane->UnbindAnimation(pAnimTrans, true);
}

void Layout::UnbindAllAnimation() {
    UnbindAnimation(nullptr);
}

bool Layout::BindAnimationAuto(const AnimResource &animRes, ResourceAccessor *pResAcsr) {
    if (!mpRootPane) {
        return false;
    }
    if (!animRes.GetResourceBlock()) {
        return false;
    }

    AnimTransform *pAnimTrans = CreateAnimTransform();
    u16 bindGroupNum = animRes.GetGroupNum();
    u16 animNum = 0;
    if (bindGroupNum == 0) {
        pAnimTrans->SetResource(animRes.GetResourceBlock(), pResAcsr, animRes.GetResourceBlock()->animContNum);
        mpRootPane->BindAnimation(pAnimTrans, true, true);
    } else {
        const AnimationGroupRef *groupRefs = animRes.GetGroupArray();
        for (int grpIdx = 0; grpIdx < bindGroupNum; grpIdx++) {
            Group *pGroup = mpGroupContainer->FindGroupByName(groupRefs[grpIdx].GetName());
            animNum += animRes.CalcAnimationNum(pGroup, animRes.IsDescendingBind());
        }
        pAnimTrans->SetResource(animRes.GetResourceBlock(), pResAcsr, animNum);
        for (int grpIdx = 0; grpIdx < bindGroupNum; grpIdx++) {
            Group *pGroup = mpGroupContainer->FindGroupByName(groupRefs[grpIdx].GetName());
            lyt::BindAnimation(pGroup, pAnimTrans, animRes.IsDescendingBind(), true);
        }
    }
    u16 animSharInfoNum = animRes.GetAnimationShareInfoNum();
    if (animSharInfoNum != 0) {
        const AnimationShareInfo *animSharInfoAry = animRes.GetAnimationShareInfoArray();
        for (int i = 0; i < animSharInfoNum; i++) {
            Pane *pSrcPane = mpRootPane->FindPaneByName(animSharInfoAry[i].GetSrcPaneName(), true);
            detail::AnimPaneTree animPaneTree = detail::AnimPaneTree(pSrcPane, animRes);
            if (animPaneTree.GetLinkNum()) {
                Group *pGroup = mpGroupContainer->FindGroupByName(animSharInfoAry[i].GetTargetGroupName());
                ut::LinkList<detail::PaneLink, 0> *paneList = pGroup->GetPaneList();
                for (ut::LinkList<detail::PaneLink, 0>::Iterator it = paneList->GetBeginIter();
                     it != paneList->GetEndIter(); it++) {
                    if (it->mTarget != pSrcPane) {
                        if (bindGroupNum != 0) {
                            bool bInclude = IsIncludeAnimationGroupRef(
                                mpGroupContainer, animRes.GetGroupArray(), bindGroupNum, animRes.IsDescendingBind(),
                                it->mTarget
                            );
                            if (!bInclude) {
                                continue;
                            }
                        }
                        animPaneTree.Bind(this, it->mTarget, pResAcsr);
                    }
                }
            }
        }
    }
    return true;
}

void Layout::SetAnimationEnable(AnimTransform *pAnimTrans, bool bEnable) {
    if (!mpRootPane) {
        return;
    }
    mpRootPane->SetAnimationEnable(pAnimTrans, bEnable, true);
}

void Layout::CalculateMtx(const DrawInfo &drawInfo) {
    if (!mpRootPane) {
        return;
    }
    mpRootPane->CalculateMtx(drawInfo);
}

void Layout::Draw(const DrawInfo &drawInfo) {
    if (!mpRootPane) {
        return;
    }
    mpRootPane->Draw(drawInfo);
}

void Layout::Animate(u32 option) {
    if (!mpRootPane) {
        return;
    }
    mpRootPane->Animate(option);
}

ut::Rect Layout::GetLayoutRect() const {
    return ut::Rect(-mLayoutSize.width / 2, mLayoutSize.height / 2, mLayoutSize.width / 2, -mLayoutSize.height / 2);
}

void Layout::SetTagProcessor(ut::TagProcessorBase<wchar_t> *pTagProcessor) {
    SetTagProcessorImpl(this->mpRootPane, pTagProcessor);
}

Pane *Layout::BuildPaneObj(s32 kind, const void *dataPtr, const ResBlockSet &resBlockSet) {
    switch (kind) {
        case 'pan1': {
            const res::Pane *pResPane = (const res::Pane *)dataPtr;
            return NewObj<Pane>(pResPane);
        } break;
        case 'pic1': {
            const res::Picture *pResPic = (const res::Picture *)dataPtr;
            return NewObj<Picture>(pResPic, resBlockSet);
        } break;
        case 'txt1': {
            const res::TextBox *pBlock = (const res::TextBox *)dataPtr;
            return NewObj<TextBox>(pBlock, resBlockSet);
        } break;
        case 'wnd1': {
            const res::Window *pBlock = (const res::Window *)dataPtr;
            return NewObj<Window>(pBlock, resBlockSet);
        } break;
        case 'bnd1': {
            const res::Bounding *pResBounding = (const res::Bounding *)dataPtr;
            return NewObj<Bounding>(pResBounding, resBlockSet);
        } break;
        default: return nullptr;
    }
}

}

}
