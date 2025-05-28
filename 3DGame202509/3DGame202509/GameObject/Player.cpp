#include "Player.h"
#include <cassert>

namespace
{
	// �A�j���[�V������
	// �ҋ@
	const char* kIdleAnimName         = "Idle";
	// ����
	const char* kRunAnimName          = "Running_A";
	// �U��
	const char* kAttackAnimName       = "1H_Melee_Attack_Slice_Diagonal";
	// 4�������
	const char* kLeftDodgeAnimName    = "Dodge_Left";
	const char* kRightDodgeAnimName   = "Dodge_Right";
	const char* kForwardDodgeAnimName = "Dodge_Forward";
	const char* kBackDodgeAnimName    = "Dodge_Backward";

	// HP�̏����l
	constexpr int kHp = 100;
	// �ړ����x
	constexpr float kSpeed = 3.5f;
	// �W�����v��
	constexpr float kJumpSpeed = 20.0f;
	// �d��
	constexpr float kGravity = 0.75f;
	// �v���C���[�̃��f���̊g��l
	constexpr float kPlayerModelScale = 45.0f;
}

Player::Player() :
	m_model(-1),
	m_blendRate(0.0f),
	m_pos(0.0f, 0.0f, 0.0f),
	m_vec(0.0f, 0.0f, 0.0f),
	m_hp(kHp),
	m_jumpSpeed(kJumpSpeed),
	m_gravity(kGravity),
	m_isAttack(false),
	m_isRun(false),
	m_frameCount(0.0f)
{
	m_model = MV1LoadModel("Data/Model/Player/Player1.mv1");
	assert(m_model >= 0);
	AttachAnim(m_nextAnim, kIdleAnimName, true);
	m_blendRate = 1.0f;
	MV1SetScale(m_model, VGet(kPlayerModelScale, kPlayerModelScale, kPlayerModelScale));
}

Player::~Player()
{
	MV1DeleteModel(m_model);
}

void Player::Update(Input& input)
{
	UpdateAnim(m_prevAnim);
	UpdateAnim(m_nextAnim);
	UpdateAnimBlend();
	if (input.IsPress(PAD_INPUT_LEFT))
	{
		m_vec.x = -kSpeed;
	}
	else if (input.IsPress(PAD_INPUT_RIGHT))
	{
		m_vec.x = kSpeed;
	}
	else
	{
		m_vec.x = 0.0f;
	}

	if (input.IsPress(PAD_INPUT_UP))
	{
		m_vec.z = kSpeed;
	}
	else if (input.IsPress(PAD_INPUT_DOWN))
	{
		m_vec.z = -kSpeed;
	}
	else
	{
		m_vec.z = 0.0f;
	}

	// x�܂���z��0.0f����Ȃ��Ƃ���m_isRun��true�ɂ���
	m_isRun = (m_vec.x != 0.0f) || (m_vec.z != 0.0f);
	m_isAttack = input.IsTrigger(PAD_INPUT_1);

	// �����Ă���Ƃ��ɑ���A�j���[�V�����ɕύX����
	if (m_isRun)
	{
		ChangeAnim(kRunAnimName, false);
	}
	// �U�����Ă���Ƃ��ɍU���A�j���[�V�����ɕύX����
	if (m_isAttack)
	{
		ChangeAnim(kAttackAnimName, false);
	}
	// ����A�j���[�V�������I�������ҋ@�A�j���[�V�����ɖ߂�
	if (m_nextAnim.isEnd)
	{
		ChangeAnim(kIdleAnimName, true);
	}

	m_vec.Normalize();
	m_vec *= kSpeed;
	m_pos += m_vec;

	MV1SetPosition(m_model, VGet(m_pos.x, m_pos.y, m_pos.z));
	// �i�s������0�łȂ���Ή�]
	if (m_vec.x != 0.0f || m_vec.z != 0.0f)
	{
		// atan2��Y����]�p���v�Z�iZ���O�AX���E�̍��W�n�̏ꍇ�j
		float angleY = std::atan2(m_vec.x, -m_vec.z);
		MV1SetRotationXYZ(m_model, VGet(0.0f, -angleY, 0.0f));
	}

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

void Player::AttachAnim(AnimData& data, const char* animName, bool isLoop)
{
	// �A�^�b�`�������A�j���̔ԍ����擾
	int index = MV1GetAnimIndex(m_model, animName);
	// ���f���ɃA�j���[�V�������A�^�b�`
	data.attachNo = MV1AttachAnim(m_model, index, -1, false);
	// �A�j���J�E���^������
	data.frame = 0.0f;
	// �A�j���[�V�����̃��[�v�ݒ�
	data.isLoop = isLoop;
	// �񃋁[�v�A�j���̏I���t���O�𗎂Ƃ��Ă���
	data.isEnd = false;
}

void Player::UpdateAnim(AnimData& data)
{
	// �A�j���[�V�������A�^�b�`����Ă��Ȃ��ꍇ�͉������Ȃ�
	if (data.attachNo == -1)
	{
		return;
	}

	// �A�j���[�V������i�߂�
	data.frame += 0.5f;

	// ���ݍĐ����̃A�j���[�V�����̑����Ԃ��擾����
	const float totalTime = MV1GetAttachAnimTotalTime(m_model, data.attachNo);

	// �A�j���[�V�����̐ݒ�ɂ���ă��[�v�����邩�Ō�̃t���[���Ŏ~�߂邩�𔻒�
	if (data.isLoop)
	{
		// �A�j���[�V���������[�v������
		while (data.frame > totalTime)
		{
			data.frame -= totalTime;
		}
	}
	else
	{
		// �Ō�̃t���[���Œ�~������
		if (data.frame > totalTime)
		{
			data.frame = totalTime;
			data.isEnd = true;
		}
	}

	// �i�s�������A�j���[�V���������f���ɓK�p����
	MV1SetAttachAnimTime(m_model, data.attachNo, data.frame);
}

void Player::UpdateAnimBlend()
{
	// �����ɃA�j�����ݒ肳��Ă��Ȃ��ꍇ�͕ω������Ȃ�
	if (m_nextAnim.attachNo == -1) return;
	if (m_prevAnim.attachNo == -1) return;

	// m_blendRate��0.0f -> 1.0f�ɕω�������
	m_blendRate += 1.0f / 8.0f;
	if (m_blendRate > 1.0f) m_blendRate = 1.0f;

	MV1SetAttachAnimBlendRate(m_model, m_prevAnim.attachNo, 1.0f - m_blendRate);
	MV1SetAttachAnimBlendRate(m_model, m_nextAnim.attachNo, m_blendRate);
}

void Player::ChangeAnim(const char* animName, bool isLoop)
{
	// �ЂƂO�̃f�[�^�͍���Ǘ��ł��Ȃ��Ȃ�̂ł��炩���߃f�^�b�`���Ă���
	MV1DetachAnim(m_model, m_prevAnim.attachNo);

	// ���ݍĐ����̃A�j���[�V�����͈�Â��f�[�^�����ɂȂ�
	m_prevAnim = m_nextAnim;

	// �V���ɃV���b�g�̃A�j���[�V�������A�^�b�`����
	AttachAnim(m_nextAnim, animName, isLoop);

	// �A�j���̃u�����h����0.0>1.0�ɕω�����̂�0.0�ŏ�����
	m_blendRate = 0.0f;

	// m_blendRate���A�j���[�V�����ɓK�p����
	MV1SetAttachAnimBlendRate(m_model, m_prevAnim.attachNo, 1.0f - m_blendRate);
	MV1SetAttachAnimBlendRate(m_model, m_nextAnim.attachNo, m_blendRate);
}

