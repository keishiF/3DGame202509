#include "EnemyMageBullet.h"

#include "SphereColliderData.h"
#include "DxLib.h"

namespace
{
	constexpr float kSpeed = 5.0f;
	constexpr float kLifeFrame = 180.0f;
	constexpr float kColRadius = 20.0f;
}

EnemyMageBullet::EnemyMageBullet(Vec3 pos, Vec3 vec, std::shared_ptr<Physics> physics) :
	Collidable(ObjectTag::Weapon, ObjectPriority::Low, ColliderData::Kind::Sphere),
	m_frame(0),
	m_isDead(false)
{
	Collidable::Init(physics);
	m_rigidbody.SetPos(pos);

	m_rigidbody.SetVelo(vec);

	auto colData = std::dynamic_pointer_cast<SphereColliderData>(m_colliderData);
	colData->m_radius = kColRadius;
}

EnemyMageBullet::~EnemyMageBullet()
{

}

void EnemyMageBullet::Update()
{
	++m_frame;

	// ˆÚ“®ˆ—
	m_rigidbody.SetVelo(m_rigidbody.GetVelo() * kSpeed);

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
}