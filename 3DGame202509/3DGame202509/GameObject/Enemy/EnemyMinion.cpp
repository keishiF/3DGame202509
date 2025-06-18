#include "EnemyMinion.h"
#include "Player/Player.h"

#include "CapsuleColliderData.h"
#include "Animation.h"

#include "DxLib.h"
#include <cassert>

namespace
{
	// �A�j���[�V������
	// �ҋ@
	const char* kFindAnimName   = "2H_Melee_Idle";
	// ����
	const char* kChaseAnimName  = "Running_C";
	// �U��
	const char* kAttackAnimName = "1H_Melee_Attack_Slice_Diagonal";
	// ��e
	const char* kHitAnimName    = "Hit_B";
	// ���S
	const char* kDeadAnimName   = "Death_B";

	// �G�l�~�[���v���C���[�𔭌��ł���͈�
	constexpr float kFindRadius   = 500.0f;
	constexpr float kAttackRadius = 100.0f;

	// ����HP
	constexpr int kHp = 10;

	// �G�l�~�[�̑��x
	constexpr float kSpeed = 1.0f;

	// �G�l�~�[�̓����蔻��p���a
	constexpr float kColRadius = 25.0f;

	constexpr float kAttackFrame = 32.0f;

	// ���f���̊g�嗦
	constexpr float kModelScale = 45.0f;
	constexpr float kBladeModelScale = 0.01f;
}

EnemyMinion::EnemyMinion()
{
	// �ʒu�̏�����
	m_pos = { 500.0f, 0.0f, 500.0f };
	m_vec = {   0.0f, 0.0f,   0.0f };
	m_colRadius = kColRadius;
	// �X�s�[�h�̏�����
	m_speed = kSpeed;
	m_findRadius = kFindRadius;
	m_attackRadius = kAttackRadius;
	m_hp = kHp;
	m_isDead = false;
	m_attackFrame = 0.0f;
	// �L�����ƕ���̃��f���̃��[�h
	m_charModel = MV1LoadModel("Data/Enemy/Minion/Minion.mv1");
	assert(m_charModel >= 0);
	m_weaponModel = MV1LoadModel("Data/Enemy/Minion/Blade.mv1");
	assert(m_weaponModel >= 0);
	// ���f���̊g��
	MV1SetScale(m_charModel, VGet(kModelScale, kModelScale, kModelScale));
	// ���f���̈ʒu�������̈ʒu�ƍ��킹��
	MV1SetPosition(m_charModel, m_pos.ToDxVECTOR());
	// �A�j���[�V�����Ǘ�
	m_anim.Init(m_charModel);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kFindAnimName, true);
}

EnemyMinion::~EnemyMinion()
{
}

void EnemyMinion::Update(std::shared_ptr<Player> player)
{
	// �A�j���[�V�����̍X�V
	m_anim.UpdateAnim(m_anim.GetPrevAnim());
	m_anim.UpdateAnim(m_anim.GetNextAnim());
	m_anim.UpdateAnimBlend();

	// �A�^�b�`���郂�f����MV1SetMatrix�̐ݒ�𖳌�������
	MV1SetMatrix(m_weaponModel, MGetIdent());
	// �A�^�b�`���郂�f���̃t���[���̍��W���擾����
	VECTOR position = MV1GetFramePosition(m_weaponModel, 0);
	// �A�^�b�`���郂�f����,�t���[���̍��W�����_�ɂ��邽�߂̕��s�ړ��s����쐬
	MATRIX transMat = MGetTranslate(VScale(position, -1.0f));
	// �A�^�b�`����郂�f���̃t���[���̍s����擾
	MATRIX frameMat = MV1GetFrameLocalWorldMatrix(m_charModel, 14);
	// �A�^�b�`���郂�f���̊g��s����擾
	MATRIX scaleMat = MGetScale(VGet(kBladeModelScale, kBladeModelScale, kBladeModelScale));
	// �A�^�b�`���郂�f���̉�]�s����擾
	MATRIX yMat     = MGetRotY(DX_PI_F);
	// �e�s�������
	MATRIX mixMat = MGetIdent();
	mixMat = MMult(transMat, mixMat);
	mixMat = MMult(frameMat, mixMat);
	mixMat = MMult(scaleMat, mixMat);
	mixMat = MMult(yMat, mixMat);
	// ���������s������f���ɃZ�b�g
	MV1SetMatrix(m_weaponModel, mixMat);

	switch (m_state)
	{
	case EnemyState::Find:
		FindUpdate(player);
		break;
	case EnemyState::Chase:
		ChaseUpdate(player);
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

void EnemyMinion::Draw()
{
#if _DEBUG
	DrawSphere3D(m_pos.ToDxVECTOR(), 10.0f, 16, 0x0000ff, 0x0000ff, true);
	DrawSphere3D(m_pos.ToDxVECTOR(), m_findRadius, 16, 0xff00ff, 0xff00ff, false);
	DrawSphere3D(m_pos.ToDxVECTOR(), m_attackRadius, 16, 0xff00ff, 0xff00ff, false);
	DrawCapsule3D(VGet(m_pos.x, m_pos.y + 20.0f, m_pos.z), VGet(m_pos.x, m_pos.y + 70.0f, m_pos.z),
		m_colRadius, 16, 0xff00ff, 0xff00ff, false);
#endif

	MV1DrawModel(m_charModel);
	MV1DrawModel(m_weaponModel);
}

const char* EnemyMinion::GetAnimName(EnemyState state) const
{
	switch (state)
	{
	case EnemyState::Find:   
		return kFindAnimName;
	case EnemyState::Chase:  
		return kChaseAnimName;
	case EnemyState::Attack: 
		return kAttackAnimName;
	case EnemyState::Hit:    
		return kHitAnimName;
	case EnemyState::Dead:   
		return kDeadAnimName;
	}
}

bool EnemyMinion::IsLoopAnim(EnemyState state) const
{
	switch (state)
	{
	case EnemyState::Find:
		return true;
	case EnemyState::Chase:
		return true;
	case EnemyState::Attack:
		return false;
	case EnemyState::Hit:
		return false;
	case EnemyState::Dead:
		return false;
	}
}

void EnemyMinion::FindUpdate(std::shared_ptr<Player> player)
{
	float distance = (m_pos - player->GetPos()).Length();
	if (distance <= (m_findRadius + player->GetRadius()))
	{
		ChangeState(EnemyState::Chase);
	}
}

void EnemyMinion::ChaseUpdate(std::shared_ptr<Player> player)
{
	// �v���C���[�ւ̕����x�N�g��
	m_vec = player->GetPos() - m_pos;
	m_vec.y = 0.0f; // Y�������̈ړ��𖳌����iXZ���ʂ݈̂ړ�����ꍇ�j

	// �������\���ɂ���ꍇ�݈̂ړ�
	if (m_vec.Length() > 1.0f) {
		m_vec.Normalize();
		m_pos += m_vec * m_speed;
		MV1SetPosition(m_charModel, m_pos.ToDxVECTOR());

		// �i�s������0�łȂ���Ή�]
		if (m_vec.x != 0.0f || m_vec.z != 0.0f)
		{
			// atan2��Y����]�p���v�Z�iZ���O�AX���E�̍��W�n�̏ꍇ�j
			float angleY = std::atan2(m_vec.x, -m_vec.z);
			MV1SetRotationXYZ(m_charModel, VGet(0.0f, -angleY, 0.0f));
		}
	}

	float distance = (m_pos - player->GetPos()).Length();
	if (distance >= (m_findRadius + player->GetRadius()))
	{
		ChangeState(EnemyState::Find);
	}

	if (distance <= (m_attackRadius + player->GetRadius()))
	{
		ChangeState(EnemyState::Attack);
	}
}

void EnemyMinion::AttackUpdate(std::shared_ptr<Player> player)
{
	if (m_attackFrame <= kAttackFrame)
	{
		++m_attackFrame;
		// �v���C���[�ւ̕����x�N�g��
		m_vec = player->GetPos() - m_pos;
		m_vec.y = 0.0f; // Y�������̈ړ��𖳌����iXZ���ʂ݈̂ړ�����ꍇ�j
		if (m_vec.x != 0.0f || m_vec.z != 0.0f)
		{
			// atan2��Y����]�p���v�Z�iZ���O�AX���E�̍��W�n�̏ꍇ�j
			float angleY = std::atan2(m_vec.x, -m_vec.z);
			MV1SetRotationXYZ(m_charModel, VGet(0.0f, -angleY, 0.0f));
		}
	}

	if (m_anim.GetNextAnim().isEnd)
	{
		float distance = (m_pos - player->GetPos()).Length();
		if (distance >= (m_findRadius + player->GetRadius()))
		{
			ChangeState(EnemyState::Find);
		}
		else
		{
			ChangeState(EnemyState::Chase);
		}
	}
}

void EnemyMinion::HitUpdate(std::shared_ptr<Player> player)
{
}

void EnemyMinion::DeadUpdate(std::shared_ptr<Player> player)
{
}
