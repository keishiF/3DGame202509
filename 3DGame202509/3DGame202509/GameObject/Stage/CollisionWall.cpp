#include "BoxColliderData.h"
#include "CollisionWall.h"
#include "Physics.h"
#include <cassert>
#include <DxLib.h>

void CollisionWall::Init(const std::string& modelPath, Vec3 pos, Vec3 rot, Vec3 scale)
{
    Collidable::Init();
    m_rigidbody.Init();
    m_rigidbody.SetPos(pos);
    SetActive(true);

    auto box = std::make_shared<BoxColliderData>();
    Vec3 halfScale = scale * 0.5f;
    box->m_min = pos - halfScale;
    box->m_max = pos + halfScale;

    m_start = box->m_min;
    m_end = box->m_max;

    m_colliderData = box;
}

void CollisionWall::Draw()
{
    DrawLine3D(m_start.ToDxVECTOR(), m_end.ToDxVECTOR(), 0xff0000);
    DrawSphere3D(m_start.ToDxVECTOR(), 10.0f, 16, 0xff00ff, 0xff00ff, false);
    DrawSphere3D(m_end.ToDxVECTOR(), 10.0f, 16, 0xff00ff, 0xff00ff, false);
}
