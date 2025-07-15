#include "BoxColliderData.h"
#include "CollisionWall.h"
#include <cassert>
#include <DxLib.h>

void CollisionWall::Init(const std::string& modelPath, Vec3 pos, Vec3 rot, Vec3 scale)
{
    SetActive(true);

    m_rigidbody.SetPos(pos);

    auto box = std::make_shared<BoxColliderData>();
    box->m_size = scale;
    m_colliderData = box;
}

void CollisionWall::Draw()
{

}
