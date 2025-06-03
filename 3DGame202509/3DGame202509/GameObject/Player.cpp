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
	// ���
	const char* kDodgeAnimName		  = "Dodge_Forward";
	// ��e
	const char* kHitAnimName		  = "Hit_B";
	// ���S
	const char* kDeadAnimName         = "Death_B";

	// HP�̏����l
	constexpr int kHp = 100;
	// �ړ����x
	constexpr float kWalkSpeed = 1.5f;
	constexpr float kRunSpeed = 4.5f;
	// �v���C���[�̃��f���̊g��l
	constexpr float kPlayerModelScale = 45.0f;
}

Player::Player() :
	m_model(-1),
	m_pos(0.0f, 0.0f, 0.0f),
	m_vec(0.0f, 0.0f, 0.0f),
	m_hp(kHp),
	m_isWalk(false),
	m_isRun(false),
	m_isChop(false),
	m_isSlice(false),
	m_isStab(false),
	m_isSpin(false),
	m_isDodge(false),
	m_isDead(false),
	m_frameCount(0.0f),
	m_walkFrameCount(0.0f),
	m_blendRate(0.0f),
	m_state(&Player::IdleInit)
{
	m_model = MV1LoadModel("Data/Model/Player/Player.mv1");
	assert(m_model >= 0);
	MV1SetScale(m_model, VGet(kPlayerModelScale, kPlayerModelScale, kPlayerModelScale));

	m_anim.Init(m_model);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kIdleAnimName, true);
	m_blendRate = 1.0f;
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
	(this->*m_state)(input);
}

void Player::Draw()
{
#if _DEBUG
	DrawSphere3D(VGet(m_pos.x, m_pos.y, m_pos.z), 10.0f, 16, 0x0000ff, 0x0000ff, true);
#endif

	MV1DrawModel(m_model);
}

void Player::OnDamage()
{
	m_hp--;
}

void Player::IdleInit(Input& input)
{
	// �ҋ@�A�j���[�V�����ɕύX
	m_anim.ChangeAnim(kIdleAnimName, true);
	m_state = &Player::IdleUpdate;
	(this->*m_state)(input);
}

void Player::IdleUpdate(Input& input)
{
	// ���X�e�B�b�N�̓��͂�����Α����ԂɈڍs���邽�߂̃t���O�𗧂Ă�
	if (input.IsPress("LEFT") || input.IsPress("RIGHT") ||
		input.IsPress("UP")   || input.IsPress("DOWN"))
	{
		m_isWalk = true;
	}

	// A�{�^���̓��͂�����΍U����ԂɈڍs���邽�߂̃t���O�𗧂Ă�
	if (input.IsTrigger("A"))
	{
		m_isChop = true;
	}

	// X�{�^���̓��͂�����΋��U����ԂɈڍs���邽�߂̃t���O�𗧂Ă�
	if (input.IsTrigger("X"))
	{
		m_isSpin = true;
	}

	// �����ԂɈڍs
	if (m_isWalk)
	{
		m_state = &Player::WalkInit;
	}
	// �U����ԂɈڍs
	if (m_isChop)
	{
		m_state = &Player::ChopInit;
	}
	// ���U����ԂɈڍs
	if (m_isSpin)
	{
		m_state = &Player::SpinInit;
	}
#if _DEBUG
	// ���S��ԂɈڍs
	if (input.IsTrigger("LB"))
	{
		m_state = &Player::DeadInit;
	}
#endif
}

void Player::WalkInit(Input& input)
{
	// �ҋ@�A�j���[�V�����ɕύX
	m_anim.ChangeAnim(kWalkAnimName, true);
	m_state = &Player::WalkUpdate;
	m_walkFrameCount = 0.0f;
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

	// ���t���[���o�߂����瑖���ԂɈڍs���邽�߂̃t���O�𗧂Ă�
	++m_walkFrameCount;
	if (m_walkFrameCount > 60.0f || input.IsTrigger("LPush"))
	{
		m_isRun = true;
	}

	// A�{�^���̓��͂�����΍U����ԂɈڍs���邽�߂̃t���O�𗧂Ă�
	if (input.IsTrigger("A"))
	{
		m_isChop = true;
	}

	// X�{�^���̓��͂�����΍����U����ԂɈڍs���邽�߂̃t���O�𗧂Ă�
	if (input.IsTrigger("X"))
	{
		m_isSpin = true;
	}

	// B�{�^���̓��͂�����Ή����ԂɈڍs���邽�߂̃t���O�𗧂Ă�
	if (input.IsTrigger("B"))
	{
		m_isDodge = true;
	}

	// ���X�e�B�b�N�̓��͂��Ȃ��ꍇ�ҋ@��ԂɈڍs
	if (m_vec.x == 0.0f && m_vec.z == 0.0f)
	{
		m_state  = &Player::IdleInit;
		m_isWalk = false;
	}

	// �����ԂɈڍs
	if (m_isRun)
	{
		m_state  = &Player::RunInit;
		m_isWalk = false;
		m_walkFrameCount = 0.0f;
	}

	// �U����ԂɈڍs
	if (m_isChop)
	{
		m_state  = &Player::ChopInit;
		m_isWalk = false;
		m_walkFrameCount = 0.0f;
	}

	// ���U����ԂɈڍs
	if (m_isSpin)
	{
		m_state  = &Player::SpinInit;
		m_isWalk = false;
		m_walkFrameCount = 0.0f;
	}
	// �����ԂɈڍs
	if (m_isDodge)
	{
		m_state  = &Player::DodgeInit;
		m_isWalk = false;
		m_walkFrameCount = 0.0f;
	}
}

void Player::RunInit(Input& input)
{
	// ����A�j���[�V�����ɕύX
	m_anim.ChangeAnim(kRunAnimName, true);
	m_state = &Player::RunUpdate;
	(this->*m_state)(input);
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

	if (input.IsTrigger("LPush"))
	{
		m_isWalk = true;
	}

	// A�{�^���̓��͂�����΍U����ԂɈڍs���邽�߂̃t���O�𗧂Ă�
	if (input.IsTrigger("A"))
	{
		m_isChop = true;
	}

	// X�{�^���̓��͂�����΋��U����ԂɈڍs���邽�߂̃t���O�𗧂Ă�
	if (input.IsTrigger("X"))
	{
		m_isSpin = true;
	}

	// B�{�^���̓��͂�����Ή����ԂɈڍs���邽�߂̃t���O�𗧂Ă�
	if (input.IsTrigger("B"))
	{
		m_isDodge = true;
	}
	
	// ���X�e�B�b�N�̓��͂��Ȃ��ꍇ�ҋ@��ԂɈڍs
	if (m_vec.x == 0.0f && m_vec.z == 0.0f)
	{
		m_state = &Player::IdleInit;
		m_isRun = false;
	}

	// ������ԂɈڍs
	if (m_isWalk)
	{
		m_state = &Player::WalkInit;
		m_isRun = false;
	}

	// �U����ԂɈڍs
	if (m_isChop)
	{
		m_state = &Player::ChopInit;
		m_isRun = false;
	}

	// ���U����ԂɈڍs
	if (m_isSpin)
	{
		m_state = &Player::SpinInit;
		m_isRun = false;
	}

	// �����ԂɈڍs
	if (m_isDodge)
	{
		m_state = &Player::DodgeInit;
		m_isRun = false;
	}
}

void Player::ChopInit(Input& input)
{
	// �U���A�j���[�V�����ɕύX
	m_anim.ChangeAnim(kChopAnimName, false);
	m_state = &Player::ChopUpdate;
	(this->*m_state)(input);
}

void Player::ChopUpdate(Input& input)
{
	// 1�{�^���̓��͂�����΍U����ԂɈڍs���邽�߂̃t���O�𗧂Ă�
	if (input.IsTrigger("A"))
	{
		m_isSlice = true;
	}

	// �A�j���[�V�������I��������ҋ@��Ԃɖ߂�
	if (m_anim.GetNextAnim().isEnd)
	{
		if (m_isSlice)
		{
			m_state = &Player::SliceInit;
			m_isChop = false;
		}
		else
		{
			m_state = &Player::IdleInit;
			m_isChop = false;
		}
	}
}

void Player::SliceInit(Input& input)
{
	// �U���A�j���[�V�����ɕύX
	m_anim.ChangeAnim(kSliceAnimName, false);
	m_state = &Player::SliceUpdate;
	(this->*m_state)(input);
}

void Player::SliceUpdate(Input& input)
{
	// 1�{�^���̓��͂�����΍U����ԂɈڍs���邽�߂̃t���O�𗧂Ă�
	if (input.IsTrigger("A"))
	{
		m_isStab = true;
	}

	// �A�j���[�V�������I��������ҋ@��Ԃɖ߂�
	if (m_anim.GetNextAnim().isEnd)
	{
		if (m_isStab)
		{
			m_state = &Player::StabInit;
			m_isSlice = false;
		}
		else
		{
			m_state = &Player::IdleInit;
			m_isSlice = false;
		}
	}
}

void Player::StabInit(Input& input)
{
	// �U���A�j���[�V�����ɕύX
	m_anim.ChangeAnim(kStabAnimName, false);
	m_state = &Player::StabUpdate;
	(this->*m_state)(input);
}

void Player::StabUpdate(Input& input)
{
	// 1�{�^���̓��͂�����΍U����ԂɈڍs���邽�߂̃t���O�𗧂Ă�
	if (input.IsTrigger("A"))
	{
		m_isChop = true;
	}

	// �A�j���[�V�������I��������ҋ@��Ԃɖ߂�
	if (m_anim.GetNextAnim().isEnd)
	{
		if (m_isChop)
		{
			m_state = &Player::ChopInit;
			m_isStab = false;
		}
		else
		{
			m_state = &Player::IdleInit;
			m_isStab = false;
		}
	}
}

void Player::SpinInit(Input& input)
{
	// ���U���A�j���[�V�����ɕύX
	m_anim.ChangeAnim(kSpinAnimName, false);
	m_state = &Player::SpinUpdate;
}

void Player::SpinUpdate(Input& input)
{
	if (m_anim.GetNextAnim().isEnd)
	{
		m_state = &Player::IdleInit;
		m_isSpin = false;
	}
}

void Player::DodgeInit(Input& input)
{
	// ����A�j���[�V�����ɕύX
	m_anim.ChangeAnim(kDodgeAnimName, false);
	m_state = &Player::DodgeUpdate;
	(this->*m_state)(input);
}

void Player::DodgeUpdate(Input& input)
{
	// �A�j���[�V�������I��������ҋ@��Ԃɖ߂�
	if (m_anim.GetNextAnim().isEnd)
	{
		m_state = &Player::IdleInit;
		m_isDodge = false;
	}
}

void Player::HitInit(Input& input)
{
	// ����A�j���[�V�����ɕύX
	m_anim.ChangeAnim(kHitAnimName, false);
	m_state = &Player::HitUpdate;
	(this->*m_state)(input);
}

void Player::HitUpdate(Input& input)
{
	// �A�j���[�V�������I��������ҋ@��Ԃɖ߂�
	if (m_anim.GetNextAnim().isEnd)
	{
		m_state = &Player::IdleInit;
	}
}

void Player::DeadInit(Input& input)
{
	// ����A�j���[�V�����ɕύX
	m_anim.ChangeAnim(kDeadAnimName, false);
	m_state = &Player::DeadUpdate;
	(this->*m_state)(input);
}

void Player::DeadUpdate(Input& input)
{
	// �A�j���[�V�������I��������ҋ@��Ԃɖ߂�
	if (m_anim.GetNextAnim().isEnd)
	{
		m_isDead = true;
	}
}

