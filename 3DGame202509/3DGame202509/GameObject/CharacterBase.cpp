#include "CharacterBase.h"

void CharacterBase::OnCollide(std::shared_ptr<Collidable> collider)
{
	OnDamage(m_atk);
}
