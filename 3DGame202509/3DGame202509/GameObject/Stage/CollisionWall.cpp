#include "CollisionWall.h"
#include "BoxColliderData.h"
#include <DxLib.h>
#include <cassert>

void CollisionWall::Init(const std::string& modelPath, Vec3 pos, Vec3 rot, Vec3 scale)
{
    SetActive(true);

    m_rigidbody.SetPos(pos);

    auto box = std::make_shared<BoxColliderData>();
    box->m_halfWidth = scale * 0.5f;
    m_colliderData = box;
}
