#include <game/BModelObj.h>
#include <game/BWorld.h>
#include <game/CDividedGround.h>
#include <game/CPpuEngine.h>
#include <math.h>

#pragma dont_inline on

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
