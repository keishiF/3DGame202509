#include "Enemy.h"
#include "DxLib.h"

namespace
{
	constexpr int kHp = 10;
}

Enemy::Enemy() :
	m_pos(0.0f, 0.0f, 0.0f),
	m_hp(kHp),
	m_isDead(false),
	m_state(&Enemy::IdleInit)
{
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
}

void Enemy::Draw()
{
}

void Enemy::OnDamage()
{
}
