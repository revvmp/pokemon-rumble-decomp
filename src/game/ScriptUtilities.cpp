#include <game/NPpuVM.h>
#include <game/MathUtil.h>
#include <game/Random.h>
#include <nw4r/math/math_triangular.h>
#include <math.h>
#include <stdlib.h>
#include <game/BModelObj.h>
#include <game/CPpuEngine.h>
#include <game/CTextWorldObjStat.h>
#include <game/IGameMap.h>
#include <game/IWorldObjStatDisplay.h>

int NormalizeDeg(NPpu::CEngine* pEngine) {
    pEngine->PushFloat(NormalizeSignedAngle(pEngine->PopFloat()));
    return 0;
}

int NormalizeDegPositive(NPpu::CEngine* pEngine) {
    pEngine->PushFloat(NormalizeAngle(pEngine->PopFloat()));
    return 0;
}

int RandInt(NPpu::CEngine* pEngine) {
    int max = pEngine->PopInt();
    int min = pEngine->PopInt();
    pEngine->PushInt(RandInt(min, max));
    return 0;
}

int RandReal(NPpu::CEngine* pEngine) {
    f32 max = pEngine->PopFloat();
    f32 min = pEngine->PopFloat();
    pEngine->PushFloat(RandReal(min, max));
    return 0;
}

int RandB(NPpu::CEngine* pEngine) {
    pEngine->PushInt(RandBool(pEngine->PopFloat()));
    return 0;
}

int Floor(NPpu::CEngine* pEngine) {
    pEngine->PushFloat(floor(pEngine->PopFloat()));
    return 0;
}

int Ceil(NPpu::CEngine* pEngine) {
    pEngine->PushFloat(ceil(pEngine->PopFloat()));
    return 0;
}

int Round(NPpu::CEngine* pEngine) {
    pEngine->PushFloat(roundf(pEngine->PopFloat()));
    return 0;
}

int SinDeg(NPpu::CEngine* pEngine) {
    pEngine->PushFloat(nw4r::math::SinDeg(pEngine->PopFloat()));
    return 0;
}

int CosDeg(NPpu::CEngine* pEngine) {
    pEngine->PushFloat(nw4r::math::CosDeg(pEngine->PopFloat()));
    return 0;
}

int TanDeg(NPpu::CEngine* pEngine) {
    pEngine->PushFloat(tan(NW4R_MATH_DEG_TO_RAD(pEngine->PopFloat())));
    return 0;
}

int Atan2Deg(NPpu::CEngine* pEngine) {
    f32 x = pEngine->PopFloat();
    f32 y = pEngine->PopFloat();
    if (x == 0.0f && y == 0.0f) {
        pEngine->PushFloat(0.0f);
    } else {
        pEngine->PushFloat(NormalizeSignedAngle(nw4r::math::Atan2Deg(y, x)));
    }
    return 0;
}

int Sqrt(NPpu::CEngine* pEngine) {
    f32 value = pEngine->PopFloat();
    if (value < 0.0f) {
        pEngine->PushFloat(0.0f);
    } else {
        pEngine->PushFloat(sqrt(value));
    }
    return 0;
}

int Abs(NPpu::CEngine* pEngine) {
    pEngine->PushFloat(__fabs(pEngine->PopFloat()));
    return 0;
}

int Fmod(NPpu::CEngine* pEngine) {
    f32 denominator = pEngine->PopFloat();
    f32 numerator = pEngine->PopFloat();
    pEngine->PushFloat(fmod(numerator, denominator));
    return 0;
}

int Pow(NPpu::CEngine* pEngine) {
    f32 exponent = pEngine->PopFloat();
    f32 value = pEngine->PopFloat();
    pEngine->PushFloat(pow(value, exponent));
    return 0;
}

int Pow2(NPpu::CEngine* pEngine) {
    f32 value = pEngine->PopFloat();
    pEngine->PushFloat(value * value);
    return 0;
}

int Div(NPpu::CEngine* pEngine) {
    NPpu::CValue* pResult = pEngine->PopReference();
    int denominator = pEngine->PopInt();
    int numerator = pEngine->PopInt();
    div_t result = div(numerator, denominator);
    pResult[0].SetInt(result.quot);
    pResult[1].SetInt(result.rem);
    pEngine->PushInt(0);
    return 0;
}

int Int(NPpu::CEngine* pEngine) {
    pEngine->PushInt(static_cast<int>(pEngine->PopFloat()));
    return 0;
}


int method_showTextWorldObjStat(NPpu::CEngine* pBaseEngine) {
    const wchar_t* pText = pBaseEngine->PopWString();
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    s32 kind = pEngine->PopInt();
    BModelObj* pObj = DynamicCast<BModelObj>(pEngine->PopWorldObj());
    if (pObj != nullptr) {
        std::tr1::shared_ptr<CTextWorldObjStat> stat(new CTextWorldObjStat);
        stat->Load(kind, pText, false);
        pObj->AddDrawLayer(stat);
        GetCurrentGameMap()->GetWorldObjStatDisplay()->Add(pObj);
    }
    pEngine->PushInt(0);
    return 0;
}

int method_enableBoundingVolume(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    bool enable = pEngine->PopInt() != 0;
    BModelObj* pObj = DynamicCast<BModelObj>(pEngine->PopWorldObj());
    if (pObj != nullptr) {
        pObj->SetBoundingEnable(enable);
    }
    pEngine->PushInt(0);
    return 0;
}

int method_setMtx34(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    NPpu::CValue* pValues = pEngine->PopReference();
    BModelObj* pObj = DynamicCast<BModelObj>(pEngine->PopWorldObj());
    if (pObj != nullptr) {
        nw4r::math::MTX34 mtx;
        for (int i = 0; i < 12; ++i) {
            mtx.a[i] = pValues[i].ToFloat();
        }
        pObj->SetBaseMtx(mtx);
    }
    pEngine->PushInt(0);
    return 0;
}

int method_unsetMtx34(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    BModelObj* pObj = DynamicCast<BModelObj>(pEngine->PopWorldObj());
    if (pObj != nullptr) {
        pObj->ClearBaseMtx();
    }
    pEngine->PushInt(0);
    return 0;
}
