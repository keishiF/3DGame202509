#include "Player.h"
#include <cassert>

namespace
{
	// HPの初期値
	constexpr int kHp = 100;
	// 移動速度
	constexpr float kSpeed = 7.5f;
	// ジャンプ力
	constexpr float kJumpSpeed = 20.0f;
	// 重力
	constexpr float kGravity = 0.75f;
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
}

Player::~Player()
{
	MV1DeleteModel(m_model);
}

void Player::Update(Input& input)
{
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

