#include "PlayerBullet.h"
#include "SphereColliderData.h"
#include <DxLib.h>

namespace
{
	constexpr float kRunSpeed = 15.0f;
	constexpr float kLifeFrame = 300.0f;
	constexpr float kColRadius = 50.0f;
}

PlayerBullet::PlayerBullet() :
	Collidable(ObjectTag::PlayerWeapon, ObjectPriority::Low, ColliderData::Kind::Sphere),
	m_dir(0.0f, 0.0f, 0.0f),
	m_frame(0),
	m_isDead(false)
{
}

PlayerBullet::~PlayerBullet()
{
}

void PlayerBullet::Init(Vector3 myPos, Vector3 dir)
{
	Collidable::Init();
	m_rigidbody.Init();
	m_rigidbody.SetPos(myPos);

	m_dir = dir;
	m_dir.y = 0.0f;

	auto colData = std::dynamic_pointer_cast<SphereColliderData>(m_colliderData);
	colData->m_radius = kColRadius;
}

void PlayerBullet::Update()
{
	++m_frame;

	// 移動処理
	m_dir.Normalize();
	m_rigidbody.SetVelo(m_dir * kRunSpeed);

	if (m_frame >= kLifeFrame)
	{
		m_isDead = true;
	}
}

void PlayerBullet::Draw()
{
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), kColRadius, 16, 0xff0000, 0xff0000, true);
}

void PlayerBullet::OnCollide(std::shared_ptr<Collidable> collider)
{
	m_isDead = true;
}
