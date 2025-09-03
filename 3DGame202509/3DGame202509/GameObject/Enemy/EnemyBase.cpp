#include "EnemyBase.h"

namespace
{
    constexpr int kInvincibilityDuration = 30;
}

EnemyBase::EnemyBase(ObjectTag tag, ObjectPriority priority, ColliderData::Kind colliderKind) :
    m_state(EnemyState::Find),
    m_prevState(m_state),
    CharacterBase(tag, priority, colliderKind)
{
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::Init(Vector3& pos, Vector3& rot, Vector3& scale)
{
}

void EnemyBase::Draw()
{
}

void EnemyBase::OnDamage(float atk)
{
    // 無敵時間中は何もしない
    if (m_isInvincible)
    {
        return;
    }

    m_status.m_hp -= atk;

    // ダメージを受けたら無敵時間を開始する
    m_isInvincible = true;
    m_invincibleFrame = kInvincibilityDuration;

    if (m_status.m_hp <= 0 && !m_isDead)
    {
        ChangeState(EnemyState::Dead);
    }
    else
    {
        ChangeState(EnemyState::Hit);
    }
}

Vector3 EnemyBase::GetScreenPos() const
{
    Vector3 worldPos = m_rigidbody.GetPos();
    worldPos.y += 120.0f; // 頭上の高さ調整

    VECTOR worldPosDx = worldPos.ToDxVECTOR();

    // 3D→2D座標変換（戻り値がスクリーン座標）
    VECTOR screenPosDx = ConvWorldPosToScreenPos(worldPosDx);

    const int gaugeWidth = 100;
    const int gaugeHeight = 10;

    screenPosDx.x = screenPosDx.x - gaugeWidth * 0.5f;
    screenPosDx.y = screenPosDx.y - gaugeHeight * 0.5f;

    Vector3 screenPos =
    {
        screenPosDx.x, screenPosDx.y, screenPosDx.z
    };

    return screenPos;
}

void EnemyBase::ChangeState(EnemyState newState)
{
    // 現在の状態と次の状態が同じ場合return
    // (Hitは除く)
    if (m_state == newState && m_state != EnemyState::Hit) return;
    // 疲れ状態の時に被弾しても被弾状態には遷移しない
    if (m_state == EnemyState::Tired && newState == EnemyState::Hit) return;

    m_prevState = m_state;
    m_state = newState;
    m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
    m_atkFrame = 0;

    const char* animName = GetAnimName(m_state);
    float animPlaySpeed = GetAnimPlaySpeed(m_state);
    bool loop = IsLoopAnim(m_state);
    if (animName)
    {
        m_anim.ChangeAnim(animName, animPlaySpeed, loop);
    }
}
