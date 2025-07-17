#define NOMINMAX
#include "PlaneColliderData.h"
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
}

void CollisionWall::Draw()
{
}
