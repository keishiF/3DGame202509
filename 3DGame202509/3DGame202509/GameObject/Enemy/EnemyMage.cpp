#include "EnemyMage.h"
#include "Player/Player.h"

#include "Animation.h"

#include "DxLib.h"
#include <cassert>

namespace
{
	// アニメーション名
	// 待機
	const char* kFindAnimName   = "Idle_B";
	// 発見
	const char* kChaseAnimName  = "Running_B";
	// 攻撃
	const char* kAttackAnimName = "1H_Melee_Attack_Stab";
	// 被弾
	const char* kHitAnimName    = "Hit_B";
	// 死亡
	const char* kDeadAnimName   = "Death_B";

	// エネミーがプレイヤーを発見できる範囲
	constexpr float kFindRadius   = 300.0f;
	constexpr float kAttackRadius = 100.0f;

	// 初期HP
	constexpr int kHp = 10;

	constexpr float kColRadius = 25.0f;

	// モデルの拡大率
	constexpr float kModelScale      = 45.0f;
	constexpr float kBladeModelScale = 0.01f;
}

EnemyMage::EnemyMage()
{
	colRadius = kColRadius;

	m_pos = { -300.0f, 0.0f, -300.0f };
	m_findRadius = kFindRadius;
	m_attackRadius = kAttackRadius;
	m_hp = kHp;
	m_isDead = false;

	m_charModel = MV1LoadModel("Data/Enemy/Mage/Mage.mv1");
	assert(m_charModel >= 0);
	m_weaponModel = MV1LoadModel("Data/Enemy/Mage/Staff.mv1");
	assert(m_weaponModel >= 0);
	MV1SetScale(m_charModel, VGet(kModelScale, kModelScale, kModelScale));

	MV1SetPosition(m_charModel, VGet(m_pos.x, m_pos.y, m_pos.z));

	m_anim.Init(m_charModel);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kFindAnimName, true);
}

EnemyMage::~EnemyMage()
{
}

void EnemyMage::Update(std::shared_ptr<Player> player)
{
	// アニメーションの更新
	m_anim.UpdateAnim(m_anim.GetPrevAnim());
	m_anim.UpdateAnim(m_anim.GetNextAnim());
	m_anim.UpdateAnimBlend();

	// アタッチするモデルのMV1SetMatrixの設定を無効化する
	MV1SetMatrix(m_weaponModel, MGetIdent());
	// アタッチするモデルのフレームの座標を取得する
	VECTOR position = MV1GetFramePosition(m_weaponModel, 0);
	// アタッチするモデルを,フレームの座標を原点にするための平行移動行列を作成
	MATRIX transMat = MGetTranslate(VScale(position, -1.0f));
	// アタッチされるモデルのフレームの行列を取得
	MATRIX frameMat = MV1GetFrameLocalWorldMatrix(m_charModel, 14);
	// アタッチするモデルの拡大行列を取得
	MATRIX scaleMat = MGetScale(VGet(kBladeModelScale, kBladeModelScale, kBladeModelScale));
	// アタッチするモデルの回転行列を取得
	MATRIX yMat = MGetRotY(DX_PI_F);
	// 各行列を合成
	MATRIX mixMat = MGetIdent();
	mixMat = MMult(transMat, mixMat);
	mixMat = MMult(frameMat, mixMat);
	mixMat = MMult(scaleMat, mixMat);
	mixMat = MMult(yMat, mixMat);
	// 合成した行列をモデルにセット
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

void EnemyMage::Draw()
{
#if _DEBUG
	DrawSphere3D(VGet(m_pos.x, m_pos.y, m_pos.z), 10.0f, 16, 0x0000ff, 0x0000ff, true);
	DrawSphere3D(VGet(m_pos.x, m_pos.y, m_pos.z), m_findRadius, 16, 0xff00ff, 0xff00ff, false);
	DrawSphere3D(VGet(m_pos.x, m_pos.y, m_pos.z), m_attackRadius, 16, 0xff00ff, 0xff00ff, false);
	DrawCapsule3D(VGet(m_pos.x, m_pos.y + 20.0f, m_pos.z), VGet(m_pos.x, m_pos.y + 70.0f, m_pos.z),
		colRadius, 16, 0xff00ff, 0xff00ff, false);
#endif

	MV1DrawModel(m_charModel);
	MV1DrawModel(m_weaponModel);
}

const char* EnemyMage::GetAnimName(EnemyState state) const
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

bool EnemyMage::IsLoopAnim(EnemyState state) const
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

void EnemyMage::FindUpdate(std::shared_ptr<Player> player)
{
	float distance = (m_pos - player->GetPos()).Length();
	if (distance <= (m_findRadius + player->GetRadius()))
	{
		ChangeState(EnemyState::Chase);
	}
}

void EnemyMage::ChaseUpdate(std::shared_ptr<Player> player)
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

void EnemyMage::AttackUpdate(std::shared_ptr<Player> player)
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

void EnemyMage::HitUpdate(std::shared_ptr<Player> player)
{
}

void EnemyMage::DeadUpdate(std::shared_ptr<Player> player)
{
}
