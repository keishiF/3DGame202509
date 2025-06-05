#include "Enemy.h"
#include "Player.h"
#include "DxLib.h"

#include <cassert>

namespace
{
	// �A�j���[�V������
	// �ҋ@
	const char* kIdleAnimName   = "Idle";
	// ����
	const char* kFoundAnimName  = "";
	// �U��
	const char* kAttackAnimName = "";
	// ��e
	const char* kHitAnimName    = "";
	// ���S
	const char* kDeadAnimName   = "";

	// �G�l�~�[���v���C���[�𔭌��ł���͈�
	constexpr float kFindRadius = 300.0f;

	// ����HP
	constexpr int kHp = 10;

	// ���f���̊g�嗦
	constexpr float kModelScale = 45.0f;
}

Enemy::Enemy() :
	m_model(-1),
	m_pos(100.0f, 0.0f, 100.0f),
	m_findRadius(kFindRadius),
	m_hp(kHp),
	m_isDead(false),
	m_state(EnemyState::Idle)
{
	m_model = MV1LoadModel("Data/Enemy/Enemy.mv1");
	assert(m_model >= 0);
	MV1SetScale(m_model, VGet(kModelScale, kModelScale, kModelScale));
	MV1SetPosition(m_model, VGet(m_pos.x, m_pos.y, m_pos.z));

	m_anim.Init(m_model);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kIdleAnimName, true);
}

Enemy::~Enemy()
{
}

void Enemy::Update(std::shared_ptr<Player> player)
{
	// �A�j���[�V�����̍X�V
	m_anim.UpdateAnim(m_anim.GetPrevAnim());
	m_anim.UpdateAnim(m_anim.GetNextAnim());
	m_anim.UpdateAnimBlend();

	switch (m_state)
	{
	case EnemyState::Idle:
		IdleUpdate(player);
		break;
	case EnemyState::Found:
		FoundUpdate(player);
		break;
	case EnemyState::Attack:
		AttackUpdate(player);
		break;
	case EnemyState::Hit:
		HitUpdate(player);
		break;
	case EnemyState::Dead:
		DeadUpdate(player);
		break;
	}
}

void Enemy::Draw()
{
#if _DEBUG
	DrawSphere3D(VGet(m_pos.x, m_pos.y, m_pos.z), 10.0f, 16, 0x0000ff, 0x0000ff, true);
	//DrawSphere3D(VGet(m_pos.x, m_pos.y, m_pos.z), m_findRadius, 16, 0xff00ff, 0xff00ff, false);
#endif

	MV1DrawModel(m_model);
}

void Enemy::OnDamage()
{
	m_hp -= 1;
}

void Enemy::ChangeState(EnemyState newState)
{
	// ���݂̏�ԂƎ��̏�Ԃ������ꍇreturn
	if (m_state == newState) return;

	m_state = newState;

	switch (m_state)
	{
	case EnemyState::Idle:
		m_anim.ChangeAnim(kIdleAnimName, true);
		break;
	case EnemyState::Found:
		m_anim.ChangeAnim(kFoundAnimName, false);
		break;
	case EnemyState::Attack:
		m_anim.ChangeAnim(kAttackAnimName, false);
		break;
	case EnemyState::Hit:
		m_anim.ChangeAnim(kHitAnimName, false);
		break;
	case EnemyState::Dead:
		m_anim.ChangeAnim(kDeadAnimName, false);
		break;
	}
}

void Enemy::IdleUpdate(std::shared_ptr<Player> player)
{
}

void Enemy::FoundUpdate(std::shared_ptr<Player> player)
{
}

void Enemy::AttackUpdate(std::shared_ptr<Player> player)
{
}

void Enemy::HitUpdate(std::shared_ptr<Player> player)
{
}

void Enemy::DeadUpdate(std::shared_ptr<Player> player)
{
}
