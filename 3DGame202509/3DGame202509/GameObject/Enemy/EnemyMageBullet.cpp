#include "EnemyMageBullet.h"

#include "SphereColliderData.h"
#include "DxLib.h"

namespace
{
	constexpr float kSpeed = 5.0f;
	constexpr float kLifeFrame = 180.0f;
	constexpr float kColRadius = 20.0f;
}

EnemyMageBullet::EnemyMageBullet() :
	Collidable(ObjectTag::Bullet, ObjectPriority::Low, ColliderData::Kind::Sphere),
	m_dir(0.0f, 0.0f, 0.0f),
	m_frame(0),
	m_isDead(false)
{
}

EnemyMageBullet::~EnemyMageBullet()
{
}

void EnemyMageBullet::Init(Vec3 myPos, Vec3 playerPos, std::shared_ptr<Physics> physics)
{
	Collidable::Init(physics);
	m_rigidbody.Init();
	m_rigidbody.SetPos(myPos);

	m_dir = playerPos - myPos;
	m_dir.y = 0.0f;

	auto colData = std::dynamic_pointer_cast<SphereColliderData>(m_colliderData);
	colData->m_radius = kColRadius;
}

void EnemyMageBullet::Update()
{
	++m_frame;

	// ˆÚ“®ˆ—
	m_dir.Normalize();
	m_rigidbody.SetVelo(m_dir * kSpeed);

	if (m_frame >= kLifeFrame)
	{
		m_isDead = true;
	}
}

void EnemyMageBullet::Draw()
{
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), kColRadius, 16, 0xff0000, 0xff0000, true);
}

void EnemyMageBullet::OnCollide(std::shared_ptr<Collidable> collider)
{
	m_isDead = true;
}
