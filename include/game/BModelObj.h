#ifndef GAME_BMODELOBJ_H
#define GAME_BMODELOBJ_H

#include <game/BKDTreeLink.h>
#include <game/BSoundSource.h>
#include <game/BWorldObj.h>
#include <nw4r/math/math_geometry.h>

#include <game/CModel.h>

class BModelObj : public BWorldObj, public BSoundSource, public BKDTreeLink {
public:
    enum { TYPE = TYPE_MODEL_OBJ };

    enum Flag {
        FLAG_DIRTY_MTX = 1 << 0,
        FLAG_HIDDEN = 1 << 1,
        FLAG_BASE_MTX = 1 << 2,
        FLAG_KDTREE = 1 << 3,
        FLAG_OUTLINE = 1 << 4,
        FLAG_SIZE_MODE = 1 << 5,
        FLAG_NO_BOUNDING = 1 << 6,
        FLAG_SHADOW = 1 << 7,
        FLAG_MODEL_SCALE_SIZE = 1 << 8
    };

    enum ModelFlag {
        MODEL_TYPE_MASK = 3,
        MODEL_TYPE_NORMAL = 0,
        MODEL_TYPE_SHADOW = 1,
        MODEL_TYPE_OUTLINE = 2,
        MODEL_SCALE_SIZE = 1 << 2
    };

    explicit BModelObj(u32 type);
    virtual ~BModelObj();

    virtual void Calc();
    virtual void CalcMtx();
    virtual void SetPosition(const nw4r::math::VEC3& rPosition);
    virtual void SetScale(const nw4r::math::VEC3& rScale);
    virtual void SetRotation(f32 rotation);
    virtual nw4r::math::VEC3 GetSize();
    virtual void SetVisible(bool visible);
    virtual DrawLayerSet* GetDrawLayers();
    virtual void OnEnterWorld();
    virtual void OnLeaveWorld(BWorld* pWorld);

    virtual void SetModel(const std::tr1::shared_ptr<CModel>& rModel, u32 flags);
    virtual void SetTransform(const nw4r::math::VEC3& rPosition, const nw4r::math::VEC3& rVelocity);
    virtual void SetModelScale(const nw4r::math::VEC3& rScale);
    virtual void SetBaseMtx(const nw4r::math::MTX34& rMtx);
    virtual void ClearBaseMtx();
    virtual void SetTransformDirect(const nw4r::math::VEC3& rPosition, const nw4r::math::VEC3& rVelocity);
    virtual void SetScaleDirect(const nw4r::math::VEC3& rScale);
    virtual void SetModelScaleDirect(const nw4r::math::VEC3& rScale);
    virtual void SetBoundingEnable(bool enable);
    virtual void SetSizeMode(int mode);
    virtual void AddDrawLayer(const std::tr1::shared_ptr<IWorldObjStat>& rLayer);

    void ChangeModel(const std::tr1::shared_ptr<CModel>& rModel, u32 flags);
    int GetMotionLength(const char* pName);
    std::tr1::shared_ptr<CModel::CMotionProgram> Play(const char* pName, f32 frame, f32 rate);
    std::tr1::shared_ptr<CModel::CMotionProgram> PlayLoop(const char* pName, f32 frame, f32 rate);
    std::tr1::shared_ptr<CModel::CMotionProgram> PlayOnce(const char* pName, f32 frame, f32 rate);
    std::tr1::shared_ptr<CModel::CMotionProgram> LengthPlay(const char* pName, f32 frame, f32 length);
    std::tr1::shared_ptr<CModel::CMotionProgram> LengthRequest(const char* pName, f32 frame, f32 length);
    std::tr1::shared_ptr<CModel::CMotionProgram> LengthRequestNext(const char* pName, f32 frame, f32 length);
    void Pause(bool pause);
    void Resume();
    void SetOutlineType(int type);
    void UpdateOutline();

    const std::tr1::shared_ptr<CModel>& GetModel() const { return mModel; }

private:
    static bool IsOutlineModel(const std::tr1::shared_ptr<CModel>& rModel);

    nw4r::math::VEC3 CalcSize(int mode) const;

    void UpdateBounds() {
        mBounds.min.x = mPosition.x - mSize.x;
        mBounds.min.y = mPosition.y;
        mBounds.min.z = mPosition.z - mSize.z;
        mBounds.max.x = mPosition.x + mSize.x;
        mBounds.max.y = mPosition.y + 2.0f * mSize.y;
        mBounds.max.z = mPosition.z + mSize.z;
    }

    u32 mFlags;
    std::tr1::shared_ptr<CModel> mModel;
    nw4r::math::VEC3 mModelScale;
    nw4r::math::VEC3 mSize;
    nw4r::math::AABB mBounds;
    nw4r::math::VEC3 mVelocity;
    nw4r::math::MTX34 mBaseMtx;
    int mOutlineType;
    DrawLayerSet mDrawLayers;
};

#endif
