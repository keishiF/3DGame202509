#include "Enemy.h"
#include "Player.h"
#include "DxLib.h"

#include <cassert>

namespace
{
	// アニメーション名
	// 待機
	const char* kFindAnimName   = "2H_Melee_Idle";
	// 発見
	const char* kChaseAnimName  = "Running_A";
	// 攻撃
	const char* kAttackAnimName = "1H_Melee_Attack_Slice_Diagonal";
	// 被弾
	const char* kHitAnimName    = "";
	// 死亡
	const char* kDeadAnimName   = "";

	// エネミーがプレイヤーを発見できる範囲
	constexpr float kFindRadius   = 300.0f;
	constexpr float kAttackRadius = 100.0f;

	// 初期HP
	constexpr int kHp = 10;

	constexpr float kRadius = 25.0f;

	// モデルの拡大率
	constexpr float kModelScale = 45.0f;
}

Enemy::Enemy() :
	m_minionModel(-1),
	m_bladeModel(-1),
	m_pos(300.0f, 0.0f, 300.0f),
	m_radius(kRadius),
	m_findRadius(kFindRadius),
	m_attackRadius(kAttackRadius),
	m_hp(kHp),
	m_isDead(false),
	m_isFind(false),
	m_isAttack(false),
	m_state(EnemyState::Find)
{
	m_minionModel = MV1LoadModel("Data/Enemy/Enemy2.mv1");
	assert(m_minionModel >= 0);
	m_bladeModel = MV1LoadModel("Data/Enemy/Blade.mv1");
	assert(m_bladeModel >= 0);
	MV1SetScale(m_minionModel, VGet(kModelScale, kModelScale, kModelScale));

	/*MV1SetMatrix(m_minionModel, MGetIdent());
	VECTOR position = MV1GetFramePosition(m_minionModel, 14);
	MATRIX transMat = MGetTranslate(VScale(position, -1.0f));
	MATRIX frameMat = MV1GetFrameLocalWorldMatrix(m_bladeModel, 0);
	MATRIX mixMat   = MMult(transMat, frameMat);
	MV1SetMatrix(m_minionModel, mixMat);*/

	MV1SetPosition(m_minionModel, VGet(m_pos.x, m_pos.y, m_pos.z));

	m_anim.Init(m_minionModel);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kFindAnimName, true);
}

Enemy::~Enemy()
{
}

void Enemy::Update(std::shared_ptr<Player> player)
{
	// アニメーションの更新
	m_anim.UpdateAnim(m_anim.GetPrevAnim());
	m_anim.UpdateAnim(m_anim.GetNextAnim());
	m_anim.UpdateAnimBlend();

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

void Enemy::Draw()
{
#if _DEBUG
	DrawSphere3D(VGet(m_pos.x, m_pos.y, m_pos.z), 10.0f, 16, 0x0000ff, 0x0000ff, true);
	DrawSphere3D(VGet(m_pos.x, m_pos.y, m_pos.z), m_findRadius, 16, 0xff00ff, 0xff00ff, false);
	DrawSphere3D(VGet(m_pos.x, m_pos.y, m_pos.z), m_attackRadius, 16, 0xff00ff, 0xff00ff, false);
	DrawCapsule3D(VGet(m_pos.x, m_pos.y + 20.0f, m_pos.z), VGet(m_pos.x, m_pos.y + 70.0f, m_pos.z),
		m_radius, 16, 0xff00ff, 0xff00ff, false);
#endif

	MV1DrawModel(m_minionModel);
}

void Enemy::OnDamage()
{
	m_hp -= 1;
}

void Enemy::ChangeState(EnemyState newState)
{
	// 現在の状態と次の状態が同じ場合return
	if (m_state == newState) return;

	m_state = newState;

	switch (m_state)
	{
	case EnemyState::Find:
		m_anim.ChangeAnim(kFindAnimName, true);
		break;
	case EnemyState::Chase:
		m_anim.ChangeAnim(kChaseAnimName, true);
		break;
	case EnemyState::Attack:
		m_anim.ChangeAnim(kAttackAnimName, false);
		break;
	case EnemyState::Hit:
		m_anim.ChangeAnim(kHitAnimName, false);
		break;
	case EnemyState::Dead:
		m_anim.ChangeAnim(kDeadAnimName, false);
		break;
	}
}

void Enemy::FindUpdate(std::shared_ptr<Player> player)
{
	float distance = (m_pos - player->GetPos()).Length();
	if (distance <= (m_findRadius + player->GetRadius()))
	{
		ChangeState(EnemyState::Chase);
	}
}

void Enemy::ChaseUpdate(std::shared_ptr<Player> player)
{
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

void Enemy::AttackUpdate(std::shared_ptr<Player> player)
{
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

void Enemy::HitUpdate(std::shared_ptr<Player> player)
{
}

void Enemy::DeadUpdate(std::shared_ptr<Player> player)
{
}
