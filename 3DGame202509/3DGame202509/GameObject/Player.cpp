#include "Player.h"

#include "DxLib.h"
#include <cassert>

namespace
{
	// �A�j���[�V������
	// �ҋ@
	const char* kIdleAnimName         = "Idle";
	// ����
	const char* kWalkAnimName         = "Walking_B";
	// ����
	const char* kRunAnimName          = "Running_A";
	// �U��
	const char* kChopAnimName	      = "1H_Melee_Attack_Chop";
	const char* kSliceAnimName        = "1H_Melee_Attack_Slice_Diagonal";
	const char* kStabAnimName         = "1H_Melee_Attack_Stab";
	const char* kSpinAnimName         = "2H_Melee_Attack_Spin";
	const char* kUltimateAnimName      = "";
	// ���
	const char* kDodgeAnimName		  = "Dodge_Forward";
	// ��e
	const char* kHitAnimName		  = "Hit_B";
	// ���S
	const char* kDeadAnimName         = "Death_B";

	// �v���C���[�̓����蔻��
	constexpr float kRadius = 30.0f;
	// HP�̏����l
	constexpr int kHp = 100;
	// �ړ����x
	constexpr float kWalkSpeed = 1.5f;
	constexpr float kRunSpeed = 4.5f;
	// �v���C���[�̃��f���̊g��l
	constexpr float kModelScale = 45.0f;
}

Player::Player() :
	m_model(-1),
	m_pos(0.0f, 0.0f, 0.0f),
	m_radius(kRadius),
	m_vec(0.0f, 0.0f, 0.0f),
	m_hp(kHp),
	m_isCombo(false),
	m_isDead(false),
	m_frameCount(0.0f),
	m_state(PlayerState::Idle)
{
	m_model = MV1LoadModel("Data/Player/Player.mv1");
	assert(m_model >= 0);
	MV1SetScale(m_model, VGet(kModelScale, kModelScale, kModelScale));
	MV1SetPosition(m_model, VGet(m_pos.x, m_pos.y, m_pos.z));

	m_anim.Init(m_model);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kIdleAnimName, true);
}

Player::~Player()
{
	MV1DeleteModel(m_model);
}

void Player::Update(Input& input)
{	
	// �A�j���[�V�����̍X�V
	m_anim.UpdateAnim(m_anim.GetPrevAnim());
	m_anim.UpdateAnim(m_anim.GetNextAnim());
	m_anim.UpdateAnimBlend();

	switch (m_state)
	{
	case PlayerState::Idle:
		IdleUpdate(input);
		break;
	case PlayerState::Walk:
		WalkUpdate(input);
		break;
	case PlayerState::Run:
		RunUpdate(input);
		break;
	case PlayerState::Chop:
		ChopUpdate(input);
		break;
	case PlayerState::Slice:
		SliceUpdate(input);
		break;
	case PlayerState::Stab:
		StabUpdate(input);
		break;
	case PlayerState::Spin:
		SpinUpdate(input);
		break;
	case PlayerState::Ultimate:
		UltimateUpdate(input);
		break;
	case PlayerState::Dodge:
		DodgeUpdate(input);
		break;
	case PlayerState::Hit:
		HitUpdate(input);
		break;
	case PlayerState::Dead:
		DeadUpdate(input);
		break;
	}
}

void Player::Draw()
{
#if _DEBUG
	DrawSphere3D(VGet(m_pos.x, m_pos.y, m_pos.z), 10.0f, 16, 0x0000ff, 0x0000ff, true);
	//DrawSphere3D(VGet(m_pos.x, m_pos.y, m_pos.z), m_radius, 16, 0xff00ff, 0xff00ff, false);
#endif

	MV1DrawModel(m_model);
}

void Player::OnDamage()
{
	m_hp--;
}

void Player::ChangeState(PlayerState newState)
{
	// ���݂̏�ԂƎ��̏�Ԃ������ꍇreturn
	if (m_state == newState) return;

	m_state = newState;

	switch (m_state)
	{
	case PlayerState::Idle:
		m_anim.ChangeAnim(kIdleAnimName, true);
		break;
	case PlayerState::Walk:
		m_anim.ChangeAnim(kWalkAnimName, true);
		break;
	case PlayerState::Run:
		m_anim.ChangeAnim(kRunAnimName, true);
		break;
	case PlayerState::Chop:
		m_anim.ChangeAnim(kChopAnimName, false);
		break;
	case PlayerState::Slice:
		m_anim.ChangeAnim(kSliceAnimName, false);
		break;
	case PlayerState::Stab:
		m_anim.ChangeAnim(kStabAnimName, false);
		break;
	case PlayerState::Spin:
		m_anim.ChangeAnim(kSpinAnimName, false);
		break;
	case PlayerState::Ultimate:
		m_anim.ChangeAnim(kUltimateAnimName, false);
		break;
	case PlayerState::Dodge:
		m_anim.ChangeAnim(kDodgeAnimName, false);
		break;
	case PlayerState::Hit:
		m_anim.ChangeAnim(kHitAnimName, false);
		break;
	case PlayerState::Dead:
		m_anim.ChangeAnim(kDeadAnimName, false);
		break;
	}
}

void Player::IdleUpdate(Input& input)
{
	// ���X�e�B�b�N�̓��͂�����Ε�����ԂɈڍs����
	if (input.IsPress("LEFT") || input.IsPress("RIGHT") ||
		input.IsPress("UP")   || input.IsPress("DOWN"))
	{
		ChangeState(PlayerState::Walk);
	}

	// A�{�^���̓��͂�����΍U����ԂɈڍs����
	if (input.IsTrigger("A"))
	{
		ChangeState(PlayerState::Chop);
	}

	// X�{�^���̓��͂�����΋��U����ԂɈڍs���邽�߂̃t���O�𗧂Ă�
	if (input.IsTrigger("X"))
	{
		ChangeState(PlayerState::Spin);
	}
#if _DEBUG
	// ���S��ԂɈڍs
	if (input.IsTrigger("LB"))
	{
		ChangeState(PlayerState::Dead);
	}
#endif
}

void Player::WalkUpdate(Input& input)
{
	// ���X�e�B�b�N�ňړ�
	// ������
	if (input.IsPress("LEFT"))
	{
		m_vec.x = -kWalkSpeed;
	}
	// �E����
	else if (input.IsPress("RIGHT"))
	{
		m_vec.x = kWalkSpeed;
	}
	// �������̓��͂Ȃ�
	else
	{
		m_vec.x = 0.0f;
	}
	// �����
	if (input.IsPress("UP"))
	{
		m_vec.z = kWalkSpeed;
	}
	// ������
	else if (input.IsPress("DOWN"))
	{
		m_vec.z = -kWalkSpeed;
	}
	// �c�����̓��͂Ȃ�
	else
	{
		m_vec.z = 0.0f;
	}

	// �x�N�g���𐳋K�����ړ����x�������|�W�V�����ɉ��Z
	m_vec.Normalize();
	m_vec *= kWalkSpeed;
	m_pos += m_vec;
	MV1SetPosition(m_model, VGet(m_pos.x, m_pos.y, m_pos.z));

	// �i�s������0�łȂ���Ή�]
	if (m_vec.x != 0.0f || m_vec.z != 0.0f)
	{
		// atan2��Y����]�p���v�Z�iZ���O�AX���E�̍��W�n�̏ꍇ�j
		float angleY = std::atan2(m_vec.x, -m_vec.z);
		MV1SetRotationXYZ(m_model, VGet(0.0f, -angleY, 0.0f));
	}

	// ���X�e�B�b�N�̓��͂��Ȃ��ꍇ�ҋ@��ԂɈڍs
	if (m_vec.x == 0.0f && m_vec.z == 0.0f)
	{
		ChangeState(PlayerState::Idle);
	}

	// L�X�e�B�b�N�������݂̓��͂�����΃_�b�V����ԂɈڍs����
	if (input.IsTrigger("LPush"))
	{
		ChangeState(PlayerState::Run);
	}

	// A�{�^���̓��͂�����΍U����ԂɈڍs����
	if (input.IsTrigger("A"))
	{
		ChangeState(PlayerState::Chop);
	}

	// X�{�^���̓��͂�����΍����U����ԂɈڍs����
	if (input.IsTrigger("X"))
	{
		ChangeState(PlayerState::Spin);
	}

	// B�{�^���̓��͂�����Ή����ԂɈڍs����
	if (input.IsTrigger("B"))
	{
		ChangeState(PlayerState::Dodge);
	}
}

void Player::RunUpdate(Input& input)
{
	// ���X�e�B�b�N�ňړ�
	// ������
	if (input.IsPress("LEFT"))
	{
		m_vec.x = -kRunSpeed;
	}
	// �E����
	else if (input.IsPress("RIGHT"))
	{
		m_vec.x = kRunSpeed;
	}
	// �������̓��͂Ȃ�
	else
	{
		m_vec.x = 0.0f;
	}
	// �����
	if (input.IsPress("UP"))
	{
		m_vec.z = kRunSpeed;
	}
	// ������
	else if (input.IsPress("DOWN"))
	{
		m_vec.z = -kRunSpeed;
	}
	// �c�����̓��͂Ȃ�
	else
	{
		m_vec.z = 0.0f;
	}

	// �x�N�g���𐳋K�����ړ����x�������|�W�V�����ɉ��Z
	m_vec.Normalize();
	m_vec *= kRunSpeed;
	m_pos += m_vec;
	MV1SetPosition(m_model, VGet(m_pos.x, m_pos.y, m_pos.z));

	// �i�s������0�łȂ���Ή�]
	if (m_vec.x != 0.0f || m_vec.z != 0.0f)
	{
		// atan2��Y����]�p���v�Z�iZ���O�AX���E�̍��W�n�̏ꍇ�j
		float angleY = std::atan2(m_vec.x, -m_vec.z);
		MV1SetRotationXYZ(m_model, VGet(0.0f, -angleY, 0.0f));
	}

	// ���X�e�B�b�N�̓��͂��Ȃ��ꍇ�ҋ@��ԂɈڍs����
	if (m_vec.x == 0.0f && m_vec.z == 0.0f)
	{
		ChangeState(PlayerState::Idle);
	}

	// ���X�e�B�b�N�������݂̓��͂�����Ε�����ԂɈڍs����
	if (input.IsTrigger("LPush"))
	{
		ChangeState(PlayerState::Walk);
	}

	// A�{�^���̓��͂�����΍U����ԂɈڍs����
	if (input.IsTrigger("A"))
	{
		ChangeState(PlayerState::Chop);
	}

	// X�{�^���̓��͂�����΋��U����ԂɈڍs����
	if (input.IsTrigger("X"))
	{
		ChangeState(PlayerState::Spin);
	}

	// B�{�^���̓��͂�����Ή����ԂɈڍs����
	if (input.IsTrigger("B"))
	{
		ChangeState(PlayerState::Dodge);
	}
}

void Player::ChopUpdate(Input& input)
{
	// 1�{�^���̓��͂�����΍U����ԂɈڍs����
	if (input.IsTrigger("A"))
	{
		m_isCombo = true;
	}

	// �A�j���[�V�������I����
	if (m_anim.GetNextAnim().isEnd)
	{
		// 1�{�^���̓��͂�����΍U����ԂɈڍs����
		if (m_isCombo)
		{
			ChangeState(PlayerState::Slice);
			m_isCombo = false;
		}
		// �����Ȃ���Αҋ@��ԂɈڍs����
		else
		{
			ChangeState(PlayerState::Idle);
		}
	}
}

void Player::SliceUpdate(Input& input)
{
	// 1�{�^���̓��͂�����΍U����ԂɈڍs����
	if (input.IsTrigger("A"))
	{
		m_isCombo = true;
	}

	// �A�j���[�V�������I��������ҋ@��Ԃɖ߂�
	if (m_anim.GetNextAnim().isEnd)
	{
		// 1�{�^���̓��͂�����΍U����ԂɈڍs����
		if (m_isCombo)
		{
			ChangeState(PlayerState::Stab);
			m_isCombo = false;
		}
		else
		{
			ChangeState(PlayerState::Idle);
		}
	}
}

void Player::StabUpdate(Input& input)
{
	// 1�{�^���̓��͂�����΍U����ԂɈڍs����
	if (input.IsTrigger("A"))
	{
		m_isCombo = true;
	}

	// �A�j���[�V�������I��������ҋ@��Ԃɖ߂�
	if (m_anim.GetNextAnim().isEnd)
	{
		// 1�{�^���̓��͂�����΍U����ԂɈڍs����
		if (m_isCombo)
		{
			ChangeState(PlayerState::Chop);
			m_isCombo = false;
		}
		else
		{
			ChangeState(PlayerState::Idle);
		}
	}
}

void Player::SpinUpdate(Input& input)
{
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::UltimateUpdate(Input& input)
{
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::DodgeUpdate(Input& input)
{
	// �A�j���[�V�������I��������ҋ@��Ԃɖ߂�
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::HitUpdate(Input& input)
{
	// �A�j���[�V�������I��������ҋ@��Ԃɖ߂�
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::DeadUpdate(Input& input)
{
	// �A�j���[�V�������I��������ҋ@��Ԃɖ߂�
	if (m_anim.GetNextAnim().isEnd)
	{
		m_isDead = true;
	}
}

