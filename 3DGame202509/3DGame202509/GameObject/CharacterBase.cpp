#include "CharacterBase.h"

CharacterBase::CharacterBase(ObjectTag tag, ObjectPriority priority, ColliderData::Kind colliderKind) :
	Collidable(tag, priority, colliderKind)
{
}

CharacterBase::~CharacterBase()
{
}

void CharacterBase::OnCollide(std::shared_ptr<Collidable> collider)
{
	OnDamage(m_atk);
}
