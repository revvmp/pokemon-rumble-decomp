#include <MSL_CPP/functional_base.h>

#pragma push
#pragma dont_inline on
namespace std {
template <> struct less<int> : binary_function<int, int, bool> {
    less() {}
    bool operator()(const int& a, const int& b) const { return a < b; }
};
}
#pragma pop

#include <game/BModelObj.h>
#include <game/BWorld.h>
#include <game/CDividedGround.h>
#include <game/CPpuEngine.h>
#include <math.h>
#include <game/CBSPTree.h>
#include <game/CItem.h>
#include <game/CPlayer.h>
#include <game/CPiiPersonalData.h>
#include <game/CPlayerPii.h>
#include <game/IGameSession.h>
#include <game/MathUtil.h>
#include <nw4r/math/math_triangular.h>
#include <game/CEnemyPii.h>
#include <game/CPii.h>
#include <algorithm>
#include <list>
#include <set>

#pragma dont_inline on

static inline bool IsPiiForDynamicCast(const BWorldObj* pObject) {
    return pObject != nullptr && (pObject->GetType() & CPii::TYPE);
}

template <> inline CPii* DynamicCast<CPii, BWorldObj>(BWorldObj* pObject) {
    return IsPiiForDynamicCast(pObject) ? static_cast<CPii*>(pObject) : nullptr;
}


static inline nw4r::math::VEC3& Scale(nw4r::math::VEC3& rVector, f32 scale) {
    nw4r::math::VEC3Scale(&rVector, &rVector, scale);
    return rVector;
}

int method_alive(NPpu::CEngine* pEngine) {
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    pEngine->PushInt(pObject ? pObject->GetId() : 0);
    return 0;
}

int method_destroy(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    BWorldObj* pObject = pEngine->PopWorldObj();
    if (pObject != nullptr) {
        if (pObject == pEngine->GetOwner()) {
            pEngine->PushInt(1);
            return 3;
        }
        std::tr1::shared_ptr<BWorldObj> object = pObject->shared_from_this();
        BWorld* pWorld = object->GetWorld();
        pWorld->RemoveObject(object);
        pEngine->PushInt(1);
        return 0;
    }
    pEngine->PushInt(0);
    return 0;
}

int method_getDirDeg(NPpu::CEngine* pEngine) {
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    pEngine->PushFloat(pObject ? pObject->GetRotation() : 0.0f);
    return 0;
}

int method_setDirDeg(NPpu::CEngine* pEngine) {
    f32 rotation = pEngine->PopFloat();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        pObject->SetRotation(rotation);
    }
    pEngine->PushNull();
    return 0;
}

int method_getModelScaleX(NPpu::CEngine* pEngine) {
    BModelObj* pModel = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    pEngine->PushFloat(pModel ? pModel->GetModelScale().x : 1.0f);
    return 0;
}

int method_getModelScaleY(NPpu::CEngine* pEngine) {
    BModelObj* pModel = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    pEngine->PushFloat(pModel ? pModel->GetModelScale().y : 1.0f);
    return 0;
}

int method_getModelScaleZ(NPpu::CEngine* pEngine) {
    BModelObj* pModel = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    pEngine->PushFloat(pModel ? pModel->GetModelScale().z : 1.0f);
    return 0;
}

int method_setModelScale(NPpu::CEngine* pEngine) {
    f32 scale = pEngine->PopFloat();
    BModelObj* pModel = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pModel != nullptr) {
        pModel->SetModelScale(nw4r::math::VEC3(scale, scale, scale));
    }
    pEngine->PushNull();
    return 0;
}

int method_setModelScaleXYZ(NPpu::CEngine* pEngine) {
    f32 z = pEngine->PopFloat();
    f32 y = pEngine->PopFloat();
    f32 x = pEngine->PopFloat();
    BModelObj* pModel = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pModel != nullptr) {
        pModel->SetModelScale(nw4r::math::VEC3(x, y, z));
    }
    pEngine->PushNull();
    return 0;
}

int method_setCollType(NPpu::CEngine* pEngine) {
    int type = pEngine->PopInt();
    BModelObj* pModel = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pModel != nullptr) {
        pModel->SetSizeMode(type);
    }
    pEngine->PushNull();
    return 0;
}

int method_getCollType(NPpu::CEngine* pEngine) {
    BModelObj* pModel = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pModel != nullptr) {
        pEngine->PushInt(pModel->IsSizeMode());
    } else {
        pEngine->PushInt(0);
    }
    return 0;
}

int method_getCollRadiusZXByType(NPpu::CEngine* pEngine) {
    int type = pEngine->PopInt();
    BModelObj* pModel = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pModel != nullptr) {
        pEngine->PushFloat(pModel->CalcSize(type).x);
    } else {
        pEngine->PushFloat(0.0f);
    }
    return 0;
}

int method_getCollRadiusY(NPpu::CEngine* pEngine) {
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    pEngine->PushFloat(pObject ? pObject->GetSize().y : 0.0f);
    return 0;
}

int method_getCollRadiusZX(NPpu::CEngine* pEngine) {
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    pEngine->PushFloat(pObject ? pObject->GetSize().x : 0.0f);
    return 0;
}

int method_moveTo(NPpu::CEngine* pEngine) {
    f32 direction = pEngine->PopFloat();
    f32 distance = pEngine->PopFloat();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    bool moved = false;
    if (pObject != nullptr) {
        f32 sine;
        f32 cosine;
        nw4r::math::SinCosFIdx(&sine, &cosine, NW4R_MATH_DEG_TO_FIDX(direction));
        nw4r::math::VEC3 step(sine, 0.0f, cosine);
        Scale(step, distance);
        nw4r::math::VEC3 position = pObject->GetPositionRef() + step;
        CDividedGround* pGround = DynamicCast<CDividedGround>(static_cast<CPpuEngine*>(pEngine)->GetWorld()->GetGround());
        if (pGround == nullptr || !pGround->IsOutOfBounds(position)) {
            BModelObj* pModel = DynamicCast<BModelObj>(pObject);
            if (pModel != nullptr) {
                pModel->SetTransform(position, step);
                moved = true;
            } else {
                pObject->SetPosition(position);
                moved = true;
            }
        }
    }
    pEngine->PushInt(moved);
    return 0;
}

static bool KeepOutOfWall(const CBSPTree* pTree, nw4r::math::VEC3& rPosition, const nw4r::math::VEC3& rOrigin,
                          const nw4r::math::VEC3& rDirection, f32 radius) {
    CBSPTree::Trace trace;
    trace.mStart.x = rOrigin.x - 0.3f * rDirection.x;
    trace.mStart.y = 1.0f;
    trace.mStart.z = rOrigin.z - 0.3f * rDirection.z;
    trace.mDirection = rDirection;
    if (!pTree->TraceSegment(trace, 0.0f, 0.3f + 2.0f * (0.05f + radius))) {
        return true;
    }
    f32 distance = trace.mpNode->CalcDistance(rPosition);
    if (0.05f <= distance) {
        return true;
    }
    rPosition = rPosition + trace.mpNode->mNormal * (0.05f - distance);
    return false;
}

int method_moveAlongWall(NPpu::CEngine* pEngine) {
    CDividedGround* pGround = DynamicCast<CDividedGround>(static_cast<CPpuEngine*>(pEngine)->GetWorld()->GetGround());
    CBSPTree* pTree = nullptr;
    if (pGround != nullptr) {
        pTree = pGround->GetBSPTree(1);
    }
    if (pTree == nullptr) {
        return method_moveTo(pEngine);
    }
    f32 direction = pEngine->PopFloat();
    f32 distance = pEngine->PopFloat();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    bool blocked = true;
    if (pObject != nullptr) {
        f32 sine;
        f32 cosine;
        nw4r::math::SinCosFIdx(&sine, &cosine, NW4R_MATH_DEG_TO_FIDX(direction));
        nw4r::math::VEC3 position = pObject->GetPositionRef();
        CBSPTree::Trace trace;
        f32 remaining = 0.3f + distance;
        trace.mStart.x = position.x - 0.3f * sine;
        trace.mStart.y = 1.0f;
        trace.mStart.z = position.z - 0.3f * cosine;
        trace.mDirection.x = sine;
        trace.mDirection.y = 0.0f;
        trace.mDirection.z = cosine;
        for (int i = 0; i < 4; i++) {
            if (!pTree->TraceSegment(trace, 0.0f, 0.05f + remaining)) {
                trace.mStart = trace.mStart + trace.mDirection * remaining;
                blocked = false;
                break;
            }
            f32 hit = trace.mFraction - 0.05f;
            if (remaining < hit) {
                trace.mStart = trace.mStart + trace.mDirection * remaining;
                blocked = false;
                break;
            }
            if (0.0f < hit) {
                remaining -= hit;
            }
            trace.mStart = trace.mStart + trace.mDirection * hit;
            if (remaining < 0.01f) {
                break;
            }
            f32 along = trace.mDirection.Dot(trace.mpNode->mNormal);
            trace.mDirection = trace.mDirection - trace.mpNode->mNormal * along;
            if (trace.mDirection.x == 0.0f && trace.mDirection.y == 0.0f && trace.mDirection.z == 0.0f) {
                break;
            }
            if (!SafeNormalize(&trace.mDirection, &trace.mDirection)) {
                break;
            }
        }
        nw4r::math::VEC3 target(trace.mStart.x, position.y, trace.mStart.z);
        if (!KeepOutOfWall(pTree, target, position, nw4r::math::VEC3(-cosine, 0.0f, sine), distance)) {
            blocked = true;
        }
        if (!KeepOutOfWall(pTree, target, position, nw4r::math::VEC3(cosine, 0.0f, -sine), distance)) {
            blocked = true;
        }
        target.y = position.y;
        BModelObj* pModel = DynamicCast<BModelObj>(pObject);
        if (pModel != nullptr) {
            pModel->SetTransform(target, target - pObject->GetPositionRef());
        } else {
            pObject->SetPosition(target);
        }
    }
    pEngine->PushInt(!blocked);
    return 0;
}

int method_moveByWall(NPpu::CEngine* pEngine) {
    CDividedGround* pGround = DynamicCast<CDividedGround>(static_cast<CPpuEngine*>(pEngine)->GetWorld()->GetGround());
    CBSPTree* pTree = nullptr;
    if (pGround != nullptr) {
        pTree = pGround->GetBSPTree(1);
    }
    if (pTree == nullptr) {
        return method_moveTo(pEngine);
    }
    f32 direction = pEngine->PopFloat();
    f32 distance = pEngine->PopFloat();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    bool blocked = true;
    if (pObject != nullptr) {
        f32 sine;
        f32 cosine;
        nw4r::math::SinCosFIdx(&sine, &cosine, NW4R_MATH_DEG_TO_FIDX(direction));
        nw4r::math::VEC3 position = pObject->GetPositionRef();
        CBSPTree::Trace trace;
        f32 remaining = 0.3f + distance;
        trace.mStart.x = position.x - 0.3f * sine;
        trace.mStart.y = 1.0f;
        trace.mStart.z = position.z - 0.3f * cosine;
        trace.mDirection.x = sine;
        trace.mDirection.y = 0.0f;
        trace.mDirection.z = cosine;
        if (!pTree->TraceSegment(trace, 0.0f, 0.05f + remaining)) {
            trace.mStart = trace.mStart + trace.mDirection * remaining;
            blocked = false;
        } else {
            trace.mStart = trace.mStart + trace.mDirection * (trace.mFraction - 0.05f);
            blocked = true;
        }
        nw4r::math::VEC3 target(trace.mStart.x, position.y, trace.mStart.z);
        if (!KeepOutOfWall(pTree, target, target, nw4r::math::VEC3(-cosine, 0.0f, sine), distance)) {
            blocked = true;
        }
        if (!KeepOutOfWall(pTree, target, target, nw4r::math::VEC3(cosine, 0.0f, -sine), distance)) {
            blocked = true;
        }
        target.y = position.y;
        BModelObj* pModel = DynamicCast<BModelObj>(pObject);
        if (pModel != nullptr) {
            pModel->SetTransform(target, target - pObject->GetPositionRef());
        } else {
            pObject->SetPosition(target);
        }
    }
    pEngine->PushInt(!blocked);
    return 0;
}

int method_moveForceTo(NPpu::CEngine* pEngine) {
    f32 direction = pEngine->PopFloat();
    f32 distance = pEngine->PopFloat();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        f32 sine;
        f32 cosine;
        nw4r::math::SinCosFIdx(&sine, &cosine, NW4R_MATH_DEG_TO_FIDX(direction));
        nw4r::math::VEC3 step(sine, 0.0f, cosine);
        Scale(step, distance);
        nw4r::math::VEC3 position = pObject->GetPositionRef() + step;
        BModelObj* pModel = DynamicCast<BModelObj>(pObject);
        if (pModel != nullptr) {
            pModel->SetTransform(position, step);
        } else {
            pObject->SetPosition(position);
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_warpTo(NPpu::CEngine* pEngine) {
    f32 direction = pEngine->PopFloat();
    f32 distance = pEngine->PopFloat();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        f32 sine;
        f32 cosine;
        nw4r::math::SinCosFIdx(&sine, &cosine, NW4R_MATH_DEG_TO_FIDX(direction));
        nw4r::math::VEC3 step(sine, 0.0f, cosine);
        Scale(step, distance);
        nw4r::math::VEC3 position = pObject->GetPositionRef() + step;
        pObject->SetPosition(position);
    }
    pEngine->PushNull();
    return 0;
}

int method_getX(NPpu::CEngine* pEngine) {
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    pEngine->PushFloat(pObject ? pObject->GetPositionRef().x : 0.0f);
    return 0;
}

int method_getY(NPpu::CEngine* pEngine) {
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    pEngine->PushFloat(pObject ? pObject->GetPositionRef().y : 0.0f);
    return 0;
}

int method_getZ(NPpu::CEngine* pEngine) {
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    pEngine->PushFloat(pObject ? pObject->GetPositionRef().z : 0.0f);
    return 0;
}

int method_setZX(NPpu::CEngine* pEngine) {
    f32 x = pEngine->PopFloat();
    f32 z = pEngine->PopFloat();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        nw4r::math::VEC3 position;
        position.x = x;
        position.y = pObject->GetPositionRef().y;
        position.z = z;
        pObject->SetPosition(position);
    }
    pEngine->PushNull();
    return 0;
}

int method_setXYZ(NPpu::CEngine* pEngine) {
    f32 z = pEngine->PopFloat();
    f32 y = pEngine->PopFloat();
    f32 x = pEngine->PopFloat();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        pObject->SetPosition(nw4r::math::VEC3(x, y, z));
    }
    pEngine->PushNull();
    return 0;
}

int method_show(NPpu::CEngine* pEngine) {
    int visible = pEngine->PopInt();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        pObject->SetVisible(visible != 0);
    }
    pEngine->PushNull();
    return 0;
}

static f32 CalcDistanceSq(const nw4r::math::VEC2& rFrom, const nw4r::math::VEC2& rTo) {
    nw4r::math::VEC2 delta;
    return nw4r::math::VEC2LenSq(nw4r::math::VEC2Sub(&delta, &rFrom, &rTo));
}

static f32 CalcPlanarDistanceSq(const nw4r::math::VEC2& rFrom, const nw4r::math::VEC2& rTo) {
    return CalcDistanceSq(rFrom, rTo);
}

static f32 CalcDistanceSqZX(const nw4r::math::VEC3& rFrom, const nw4r::math::VEC3& rTo) {
    return CalcPlanarDistanceSq(nw4r::math::VEC2(rFrom.z, rFrom.x), nw4r::math::VEC2(rTo.z, rTo.x));
}

int method_findNearestPlayerPiiZX(NPpu::CEngine* pEngine) {
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        f32 nearestDistance = 0.0f;
        CPlayerPii* pNearest = nullptr;
        for (int i = 0; i < 4; i++) {
            CPlayerPii* pPii = IGameSession::GetInstance()->GetPlayer(i)->GetPii();
            if (pPii != nullptr) {
                f32 distance = CalcDistanceSqZX(pPii->GetPositionRef(), pObject->GetPositionRef());
                if (pNearest == nullptr || (pNearest != nullptr && nearestDistance > distance)) {
                    pNearest = pPii;
                    nearestDistance = distance;
                }
            }
        }
        if (pNearest != nullptr) {
            pEngine->PushInt(pNearest->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_findNearestLivingPlayerPiiZX(NPpu::CEngine* pEngine) {
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        f32 nearestDistance = 0.0f;
        CPlayerPii* pNearest = nullptr;
        for (int i = 0; i < 4; i++) {
            CPlayerPii* pPii = IGameSession::GetInstance()->GetPlayer(i)->GetPii();
            if (pPii != nullptr) {
                f32 distance = CalcDistanceSqZX(pPii->GetPositionRef(), pObject->GetPositionRef());
                if (pPii->GetPersonalData()->GetHP() > 0 && (pNearest == nullptr || (pNearest != nullptr && nearestDistance > distance))) {
                    pNearest = pPii;
                    nearestDistance = distance;
                }
            }
        }
        if (pNearest != nullptr) {
            pEngine->PushInt(pNearest->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_findNearestObj(NPpu::CEngine* pEngine) {
    int type = pEngine->PopInt();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        BWorldObj* pNearest = static_cast<CPpuEngine*>(pEngine)->GetWorld()->FindNearestObject(type, pObject);
        if (pNearest != nullptr) {
            pEngine->PushInt(pNearest->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_findNearestDummyZX(NPpu::CEngine* pEngine) {
    NPpu::CValue key;
    pEngine->Pop(key);
    int index = pEngine->PopInt();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        std::vector<BWorldObj*> objects = static_cast<CPpuEngine*>(pEngine)->GetWorld()->GetObjects(0x800);
        f32 nearestDistance = 0.0f;
        BWorldObj* pNearest = nullptr;
        for (std::vector<BWorldObj*>::iterator it = objects.begin(); it != objects.end(); ++it) {
            BWorldObj* pDummy = DynamicCast<CItem>(*it);
            if (pDummy != nullptr) {
                NPpu::CValue value = pDummy->GetRegister(index);
                if (value.mType == key.mType) {
                    f32 distance = CalcDistanceSqZX(pDummy->GetPositionRef(), pObject->GetPositionRef());
                    if (pNearest == nullptr || (pNearest != nullptr && nearestDistance > distance)) {
                        pNearest = pDummy;
                        nearestDistance = distance;
                    }
                }
            }
        }
        if (pNearest != nullptr) {
            pEngine->PushInt(pNearest->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_findMostVitalPiiInCircleZX(NPpu::CEngine* pEngine) {
    f32 radius = pEngine->PopFloat();
    int type = pEngine->PopInt();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        std::list<BWorldObj*> candidates =
            static_cast<CPpuEngine*>(pEngine)->GetWorld()->FindObjectsInRing(type, pObject->GetPositionRef(), 0.0f, radius);
        CPii* pMostVital = nullptr;
        for (std::list<BWorldObj*>::iterator it = candidates.begin(); it != candidates.end(); ++it) {
            if (pObject == *it) {
                continue;
            }
            CPii* pPii = DynamicCast<CPii>(*it);
            if (pPii == nullptr) {
                continue;
            }
            if (pPii->GetPersonalData()->GetHP() <= 0) {
                continue;
            }
            if (pMostVital == nullptr ||
                pMostVital->GetPersonalData()->GetHP() < pPii->GetPersonalData()->GetHP()) {
                pMostVital = pPii;
            }
        }
        if (pMostVital != nullptr) {
            pEngine->PushInt(pMostVital->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_findMostVitalPiiInArcZX(NPpu::CEngine* pEngine) {
    f32 width = pEngine->PopFloat();
    f32 direction = pEngine->PopFloat();
    f32 innerRadius = pEngine->PopFloat();
    f32 outerRadius = pEngine->PopFloat();
    int type = pEngine->PopInt();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        f32 halfWidth = width / 2.0f;
        f32 startAngle = direction - halfWidth;
        f32 endAngle = direction + halfWidth;
        std::list<BWorldObj*> candidates = static_cast<CPpuEngine*>(pEngine)->GetWorld()->FindObjectsInSector(
            type, pObject->GetPositionRef(), outerRadius, innerRadius, startAngle, endAngle);
        CPii* pMostVital = nullptr;
        for (std::list<BWorldObj*>::iterator it = candidates.begin(); it != candidates.end(); ++it) {
            if (pObject == *it) {
                continue;
            }
            CPii* pPii = DynamicCast<CPii>(*it);
            if (pPii == nullptr) {
                continue;
            }
            if (pPii->GetPersonalData()->GetHP() <= 0) {
                continue;
            }
            if (pMostVital == nullptr ||
                pMostVital->GetPersonalData()->GetHP() < pPii->GetPersonalData()->GetHP()) {
                pMostVital = pPii;
            }
        }
        if (pMostVital != nullptr) {
            pEngine->PushInt(pMostVital->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

static f32 CalcDegreesZX(f32 x, f32 z) {
    return NW4R_MATH_FIDX_TO_DEG(nw4r::math::Atan2FIdx(x, z));
}

int method_getDirDegToObjZX(NPpu::CEngine* pEngine) {
    BWorldObj* pTarget = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pTarget != nullptr && pObject != nullptr) {
        const nw4r::math::VEC3& rTarget = pTarget->GetPositionRef();
        const nw4r::math::VEC3& rObject = pObject->GetPositionRef();
        f32 z = rTarget.z - rObject.z;
        f32 x = rTarget.x - rObject.x;
        pEngine->PushFloat(NormalizeSignedAngle(CalcDegreesZX(x, z)));
    } else {
        pEngine->PushFloat(0.0f);
    }
    return 0;
}

int method_getDirDegToZX(NPpu::CEngine* pEngine) {
    f32 x = pEngine->PopFloat();
    f32 z = pEngine->PopFloat();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        const nw4r::math::VEC3& rPosition = pObject->GetPositionRef();
        f32 dz = z - rPosition.z;
        f32 dx = x - rPosition.x;
        pEngine->PushFloat(NormalizeSignedAngle(CalcDegreesZX(dx, dz)));
    } else {
        pEngine->PushFloat(0.0f);
    }
    return 0;
}

int method_getDistanceSqToObjZX(NPpu::CEngine* pEngine) {
    BWorldObj* pTarget = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr && pTarget != nullptr) {
        pEngine->PushFloat(CalcDistanceSqZX(pObject->GetPositionRef(), pTarget->GetPositionRef()));
    } else {
        pEngine->PushFloat(0.0f);
    }
    return 0;
}

int method_getDistanceSqToZX(NPpu::CEngine* pEngine) {
    f32 x = pEngine->PopFloat();
    f32 z = pEngine->PopFloat();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        pEngine->PushFloat(CalcDistanceSqZX(pObject->GetPositionRef(), nw4r::math::VEC3(x, 0.0f, z)));
    } else {
        pEngine->PushFloat(0.0f);
    }
    return 0;
}

typedef int (*FoundFunc)(NPpu::CEngine*, NPpu::Slice*, int, std::list<BWorldObj*>&);
typedef int (*NoneFunc)(NPpu::CEngine*, NPpu::Slice*, int);

int method_setOutlineType(NPpu::CEngine* pEngine) {
    int type = pEngine->PopInt();
    BModelObj* pModel = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pModel != nullptr) {
        pModel->SetOutlineType(type);
    }
    pEngine->PushNull();
    return 0;
}

int method_getOutlineType(NPpu::CEngine* pEngine) {
    BModelObj* pModel = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pModel != nullptr) {
        pEngine->PushInt(pModel->GetOutlineType());
    } else {
        pEngine->PushInt(0);
    }
    return 0;
}

int method_isInKeepOutArea(NPpu::CEngine* pEngine) {
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    bool keepOut = true;
    if (pObject != nullptr) {
        nw4r::math::VEC3 position = pObject->GetPositionRef();
        CDividedGround* pGround = DynamicCast<CDividedGround>(static_cast<CPpuEngine*>(pEngine)->GetWorld()->GetGround());
        if (pGround != nullptr && !pGround->IsOutOfBounds(position)) {
            keepOut = false;
        }
    }
    pEngine->PushInt(keepOut);
    return 0;
}

int method_isCPii(NPpu::CEngine* pEngine) {
    pEngine->PushInt(IsKindOf<CPii>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj()));
    return 0;
}

int method_isCPlayerPii(NPpu::CEngine* pEngine) {
    pEngine->PushInt(IsKindOf<CPlayerPii>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj()));
    return 0;
}

int method_isCEnemyPii(NPpu::CEngine* pEngine) {
    pEngine->PushInt(IsKindOf<CEnemyPii>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj()));
    return 0;
}

static int PushObjectIds(NPpu::CEngine* pEngine, NPpu::Slice* pSlices, int capacity, std::list<BWorldObj*>& rObjects) {
    int i = 0;
    int count = std::min(capacity - 1, static_cast<int>(rObjects.size()));
    const std::list<BWorldObj*>& rList = rObjects;
    std::list<BWorldObj*>::const_iterator it = rList.begin();
    for (; i < count; ++it, ++i) {
        pSlices[i].SetInt((*it)->GetId());
    }
    if (count >= 0) {
        pSlices[count].SetInt(0);
    }
    pEngine->PushInt(count);
    return 0;
}

static int MergeObjectIds(NPpu::CEngine* pEngine, NPpu::Slice* pSlices, int capacity, std::list<BWorldObj*>& rObjects) {
    std::set<int> ids;
    for (int i = 0; i < capacity; ++i) {
        int id = pSlices[i].ToInt();
        if (id == 0) {
            break;
        }
        ids.insert(id);
    }
    const std::list<BWorldObj*>& rList = rObjects;
    for (std::list<BWorldObj*>::const_iterator it = rList.begin(); it != rList.end(); ++it) {
        ids.insert((*it)->GetId());
    }
    int i = 0;
    int count = std::min(capacity - 1, static_cast<int>(ids.size()));
    std::set<int>::iterator it = ids.begin();
    for (; i < count; ++it, ++i) {
        pSlices[i].SetInt(*it);
    }
    if (count >= 0) {
        pSlices[count].SetInt(0);
    }
    pEngine->PushInt(count);
    return 0;
}

static int PushNoObjects(NPpu::CEngine* pEngine, NPpu::Slice* pSlices, int capacity) {
    if (capacity > 0) {
        pSlices[0].SetInt(0);
    }
    pEngine->PushInt(0);
    return 0;
}

static int CountMergedIds(NPpu::CEngine* pEngine, NPpu::Slice* pSlices, int capacity) {
    NPpu::Slice* pSlice = pSlices;
    int count = 0;
    for (; count < capacity; ++pSlice, ++count) {
        if (pSlice->ToInt() == 0) {
            break;
        }
    }
    if (count == capacity && count > 0) {
        pSlices[count - 1].SetInt(0);
    }
    pEngine->PushInt(count);
    return 0;
}

static int PushObjectCount(NPpu::CEngine* pEngine, NPpu::Slice*, int, std::list<BWorldObj*>& rObjects) {
    pEngine->PushInt(rObjects.size());
    return 0;
}

static int PushZeroCount(NPpu::CEngine* pEngine, NPpu::Slice*, int) {
    pEngine->PushInt(0);
    return 0;
}

template <int Mode> static int SearchArc(NPpu::CEngine* pBase, FoundFunc pFound, NoneFunc pNone) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    f32 width = pEngine->PopFloat();
    f32 outerRadius = pEngine->PopFloat();
    f32 innerRadius = pEngine->PopFloat();
    int type = pEngine->PopInt();
    int capacity = 0;
    NPpu::Slice* pSlices = pEngine->PopArray(&capacity);
    BWorldObj* pObject = pEngine->PopWorldObj();
    if (pObject != nullptr) {
        f32 rotation = pObject->GetRotation();
        f32 halfWidth = width / 2.0f;
        f32 start = rotation - halfWidth;
        f32 end = rotation + halfWidth;
        std::list<BWorldObj*> objects = pEngine->GetWorld()->FindObjectsInSector(type, pObject->GetPositionRef(), innerRadius, outerRadius, start, end);
        return pFound(pEngine, pSlices, capacity, objects);
    }
    return pNone(pEngine, pSlices, capacity);
}

static int CountArc(NPpu::CEngine* pBase, FoundFunc pFound, NoneFunc pNone) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    f32 width = pEngine->PopFloat();
    f32 outerRadius = pEngine->PopFloat();
    f32 innerRadius = pEngine->PopFloat();
    int type = pEngine->PopInt();
    BWorldObj* pObject = pEngine->PopWorldObj();
    if (pObject != nullptr) {
        f32 rotation = pObject->GetRotation();
        f32 halfWidth = width / 2.0f;
        f32 start = rotation - halfWidth;
        f32 end = rotation + halfWidth;
        std::list<BWorldObj*> objects = pEngine->GetWorld()->FindObjectsInSector(type, pObject->GetPositionRef(), innerRadius, outerRadius, start, end);
        return pFound(pEngine, nullptr, 0, objects);
    }
    return pNone(pEngine, nullptr, 0);
}

int method_findObjInArcZX(NPpu::CEngine* pEngine) {
    return SearchArc<0>(pEngine, PushObjectIds, PushNoObjects);
}

int method_mergeFindObjInArcZX(NPpu::CEngine* pEngine) {
    return SearchArc<1>(pEngine, MergeObjectIds, CountMergedIds);
}

int method_countObjInArcZX(NPpu::CEngine* pEngine) {
    return CountArc(pEngine, PushObjectCount, PushZeroCount);
}

template <int Mode> static int SearchCircle(NPpu::CEngine* pBase, FoundFunc pFound, NoneFunc pNone) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    f32 outerRadius = pEngine->PopFloat();
    f32 innerRadius = pEngine->PopFloat();
    int type = pEngine->PopInt();
    int capacity = 0;
    NPpu::Slice* pSlices = pEngine->PopArray(&capacity);
    BWorldObj* pObject = pEngine->PopWorldObj();
    if (pObject != nullptr) {
        std::list<BWorldObj*> objects = pEngine->GetWorld()->FindObjectsInRing(type, pObject->GetPositionRef(), innerRadius, outerRadius);
        return pFound(pEngine, pSlices, capacity, objects);
    }
    return pNone(pEngine, pSlices, capacity);
}

static int CountCircle(NPpu::CEngine* pBase, FoundFunc pFound, NoneFunc pNone) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    f32 outerRadius = pEngine->PopFloat();
    f32 innerRadius = pEngine->PopFloat();
    int type = pEngine->PopInt();
    BWorldObj* pObject = pEngine->PopWorldObj();
    if (pObject != nullptr) {
        std::list<BWorldObj*> objects = pEngine->GetWorld()->FindObjectsInRing(type, pObject->GetPositionRef(), innerRadius, outerRadius);
        return pFound(pEngine, nullptr, 0, objects);
    }
    return pNone(pEngine, nullptr, 0);
}

template <int Mode> static int SearchRectangle(NPpu::CEngine* pBase, FoundFunc pFound, NoneFunc pNone) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    f32 width = pEngine->PopFloat();
    f32 maxZ = pEngine->PopFloat();
    f32 minZ = pEngine->PopFloat();
    int type = pEngine->PopInt();
    int capacity = 0;
    NPpu::Slice* pSlices = pEngine->PopArray(&capacity);
    BWorldObj* pObject = pEngine->PopWorldObj();
    if (pObject != nullptr) {
        std::list<BWorldObj*> objects = pEngine->GetWorld()->FindObjectsInRect(type, pObject->GetPositionRef(), pObject->GetRotation(), minZ, maxZ, -width / 2.0f, width / 2.0f);
        return pFound(pEngine, pSlices, capacity, objects);
    }
    return pNone(pEngine, pSlices, capacity);
}

static int CountRectangle(NPpu::CEngine* pBase, FoundFunc pFound, NoneFunc pNone) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    f32 width = pEngine->PopFloat();
    f32 maxZ = pEngine->PopFloat();
    f32 minZ = pEngine->PopFloat();
    int type = pEngine->PopInt();
    BWorldObj* pObject = pEngine->PopWorldObj();
    if (pObject != nullptr) {
        std::list<BWorldObj*> objects = pEngine->GetWorld()->FindObjectsInRect(type, pObject->GetPositionRef(), pObject->GetRotation(), minZ, maxZ, -width / 2.0f, width / 2.0f);
        return pFound(pEngine, nullptr, 0, objects);
    }
    return pNone(pEngine, nullptr, 0);
}

int method_findObjInCircleZX(NPpu::CEngine* pEngine) {
    return SearchCircle<0>(pEngine, PushObjectIds, PushNoObjects);
}

int method_mergeFindObjInCircleZX(NPpu::CEngine* pEngine) {
    return SearchCircle<1>(pEngine, MergeObjectIds, CountMergedIds);
}

int method_countObjInCircleZX(NPpu::CEngine* pEngine) {
    return CountCircle(pEngine, PushObjectCount, PushZeroCount);
}

int method_findObjInRectangleZX(NPpu::CEngine* pEngine) {
    return SearchRectangle<0>(pEngine, PushObjectIds, PushNoObjects);
}

int method_mergeFindObjInRectangleZX(NPpu::CEngine* pEngine) {
    return SearchRectangle<1>(pEngine, MergeObjectIds, CountMergedIds);
}

int method_countObjInRectangleZX(NPpu::CEngine* pEngine) {
    return CountRectangle(pEngine, PushObjectCount, PushZeroCount);
}

int method_getGenerator(NPpu::CEngine* pEngine) {
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        BWorldObj* pGenerator = pObject->GetParent();
        if (pGenerator != nullptr) {
            pEngine->PushInt(pGenerator->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_setGenerator(NPpu::CEngine* pEngine) {
    BWorldObj* pGenerator = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        if (pGenerator != nullptr) {
            pObject->SetParent(pGenerator->shared_from_this());
        } else {
            pObject->SetParent(std::tr1::shared_ptr<BWorldObj>(0));
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_getRegister(NPpu::CEngine* pEngine) {
    u32 index = pEngine->PopInt();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        pEngine->Push(pObject->GetRegister(index));
    } else {
        pEngine->PushInt(0);
    }
    return 0;
}

int method_setRegister(NPpu::CEngine* pEngine) {
    NPpu::Slice value;
    pEngine->Pop(value);
    u32 index = pEngine->PopInt();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        pObject->SetRegister(index, value);
    }
    pEngine->PushNull();
    return 0;
}

int GetNewObjRegister(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    u32 index = pEngine->PopInt();
    pEngine->Push(pEngine->GetRegister()->GetValue(index));
    return 0;
}

int SetNewObjRegister(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    NPpu::Slice value;
    pEngine->Pop(value);
    u32 index = pEngine->PopInt();
    pEngine->GetRegister()->Set(index, value);
    pEngine->PushNull();
    return 0;
}

int method_isShown(NPpu::CEngine* pEngine) {
    bool shown = false;
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        shown = pObject->IsVisible();
    }
    pEngine->PushInt(shown);
    return 0;
}

int method_showInMiniMap(NPpu::CEngine* pEngine) {
    int show = pEngine->PopInt();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        pObject->SetEffectEnable(show != 0);
    }
    pEngine->PushNull();
    return 0;
}

int method_enablePauseForUI(NPpu::CEngine* pEngine) {
    bool enable = pEngine->PopInt() != 0;
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        pObject->EnablePauseForUI(enable);
    }
    pEngine->PushNull();
    return 0;
}

int method_isPauseForUIEnabled(NPpu::CEngine* pEngine) {
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    pEngine->PushInt(pObject != nullptr ? pObject->IsPauseForUIEnabled() : true);
    pEngine->PushNull();
    return 0;
}

int method_enableHitstop(NPpu::CEngine* pEngine) {
    bool enable = pEngine->PopInt() != 0;
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        pObject->EnableHitstop(enable);
    }
    pEngine->PushNull();
    return 0;
}

int method_isHitstopEnabled(NPpu::CEngine* pEngine) {
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    pEngine->PushInt(pObject != nullptr ? pObject->IsHitstopEnabled() : true);
    pEngine->PushNull();
    return 0;
}

int method_setIgnoreEnterEvent(NPpu::CEngine* pEngine) {
    bool ignore = pEngine->PopInt() != 0;
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        pObject->SetIgnoreEnterEvent(ignore);
    }
    pEngine->PushNull();
    return 0;
}

int method_isIgnoreEnterEvent(NPpu::CEngine* pEngine) {
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    pEngine->PushInt(pObject != nullptr ? pObject->IsIgnoreEnterEvent() : true);
    pEngine->PushNull();
    return 0;
}

int CountObj(NPpu::CEngine* pEngine) {
    u32 type = pEngine->PopInt();
    pEngine->PushInt(static_cast<CPpuEngine*>(pEngine)->GetWorld()->CountObjects(type));
    return 0;
}

int GetCIStr(NPpu::CEngine* pEngine) {
    pEngine->Drop(1);
    pEngine->PushNull();
    return 0;
}
