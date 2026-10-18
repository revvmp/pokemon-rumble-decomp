#ifndef GAME_BWORLD_H
#define GAME_BWORLD_H

#include <game/BWorldObj.h>
#include <game/CaptureTexture.h>
#include <game/ICalcDraw.h>
#include <memory>
#include <nw4r/ef/ef_res_texture.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <nw4r/g3d/g3d_scnroot.h>
#include <nw4r/ut/ut_Color.h>
#include <set>
#include <vector>

class BModelObj;
class BWorldCamera;
class CEffect;
class CShadowCamera;
template <typename T> class CKDTree;

class BWorld : public ICalcDraw, public std::tr1::enable_shared_from_this<BWorld> {
public:
    enum Flag {
        FLAG_IN_CALC = 1 << 0,
        FLAG_CALC_DISABLED = 1 << 1,
        FLAG_SKIP_STATE_2 = 1 << 2,
        FLAG_SKIP_STATE_4 = 1 << 3,
        FLAG_PAUSE_REQUESTED = 1 << 4,
        FLAG_RESUME_REQUESTED = 1 << 5,
        FLAG_PAUSED = 1 << 6,
        FLAG_SOUND_LISTENER = 1 << 7
    };

    enum DrawType {
        DRAW_TYPE_NORMAL,
        DRAW_TYPE_OUTLINE,
        DRAW_TYPE_2,
        DRAW_TYPE_OUTLINE_ALPHA,
        DRAW_TYPE_ALPHA
    };

    class Desc {
    public:
        Desc();

        void SetDrawType(u32 type);
        void SetObjectCapacity(u32 capacity);
        void SetOutlineCapacity(u32 capacity);
        void SetModelCapacity(u32 capacity);
        void SetScnObjCapacity(u32 capacity);
        void SetEffectCapacity(u32 capacity);
        void SetCaptureEnable(bool enable);
        void SetSoundListenerEnable(bool enable);

        u32 GetDrawType() const { return mDrawType; }
        u32 GetObjectCapacity() const { return mObjectCapacity; }
        u32 GetOutlineCapacity() const { return mOutlineCapacity; }
        u32 GetModelCapacity() const { return mModelCapacity; }
        u32 GetScnObjCapacity() const { return mScnObjCapacity; }
        u32 GetEffectCapacity() const { return mEffectCapacity; }
        bool IsCaptureEnable() const { return mCaptureEnable; }
        bool IsSoundListenerEnable() const { return mSoundListenerEnable; }

    private:
        u32 mDrawType;
        u32 mObjectCapacity;
        u32 mOutlineCapacity;
        u32 mModelCapacity;
        u32 mScnObjCapacity;
        u32 mEffectCapacity;
        bool mCaptureEnable;
        bool mSoundListenerEnable;
    };

    class TextureState {
    public:
        void Save(nw4r::ef::ResTexture texture);
        void Restore();

    private:
        nw4r::ef::ResTexture mTexture;
        u16 mWidth;
        u16 mHeight;
        u8 mFormat;
        bool mHasTlut;
        u8 mTlutFormat;
        u16 mTlutEntries;
        u8 mMipmap;
        u8* mpTexture;
        u8* mpTlut;
    };

    struct ObjEntry {
        ObjEntry() : mFlags(0) {}

        u32 mFlags;
        u16 mNext;
        u16 mPrev;
        std::tr1::shared_ptr<BWorldObj> mObj;
    };

    struct EffectData {
        u32 mLastHandle;
        u32 mGroupID;
        u32 mPauseGroupID;
        std::vector<std::tr1::shared_ptr<CEffect> > mEffects;
        bool mCaptureEnable;
        CaptureTexture mCaptureTexture;
        TextureState mTextureState;
        u32 mCaptureGroupID;
    };

    struct OutlineData {
        CaptureTexture mTexture;
        int mWidth;
        u8 mAlpha;
        nw4r::ut::Color mColor __attribute__((aligned(4)));
    };
    explicit BWorld(const Desc& rDesc);
    virtual ~BWorld();

    virtual void Calc();
    virtual void CalcMtx();
    virtual void CalcView();
    virtual void Draw();

    void SetFlag(u32 flag, bool enable) {
        if (enable) {
            mFlags |= flag;
        } else {
            mFlags &= ~flag;
        }
    }
    void EnableCalc(bool enable);
    void RequestPause();
    void RequestResume();
    BWorldCamera* GetCamera(int index);
    bool AddCamera(std::tr1::shared_ptr<BWorldCamera> camera, u32 index);
    void SetCamera(std::tr1::shared_ptr<BWorldCamera> camera, u32 index);
    int AddCamera(std::tr1::shared_ptr<BWorldCamera> camera);
    int AttachCamera(std::tr1::shared_ptr<BWorldCamera> camera);
    void RemoveCamera(const std::tr1::shared_ptr<BWorldCamera>& camera);
    void ClearCamera(u32 index);
    CShadowCamera* GetShadowCamera();
    void SetCurrentCamera(const std::tr1::shared_ptr<BWorldCamera>& camera);
    BWorldCamera* GetCurrentCamera();
    nw4r::g3d::Camera GetG3dCamera();
    void SetOutlineAlpha(u8 alpha);
    void SetOutlineWidth(int width);
    bool AddGround(std::tr1::shared_ptr<BWorldObj> obj);
    void SetGround(std::tr1::shared_ptr<BWorldObj> obj);
    bool Add(std::tr1::shared_ptr<BWorldObj> obj);
    bool AddObject(std::tr1::shared_ptr<BWorldObj> obj);
    void RemoveObject(std::tr1::shared_ptr<BWorldObj> obj);
    void RemoveAllEffects(bool retire);

private:
    u32 mFlags;
    u32 mLastObjHandle;
    u32 mPendingFrames;
    int mDrawType;
    std::tr1::shared_ptr<nw4r::g3d::ScnRoot> mScnRoot;
    std::tr1::shared_ptr<nw4r::g3d::ScnGroup> mOutlineGroup;
    std::tr1::shared_ptr<nw4r::g3d::ScnGroup> mModelGroup;
    std::vector<std::tr1::weak_ptr<BWorldCamera> > mCameras;
    std::tr1::weak_ptr<BWorldCamera> mCurrentCamera;
    std::tr1::weak_ptr<CShadowCamera> mShadowCamera;
    std::tr1::weak_ptr<BWorldObj> mGround;
    std::vector<ObjEntry> mObjEntries;
    ObjEntry* mObjHead;
    ObjEntry* mObjTail;
    std::set<std::tr1::shared_ptr<BWorldObj> > mRemovedObjs;
    std::tr1::shared_ptr<CKDTree<BModelObj> > mModelTree;
    std::tr1::shared_ptr<EffectData> mEffectData;
    std::tr1::shared_ptr<OutlineData> mOutlineData;
};

#endif