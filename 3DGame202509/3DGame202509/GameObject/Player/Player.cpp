#include "Player.h"
#include "PlayerWeapon.h"

#include "Animator.h"
#include "CapsuleColliderData.h"
#include "Physics.h"

#include "DxLib.h"
#include <algorithm>
#include <cassert>
#include <unordered_map>

namespace
{
	// HP�̏����l
	constexpr int kHp = 100;
	// �ړ����x
	constexpr float kWalkSpeed = 1.5f;
	constexpr float kRunSpeed = 5.0f;
	constexpr float kAttackMoveSpeed = 0.75f;
	// �v���C���[�̃��f���̊g��l
	constexpr float kModelScale = 45.0f;
	// �v���C���[�̓����蔻��
	constexpr float kRadius = 15.0f;
	constexpr float kColScale = 90.0f;
	constexpr float kAttackOffsetRadius = 100.0f;

	// �A�j���[�V������
	// �ҋ@
	const char* kIdleAnimName = "Idle";
	// ����
	const char* kWalkAnimName         = "Walking_B";
	// ����
	const char* kRunAnimName          = "Running_A";
	// �U��
	const char* kChopAnimName	      = "1H_Melee_Attack_Chop";
	const char* kSliceAnimName        = "1H_Melee_Attack_Slice_Diagonal";
	const char* kStabAnimName         = "1H_Melee_Attack_Stab";
	const char* kSpinAnimName         = "2H_Melee_Attack_Spin";
	const char* kUltimateAnimName     = "";
	// ���
	const char* kDodgeAnimName		  = "Dodge_Forward";
	// ��e
	const char* kHitAnimName		  = "Hit_B";
	// ���S
	const char* kDeadAnimName         = "Death_B";
	// �A�j���[�V�����̍Đ����x
	constexpr float kAnimSpeed = 1.0f;
	constexpr float kIdleAnimSpeed = 0.5f;
	constexpr float kWalkAnimSpeed = 0.75f;
	constexpr float kChopAnimSpeed = 1.0f;

	const std::unordered_map<PlayerState, AttackTiming> kColTimingTable =
	{
		{PlayerState::Idle,		 { 0,  0}},
		{PlayerState::Walk,		 { 0,  0}},
		{PlayerState::Run,		 { 0,  0}},
		{PlayerState::Chop,		 {16, 28}},
		{PlayerState::Slice,	 {16, 28}},
		{PlayerState::Stab,		 {16, 28}},
		{PlayerState::Spin,      {16, 36}},
		{PlayerState::Ultimate,  {16, 36}},
		{PlayerState::Dodge,	 { 0,  0}},
		{PlayerState::Hit,		 { 0,  0}},
		{PlayerState::Dead,		 { 0,  0}}
	};
}

Player::Player() :
	m_charModel(-1),
	m_radius(kRadius),
	m_hp(kHp),
	m_isCombo(false),
	m_isDead(false),
	m_attackFrame(0.0f),
	m_state(PlayerState::Idle),
	Collidable(ObjectTag::Player, ObjectPriority::High, ColliderData::Kind::Capsule)
{
}

Player::~Player()
{
	MV1DeleteModel(m_charModel);
}

void Player::Init(std::shared_ptr<Physics> physics)
{
	Collidable::Init(physics);
	Vec3 pos = { 0.0f, 0.0f, -500.0f };
	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);

	//�����蔻��
	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->m_startPos = pos;
	colData->m_radius = kRadius;

	m_charModel = MV1LoadModel("Data/Model/Player/Player.mv1");
	assert(m_charModel >= 0);
	MV1SetScale(m_charModel, VGet(kModelScale, kModelScale, kModelScale));
	MV1SetPosition(m_charModel, pos.ToDxVECTOR());

	m_anim.Init(m_charModel);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kIdleAnimName, kIdleAnimSpeed, true);

	m_weapon = std::make_shared<PlayerWeapon>();
	m_weapon->Init(physics);
}

void Player::Init(std::shared_ptr<Physics> physics, Vec3& pos, const Vec3& rot, const Vec3& scale)
{
	Collidable::Init(physics);

	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);

	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->m_startPos = pos;
	colData->m_radius = kRadius;

	m_charModel = MV1LoadModel("Data/Model/Player/Player.mv1");
	assert(m_charModel >= 0);

	MV1SetScale(m_charModel, VGet(scale.x * 100.0f, scale.y * 100.0f, scale.z * 100.0f));
	MV1SetPosition(m_charModel, pos.ToDxVECTOR());

	m_anim.Init(m_charModel);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kIdleAnimName, kIdleAnimSpeed, true);

	m_weapon = std::make_shared<PlayerWeapon>();
	m_weapon->Init(physics);
}

void Player::Update()
{	
	if (m_isDead && m_charModel < 0)
	{
		return;
	}

	// �A�j���[�V�����̍X�V
	m_anim.UpdateAnim(m_anim.GetPrevAnim());
	m_anim.UpdateAnim(m_anim.GetNextAnim());
	m_anim.UpdateAnimBlend();

	switch (m_state)
	{
	case PlayerState::Idle:
		IdleUpdate();
		break;
	case PlayerState::Walk:
		WalkUpdate();
		break;
	case PlayerState::Run:
		RunUpdate();
		break;
	case PlayerState::Chop:
		ChopUpdate();
		break;
	case PlayerState::Slice:
		SliceUpdate();
		break;
	case PlayerState::Stab:
		StabUpdate();
		break;
	case PlayerState::Spin:
		SpinUpdate();
		break;
	case PlayerState::Ultimate:
		UltimateUpdate();
		break;
	case PlayerState::Dodge:
		DodgeUpdate();
		break;
	case PlayerState::Hit:
		HitUpdate();
		break;
	case PlayerState::Dead:
		DeadUpdate();
		break;
	}

	//�����蔻��
	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	Vec3 colPos = m_rigidbody.GetPos();
	colPos.y += kColScale;
	colData->m_startPos = colPos;
}

void Player::Draw()
{
	if (m_isDead && m_charModel < 0)
	{
		return;
	}

#if _DEBUG
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), m_radius, 16, 0x00ff00, 0x00ff00, false);
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), kAttackOffsetRadius, 16, 0xff0000, 0xff0000, false);
#endif
	MV1DrawModel(m_charModel);
	m_weapon->Draw();

	const int gaugeWidth = 200;
	const int gaugeHeight = 20;
	const int gaugeX = 50;
	const int gaugeY = 50;

	// HP�̊���
	float hpRate = static_cast<float>(m_hp) / kHp;
	hpRate = std::clamp(hpRate, 0.0f, 1.0f);

	// �Q�[�W�F�̌���
	int color;
	if (hpRate > 0.5f) 
	{
		color = 0x00ff00;
	}
	else if (hpRate > 0.25f) 
	{
		color = 0xffff00;
	}
	else 
	{
		color = 0xff0000;
	}

	// �Q�[�W�w�i�i�D�F�j
	DrawBox(gaugeX, gaugeY,
		gaugeX + gaugeWidth,
		gaugeY + gaugeHeight,
		0x808080, true);

	// ���݂�HP���̒����̃Q�[�W
	int hpBarWidth = static_cast<int>(gaugeWidth * hpRate);
	DrawBox(gaugeX, gaugeY,
		gaugeX + hpBarWidth,
		gaugeY + gaugeHeight,
		color, true);

	// �g���i���j
	DrawBox(gaugeX, gaugeY,
		gaugeX + gaugeWidth,
		gaugeY + gaugeHeight,
		0x000000, false);
}

void Player::OnDamage()
{
	m_hp -= 10;

	if (m_hp <= 0 && !m_isDead)
	{
		ChangeState(PlayerState::Dead);
	}
	else
	{
		ChangeState(PlayerState::Hit);
	}
}

void Player::OnCollide(std::shared_ptr<Collidable> collider)
{
	OnDamage();
}

void Player::ChangeState(PlayerState newState)
{
	// ���݂̏�ԂƎ��̏�Ԃ������ꍇreturn
	// Hit������O����
	if (m_state == newState && m_state != PlayerState::Hit) return;

	m_state = newState;

	m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
	m_attackFrame = 0.0f;
	m_isCombo = false;

	switch (m_state)
	{
	case PlayerState::Idle:
		m_anim.ChangeAnim(kIdleAnimName, kIdleAnimSpeed, true);
		break;
	case PlayerState::Walk:
		m_anim.ChangeAnim(kWalkAnimName, kWalkAnimSpeed, true);
		break;
	case PlayerState::Run:
		m_anim.ChangeAnim(kRunAnimName, kAnimSpeed, true);
		break;
	case PlayerState::Chop:
		m_anim.ChangeAnim(kChopAnimName, kChopAnimSpeed, false);
		break;
	case PlayerState::Slice:
		m_anim.ChangeAnim(kSliceAnimName, kAnimSpeed, false);
		break;
	case PlayerState::Stab:
		m_anim.ChangeAnim(kStabAnimName, kAnimSpeed, false);
		break;
	case PlayerState::Spin:
		m_anim.ChangeAnim(kSpinAnimName, kAnimSpeed, false);
		break;
	case PlayerState::Ultimate:
		m_anim.ChangeAnim(kUltimateAnimName, kAnimSpeed, false);
		break;
	case PlayerState::Dodge:
		m_anim.ChangeAnim(kDodgeAnimName, kAnimSpeed, false);
		break;
	case PlayerState::Hit:
		m_anim.ChangeAnim(kHitAnimName, kAnimSpeed, false);
		break;
	case PlayerState::Dead:
		m_anim.ChangeAnim(kDeadAnimName, kAnimSpeed, false);
		break;
	}
}

void Player::IdleUpdate()
{
	SetActive(true);
	m_weapon->Update(m_charModel, m_attackFrame, kColTimingTable.at(PlayerState::Idle));

	// ���X�e�B�b�N�̓��͂�����Ε�����ԂɈڍs����
	if (Input::Instance().IsPress("LEFT") || Input::Instance().IsPress("RIGHT") ||
		Input::Instance().IsPress("UP")   || Input::Instance().IsPress("DOWN"))
	{
		ChangeState(PlayerState::Walk);
	}

	// A�{�^���̓��͂�����΍U����ԂɈڍs����
	if (Input::Instance().IsTrigger("A"))
	{
		ChangeState(PlayerState::Chop);
	}

	// X�{�^���̓��͂�����΋��U����ԂɈڍs���邽�߂̃t���O�𗧂Ă�
	if (Input::Instance().IsTrigger("X"))
	{
		ChangeState(PlayerState::Spin);
	}
#if _DEBUG
	// ���S��ԂɈڍs
	if (Input::Instance().IsTrigger("LB"))
	{
		ChangeState(PlayerState::Dead);
	}
#endif
}

void Player::WalkUpdate()
{
	SetActive(true);
	m_weapon->Update(m_charModel, m_attackFrame, kColTimingTable.at(PlayerState::Walk));

	Vec3 dir = { 0.0f, 0.0f,0.0f };
	// ���X�e�B�b�N�ňړ�
	// ������
	if (Input::Instance().IsPress("LEFT"))
	{
		dir.x = -kWalkSpeed;
	}
	// �E����
	else if (Input::Instance().IsPress("RIGHT"))
	{
		dir.x = kWalkSpeed;
	}
	// �������̓��͂Ȃ�
	else
	{
		dir.x = 0.0f;
	}
	// �����
	if (Input::Instance().IsPress("UP"))
	{
		dir.z = kWalkSpeed;
	}
	// ������
	else if (Input::Instance().IsPress("DOWN"))
	{
		dir.z = -kWalkSpeed;
	}
	// �c�����̓��͂Ȃ�
	else
	{
		dir.z = 0.0f;
	}

	// �x�N�g���𐳋K�����ړ����x�������|�W�V�����ɉ��Z
	dir.Normalize();
	m_rigidbody.SetVelo(dir * kWalkSpeed);
	MV1SetPosition(m_charModel, m_rigidbody.GetPos().ToDxVECTOR());

	// �i�s������0�łȂ���Ή�]
	if (m_rigidbody.GetVelo().x != 0.0f || m_rigidbody.GetVelo().z != 0.0f)
	{
		// atan2��Y����]�p���v�Z�iZ���O�AX���E�̍��W�n�̏ꍇ�j
		float angleY = std::atan2(m_rigidbody.GetVelo().x, -m_rigidbody.GetVelo().z);
		MV1SetRotationXYZ(m_charModel, VGet(0.0f, -angleY, 0.0f));
	}

	// ���X�e�B�b�N�̓��͂��Ȃ��ꍇ�ҋ@��ԂɈڍs
	if (m_rigidbody.GetVelo().x == 0.0f && m_rigidbody.GetVelo().z == 0.0f)
	{
		ChangeState(PlayerState::Idle);
	}

	// L�X�e�B�b�N�������݂̓��͂�����΃_�b�V����ԂɈڍs����
	if (Input::Instance().IsTrigger("LPush"))
	{
		ChangeState(PlayerState::Run);
	}

	// A�{�^���̓��͂�����΍U����ԂɈڍs����
	if (Input::Instance().IsTrigger("A"))
	{
		ChangeState(PlayerState::Chop);
	}

	// X�{�^���̓��͂�����΋��U����ԂɈڍs����
	if (Input::Instance().IsTrigger("X"))
	{
		ChangeState(PlayerState::Spin);
	}

	// B�{�^���̓��͂�����Ή����ԂɈڍs����
	if (Input::Instance().IsTrigger("B"))
	{
		ChangeState(PlayerState::Dodge);
	}
}

void Player::RunUpdate()
{
	SetActive(true);
	m_weapon->Update(m_charModel, m_attackFrame, kColTimingTable.at(PlayerState::Run));

	Vec3 dir = { 0.0f, 0.0f, 0.0f };
	// ���X�e�B�b�N�ňړ�
	// ������
	if (Input::Instance().IsPress("LEFT"))
	{
		dir.x = -kRunSpeed;
	}
	// �E����
	else if (Input::Instance().IsPress("RIGHT"))
	{
		dir.x = kRunSpeed;
	}
	// �������̓��͂Ȃ�
	else
	{
		dir.x = 0.0f;
	}
	// �����
	if (Input::Instance().IsPress("UP"))
	{
		dir.z = kRunSpeed;
	}
	// ������
	else if (Input::Instance().IsPress("DOWN"))
	{
		dir.z = -kRunSpeed;
	}
	// �c�����̓��͂Ȃ�
	else
	{
		dir.z = 0.0f;
	}

	// �x�N�g���𐳋K�����ړ����x�������|�W�V�����ɉ��Z
	dir.Normalize();
	m_rigidbody.SetVelo(dir * kRunSpeed);
	MV1SetPosition(m_charModel, m_rigidbody.GetPos().ToDxVECTOR());

	// �i�s������0�łȂ���Ή�]
	if (m_rigidbody.GetVelo().x != 0.0f || m_rigidbody.GetVelo().z != 0.0f)
	{
		// atan2��Y����]�p���v�Z�iZ���O�AX���E�̍��W�n�̏ꍇ�j
		float angleY = std::atan2(m_rigidbody.GetVelo().x, -m_rigidbody.GetVelo().z);
		MV1SetRotationXYZ(m_charModel, VGet(0.0f, -angleY, 0.0f));
	}

	// ���X�e�B�b�N�̓��͂��Ȃ��ꍇ�ҋ@��ԂɈڍs����
	if (m_rigidbody.GetVelo().x == 0.0f && m_rigidbody.GetVelo().z == 0.0f)
	{
		ChangeState(PlayerState::Idle);
	}

	// ���X�e�B�b�N�������݂̓��͂�����Ε�����ԂɈڍs����
	if (Input::Instance().IsTrigger("LPush"))
	{
		ChangeState(PlayerState::Walk);
	}

	// A�{�^���̓��͂�����΍U����ԂɈڍs����
	if (Input::Instance().IsTrigger("A"))
	{
		ChangeState(PlayerState::Chop);
	}

	// X�{�^���̓��͂�����΋��U����ԂɈڍs����
	if (Input::Instance().IsTrigger("X"))
	{
		ChangeState(PlayerState::Spin);
	}

	// B�{�^���̓��͂�����Ή����ԂɈڍs����
	if (Input::Instance().IsTrigger("B"))
	{
		ChangeState(PlayerState::Dodge);
	}
}

void Player::ChopUpdate()
{
	SetActive(true);
	++m_attackFrame;
	m_weapon->Update(m_charModel, m_attackFrame, kColTimingTable.at(PlayerState::Chop));

	// 1�{�^���̓��͂�����΍U����ԂɈڍs����
	if (Input::Instance().IsTrigger("A"))
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
		}
		// �����Ȃ���Αҋ@��ԂɈڍs����
		else
		{
			ChangeState(PlayerState::Idle);
		}
	}
}

void Player::SliceUpdate()
{
	SetActive(true);
	++m_attackFrame;
	m_weapon->Update(m_charModel, m_attackFrame, kColTimingTable.at(PlayerState::Slice));

	// 1�{�^���̓��͂�����΍U����ԂɈڍs����
	if (Input::Instance().IsTrigger("A"))
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
		}
		else
		{
			ChangeState(PlayerState::Idle);
		}
	}
}

void Player::StabUpdate()
{
	SetActive(true);
	++m_attackFrame;
	m_weapon->Update(m_charModel, m_attackFrame, kColTimingTable.at(PlayerState::Stab));

	// 1�{�^���̓��͂�����΍U����ԂɈڍs����
	if (Input::Instance().IsTrigger("A"))
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
		}
		else
		{
			ChangeState(PlayerState::Idle);
		}
	}
}

void Player::SpinUpdate()
{
	SetActive(true);
	++m_attackFrame;
	m_weapon->Update(m_charModel, m_attackFrame, kColTimingTable.at(PlayerState::Spin));

	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::UltimateUpdate()
{
	SetActive(true);
	m_weapon->AttackUpdate(m_charModel);

	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::DodgeUpdate()
{
	SetActive(false);
	m_weapon->Update(m_charModel, m_attackFrame, kColTimingTable.at(PlayerState::Dodge));

	// �A�j���[�V�������I��������ҋ@��Ԃɖ߂�
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::HitUpdate()
{
	SetActive(false);
	m_weapon->Update(m_charModel, m_attackFrame, kColTimingTable.at(PlayerState::Hit));

	MV1SetPosition(m_charModel, m_rigidbody.GetPos().ToDxVECTOR());
	// �A�j���[�V�������I��������ҋ@��Ԃɖ߂�
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::DeadUpdate()
{
	SetActive(false);
	m_weapon->Update(m_charModel, m_attackFrame, kColTimingTable.at(PlayerState::Dead));

	// �A�j���[�V�������I��������ҋ@��Ԃɖ߂�
	if (m_anim.GetNextAnim().isEnd)
	{
		if (m_charModel >= 0)
		{
			MV1DeleteModel(m_charModel);
			m_charModel = -1;
		}
		m_isDead = true;
	}
}

