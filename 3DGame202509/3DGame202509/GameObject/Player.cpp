#include "Player.h"
#include <cassert>

namespace
{
	// HP�̏����l
	constexpr int kHp = 100;
	// �ړ����x
	constexpr float kSpeed = 3.5f;
	// �W�����v��
	constexpr float kJumpSpeed = 20.0f;
	// �d��
	constexpr float kGravity = 0.75f;
	// �v���C���[�̃��f���̊g��l
	constexpr float kPlayerModelScale = 30.0f;
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
	m_model = MV1LoadModel("Data/Model/Player/Player1.mv1");
	assert(m_model >= 0);
	MV1SetScale(m_model, VGet(kPlayerModelScale, kPlayerModelScale, kPlayerModelScale));
}

Player::~Player()
{
	MV1DeleteModel(m_model);
}

void Player::Update(Input& input)
{
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

