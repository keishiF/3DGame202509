#include "EnemyMageBullet.h"
#include "DxLib.h"

namespace
{
	constexpr float kSpeed = 5.0f;
	constexpr float kLifeFrame = 180.0f;
}

EnemyMageBullet::EnemyMageBullet(Vec3 pos, Vec3 vec) :
	Collidable(GameObjectTag::Bullet, ColliderData::Kind::Sphere),
	m_frame(0)
{
	m_pos = pos;
	m_vec = vec;
}

EnemyMageBullet::~EnemyMageBullet()
{

}

void EnemyMageBullet::Update()
{
	++m_frame;
	m_pos += m_vec * kSpeed;
	if (m_frame >= kLifeFrame)
	{
		delete(this);
	}
}

void EnemyMageBullet::Draw()
{
	DrawSphere3D(VGet(m_pos.x, m_pos.y, m_pos.z), 20.0f, 16, 0xff0000, 0xff0000, true);
}

void EnemyMageBullet::OnCollide(std::shared_ptr<Collidable> collider)
{
}