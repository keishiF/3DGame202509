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
    // 衝突相手の攻撃力を取得する
    float damage = collider->GetAttackPower();
    
    // ダメージが0より大きい場合のみ処理する
    if (damage > 0.0f)
    {
        OnDamage(damage);
    }
}
