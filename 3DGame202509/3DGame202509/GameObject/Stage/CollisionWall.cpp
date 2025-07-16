#define NOMINMAX
#include "BoxColliderData.h"
#include "CollisionWall.h"
#include "Physics.h"
#include <algorithm>
#include <cassert>
#include <DxLib.h>

void CollisionWall::Init(const std::string& modelPath, Vec3 pos, Vec3 rot, Vec3 scale)
{
    Collidable::Init();
    m_rigidbody.Init();
    m_rigidbody.SetPos(pos);
    SetActive(true);

    auto box = std::make_shared<BoxColliderData>();

    Vec3 localBoundsCenter = { 0.0f, 0.0f, 0.0f };
    Vec3 localHalfExtents = { 0.5f, 0.5f, 0.5f };
    m_center = pos;
    Vec3 halfExtents = { localHalfExtents.x * scale.x, localHalfExtents.y * scale.y, localHalfExtents.z * scale.z };

    float eulerX_rad = rot.x * (DX_PI_F / 180.0f);
    float eulerY_rad = rot.y * (DX_PI_F / 180.0f);
    float eulerZ_rad = rot.z * (DX_PI_F / 180.0f);

    Quaternion orientation = QuaternionToEuler(eulerX_rad, eulerY_rad, eulerZ_rad);

    Vec3 obbVertices[8];
    Vec3 h = halfExtents;
    
    Vec3 localVertices[8] = {
    Vec3(-h.x, -h.y, -h.z), Vec3(h.x, -h.y, -h.z),
    Vec3(-h.x,  h.y, -h.z), Vec3(h.x,  h.y, -h.z),
    Vec3(-h.x, -h.y,  h.z), Vec3(h.x, -h.y,  h.z),
    Vec3(-h.x,  h.y,  h.z), Vec3(h.x,  h.y,  h.z)
    };
    
    // ローカル頂点をワールド座標に変換
    for (int i = 0; i < 8; ++i) 
    {
        obbVertices[i] = orientation * localVertices[i] + m_center;
    }
    
    // 8つの頂点から最小のAABBを計算
    Vec3 aabbMin = obbVertices[0];
    Vec3 aabbMax = obbVertices[0];
    
    for (int i = 1; i < 8; ++i) 
    {
        aabbMin.x = std::min(aabbMin.x, obbVertices[i].x);
        aabbMin.y = std::min(aabbMin.y, obbVertices[i].y);
        aabbMin.z = std::min(aabbMin.z, obbVertices[i].z);
        
        aabbMax.x = std::max(aabbMax.x, obbVertices[i].x);
        aabbMax.y = std::max(aabbMax.y, obbVertices[i].y);
        aabbMax.z = std::max(aabbMax.z, obbVertices[i].z);
    }
    
    box->m_min = aabbMin;
    box->m_max = aabbMax;

    m_start = aabbMin;
    m_center = pos;
    m_end = aabbMax;
    m_colliderData = box;
}

void CollisionWall::Draw()
{
    DrawLine3D(m_start.ToDxVECTOR(), m_end.ToDxVECTOR(), 0xff0000);
    DrawSphere3D(m_center.ToDxVECTOR(), 10.0f, 16, 0xff00ff, 0xff00ff, false);
}

Quaternion CollisionWall::QuaternionToEuler(float eulerX_rad, float eulerY_rad, float eulerZ_rad)
{
    float cx = cosf(eulerX_rad * 0.5f);
    float sx = sinf(eulerX_rad * 0.5f);
    float cy = cosf(eulerY_rad * 0.5f);
    float sy = sinf(eulerY_rad * 0.5f);
    float cz = cosf(eulerZ_rad * 0.5f);
    float sz = sinf(eulerZ_rad * 0.5f);
    
    Quaternion q;
    // UnityのZXY順のオイラー角からクォータニオンへの変換式
    q.w = cz * cy * cx + sz * sy * sx;
    q.x = cz * cy * sx - sz * sy * cx;
    q.y = cz * sy * cx + sz * cy * sx;
    q.z = sz * cy * cx - cz * sy * sx;
    return q;
}
