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
	m_state(&Enemy::IdleInit)
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
	(this->*m_state)(player);
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

void Enemy::IdleInit(std::shared_ptr<Player> player)
{
	// �ҋ@�A�j���[�V�����ɕύX
	m_anim.ChangeAnim(kIdleAnimName, true);
	m_state = &Enemy::IdleUpdate;
	(this->*m_state)(player);
}

void Enemy::IdleUpdate(std::shared_ptr<Player> player)
{
}
