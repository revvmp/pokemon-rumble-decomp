#ifndef GAME_BWORLDOBJ_H
#define GAME_BWORLDOBJ_H

#include <game/ICalc.h>
#include <memory>
#include <nw4r/math/math_types.h>
#include <set>

class BWorld;
class BWorldObjEffect;
class BWorldObjOwner;
class CPpuEngine;
class IDrawLayer;

typedef std::set<std::tr1::shared_ptr<IDrawLayer> > DrawLayerSet;

class BWorldObj : public ICalc, public std::tr1::enable_shared_from_this<BWorldObj> {
public:

    enum StateFlag {
        STATE_VISIBLE = 1 << 0
    };

    enum TypeFlag {
        TYPE_WORLD_OBJ = 1 << 0,
        TYPE_CAMERA = 1 << 1,
        TYPE_SHADOW_CAMERA = 1 << 2,
        TYPE_MODEL_OBJ = 1 << 5
    };

    explicit BWorldObj(u32 type);
    virtual ~BWorldObj();

    virtual void Calc() {}
    virtual void CalcMtx() {}
    virtual void SetPosition(const nw4r::math::VEC3& rPosition);
    virtual void SetScale(const nw4r::math::VEC3& rScale);
    virtual void SetRotation(f32 rotation);
    virtual nw4r::math::VEC3 GetSize() { return nw4r::math::VEC3(0.0f, 0.0f, 0.0f); }
    virtual nw4r::math::VEC3 GetPosition() { return mPosition; }
    virtual void SetVisible(bool visible);
    virtual CPpuEngine* GetPpuEngine() const { return nullptr; }
    virtual BWorldObjEffect* GetEffect() const { return nullptr; }
    virtual void SetEffectEnable(bool enable) {}
    virtual DrawLayerSet* GetDrawLayers() { return nullptr; }
    virtual void OnEnterWorld() {}
    virtual void OnLeaveWorld() {}
    virtual void UpdateVisible() {}

    bool IsVisible() const { return mState & STATE_VISIBLE; }
    BWorld* GetWorld() const { return mWorld; }
    u32 GetId() const { return mId; }
    u32 GetType() const { return mType; }

    void Pause();

protected:
    friend class BWorld;


    u32 mState;
    u32 mType;
    BWorld* mWorld;
    u32 mId;
    std::tr1::weak_ptr<BWorld> mWorldRef;
    std::tr1::shared_ptr<BWorldObjOwner> mOwnerRef;
    f32 mRotation;
    nw4r::math::VEC3 mPosition;
    nw4r::math::VEC3 mScale;
};

template <typename T, typename U> bool IsKindOf(const std::tr1::shared_ptr<U>& rObj) {
    return rObj && (rObj->GetType() & T::TYPE);
}

template <typename T, typename U> std::tr1::shared_ptr<T> DynamicCast(const std::tr1::shared_ptr<U>& rObj) {
    return IsKindOf<T>(rObj) ? std::tr1::static_pointer_cast<T>(rObj) : std::tr1::shared_ptr<T>();
}

#endif
