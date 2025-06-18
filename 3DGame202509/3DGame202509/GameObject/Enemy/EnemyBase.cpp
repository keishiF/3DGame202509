#include "EnemyBase.h"

EnemyBase::EnemyBase() :
    m_state(EnemyState::Find),
    m_charModel(-1),
    m_weaponModel(-1),
    m_pos(),
    m_vec(),
    m_speed(),
    m_findRadius(0.0f),
    m_attackRadius(0.0f),
    m_hp(0),
    m_isDead(false),
    m_attackFrame(0),
    Collidable(GameObjectTag::Enemy, ColliderData::Kind::Capsule)
{
}

EnemyBase::~EnemyBase()
{

}

void EnemyBase::OnDamage()
{
    m_hp -= 1;
}

void EnemyBase::OnCollide(std::shared_ptr<Collidable> collider)
{
    OnDamage();
}

void EnemyBase::ChangeState(EnemyState newState)
{
    if (m_state == newState) return;

    m_state = newState;

    const char* animName = GetAnimName(newState);
    bool loop = IsLoopAnim(newState);
    if (animName)
    {
        m_anim.ChangeAnim(animName, loop);
    }
}
