#include "EnemyBase.h"

EnemyBase::EnemyBase() :
    m_state(EnemyState::Find),
    m_charModel(-1),
    m_weaponModel(-1),
    m_findRadius(0.0f),
    m_attackRadius(0.0f),
    m_hp(0),
    m_isDead(false),
    m_attackFrame(0),
    Collidable(ObjectTag::Enemy, ObjectPriority::Low, ColliderData::Kind::Capsule)
{
}

EnemyBase::~EnemyBase()
{

}

void EnemyBase::OnCollide(std::shared_ptr<Collidable> collider)
{
    OnDamage();
    SetActive(false);
}

void EnemyBase::ChangeState(EnemyState newState, float playSpeed)
{
    // Œ»İ‚Ìó‘Ô‚ÆŸ‚Ìó‘Ô‚ª“¯‚¶ê‡return
    // Hit‚¾‚¯—áŠOˆ—
    if (m_state == newState && m_state != EnemyState::Hit) return;

    m_state = newState;
    m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
    m_attackFrame = 0.0f;

    const char* animName = GetAnimName(newState);
    bool loop = IsLoopAnim(newState);
    if (animName)
    {
        m_anim.ChangeAnim(animName, playSpeed, loop);
    }
}
