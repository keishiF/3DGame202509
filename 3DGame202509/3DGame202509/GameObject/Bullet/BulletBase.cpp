#include "BulletBase.h"

BulletBase::BulletBase(ObjectTag tag, ObjectPriority priority, ColliderData::Kind colliderKind) :
	m_dir(0.0f, 0.0f, 0.0f),
	m_lifeFrame(0.0f),
	m_isDead(false),
	Collidable(tag, priority, colliderKind)
{
}

BulletBase::~BulletBase()
{
}

void BulletBase::OnCollide(std::shared_ptr<Collidable> collider)
{
	m_isDead = true;
}
