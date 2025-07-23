#pragma once
#include "ColliderData.h"
#include "Vector3.h"
#include <DxLib.h>

class PolygonColliderData : public ColliderData
{
public:
    PolygonColliderData();
    virtual ~PolygonColliderData();
    MV1_COLL_RESULT_POLY_DIM& GetHitDim() { return m_hitDim; }
    void SetHitDim(MV1_COLL_RESULT_POLY_DIM& dim) { m_hitDim = dim; }
    //壁と床の近い座標
    Vector3 GetNearWallHitPos() const { return m_nearWallHitPos; }
    Vector3 GetNearFloorHitPos() const { return m_nearFloorHitPos; }
    //モデル
    int GetModelHandle() const { return m_modelHandle; }
public:
    //当たり判定をするモデル
    int m_modelHandle;
    //当たったポリゴンの情報
    MV1_COLL_RESULT_POLY_DIM m_hitDim;
    //当たった際の最も近い壁ポリゴンの座標
    Vector3 m_nearWallHitPos;
    //当たった際の最も近い床ポリゴンの座標
    Vector3 m_nearFloorHitPos;
};

