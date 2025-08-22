#include "PlayerBullet.h"
#include "SphereColliderData.h"

namespace
{
	// 弾速
	constexpr float kRunSpeed = 15.0f;
	// 弾の生存時間
	constexpr float kLifeFrame = 300.0f;
	// 弾の半径
	constexpr float kColRadius = 50.0f;
}

PlayerBullet::PlayerBullet() :
	BulletBase(ObjectTag::PlayerWeapon, ObjectPriority::Low, ColliderData::Kind::Sphere)
{
}

PlayerBullet::~PlayerBullet()
{
}

void PlayerBullet::Init(Vector3 pos, Vector3 dir)
{
	Collidable::Init();
	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);

	m_dir = dir;
	m_dir.y = 0.0f;
	m_dir.Normalize();

	auto colData = std::dynamic_pointer_cast<SphereColliderData>(m_colliderData);
	colData->m_radius = kColRadius;
}

void PlayerBullet::Update()
{
	++m_lifeFrame;
	// 移動処理
	m_rigidbody.SetVelo(m_dir * kRunSpeed);
	
	if (m_lifeFrame >= kLifeFrame)
	{
		m_isDead = true;
	}
}

void PlayerBullet::Draw()
{
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), kColRadius, 16, 0xff0000, 0xff0000, true);
}
