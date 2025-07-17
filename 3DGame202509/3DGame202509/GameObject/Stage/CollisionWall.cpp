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

    auto colData = std::dynamic_pointer_cast<BoxColliderData>(m_colliderData);
    colData->SetCenter(pos);
    colData->SetSize(scale);
}

void CollisionWall::Draw()
{
    auto colData = std::dynamic_pointer_cast<BoxColliderData>(m_colliderData);
    DrawCube3D(colData->GetMin().ToDxVECTOR(), colData->GetMax().ToDxVECTOR(), 0xff00ff, 0xff00ff, false);
}
