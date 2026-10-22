#include <game/BModelObj.h>
#include <game/BWorld.h>
#include <game/CBSPTree.h>
#include <game/CDividedGround.h>
#include <game/CItem.h>
#include <game/CPlayer.h>
#include <game/CPiiPersonalData.h>
#include <game/CPlayerPii.h>
#include <game/CPpuEngine.h>
#include <game/IGameSession.h>
#include <game/MathUtil.h>
#include <nw4r/math/math_triangular.h>

#pragma dont_inline on

int method_moveTo(NPpu::CEngine* pEngine);

static inline nw4r::math::VEC3& Scale(nw4r::math::VEC3& rVector, f32 scale) {
    nw4r::math::VEC3Scale(&rVector, &rVector, scale);
    return rVector;
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
            if (pPii->GetPersonalData()->GetHP() == 0) {
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
        f32 halfWidth = 0.5f * width;
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
            if (pPii->GetPersonalData()->GetHP() == 0) {
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
