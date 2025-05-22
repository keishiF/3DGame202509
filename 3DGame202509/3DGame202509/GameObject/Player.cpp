#include "Player.h"
#include <cassert>

namespace
{
	// HP�̏����l
	constexpr int kHp = 100;
	// �ړ����x
	constexpr float kSpeed = 7.5f;
	// �W�����v��
	constexpr float kJumpSpeed = 20.0f;
	// �d��
	constexpr float kGravity = 0.75f;
	// �v���C���[�̃��f���̊g��l
	constexpr float kModelScale = 20.0f;
}

Player::Player() :
	m_model(-1),
	m_pos(0.0f, 0.0f, 0.0f),
	m_vec(0.0f, 0.0f, 0.0f),
	m_hp(kHp),
	m_jumpSpeed(kJumpSpeed),
	m_gravity(kGravity),
	m_frameCount(0.0f)
{
	m_model = MV1LoadModel("Data/Model/Player1.mv1");
	assert(m_model >= 0);
	MV1SetScale(m_model, VGet(kModelScale, kModelScale, kModelScale));
}

Player::~Player()
{
	MV1DeleteModel(m_model);
}

void Player::Update(Input& input)
{
	if (input.IsPress(PAD_INPUT_LEFT))
	{
		m_vec.x = kSpeed;
	}
	else if (input.IsPress(PAD_INPUT_RIGHT))
	{
		m_vec.x = -kSpeed;
	}
	else
	{
		m_vec.x = 0.0f;
	}

	if (input.IsPress(PAD_INPUT_UP))
	{
		m_vec.z = -kSpeed;
	}
	else if (input.IsPress(PAD_INPUT_DOWN))
	{
		m_vec.z = kSpeed;
	}
	else
	{
		m_vec.z = 0.0f;
	}

	//m_vec.Normalize();
	m_pos += m_vec;

	MV1SetPosition(m_model, VGet(m_pos.x, m_pos.y, m_pos.z));
}

void Player::Draw()
{
	MV1DrawModel(m_model);
}

void Player::OnDamage()
{
	m_hp--;
}

