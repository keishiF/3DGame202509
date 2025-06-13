#include "Minion.h"
#include "Player/Player.h"

#include "Animation.h"

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
	const char* kHitAnimName    = "Hit_B";
	// 死亡
	const char* kDeadAnimName   = "Death_B";

	// エネミーがプレイヤーを発見できる範囲
	constexpr float kFindRadius   = 300.0f;
	constexpr float kAttackRadius = 100.0f;

	// 初期HP
	constexpr int kHp = 10;

	constexpr float kRadius = 25.0f;

	// モデルの拡大率
	constexpr float kModelScale = 45.0f;
	constexpr float kBladeModelScale = 0.01f;
}

Minion::Minion() :
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
	m_state(MinionState::Find)
{
	m_minionModel = MV1LoadModel("Data/Enemy/Minion/Minion.mv1");
	assert(m_minionModel >= 0);
	m_bladeModel = MV1LoadModel("Data/Enemy/Minion/Blade.mv1");
	assert(m_bladeModel >= 0);
	MV1SetScale(m_minionModel, VGet(kModelScale, kModelScale, kModelScale));

	MV1SetPosition(m_minionModel, VGet(m_pos.x, m_pos.y, m_pos.z));

	m_anim.Init(m_minionModel);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kFindAnimName, true);
}

Minion::~Minion()
{
}

void Minion::Update(std::shared_ptr<Player> player)
{
	// アニメーションの更新
	m_anim.UpdateAnim(m_anim.GetPrevAnim());
	m_anim.UpdateAnim(m_anim.GetNextAnim());
	m_anim.UpdateAnimBlend();

	// アタッチするモデルのMV1SetMatrixの設定を無効化する
	MV1SetMatrix(m_bladeModel, MGetIdent());
	// アタッチするモデルのフレームの座標を取得する
	VECTOR position = MV1GetFramePosition(m_bladeModel, 0);
	// アタッチするモデルを,フレームの座標を原点にするための平行移動行列を作成
	MATRIX transMat = MGetTranslate(VScale(position, -1.0f));
	// アタッチされるモデルのフレームの行列を取得
	MATRIX frameMat = MV1GetFrameLocalWorldMatrix(m_minionModel, 14);
	// アタッチするモデルの拡大行列を取得
	MATRIX scaleMat = MGetScale(VGet(kBladeModelScale, kBladeModelScale, kBladeModelScale));
	// アタッチするモデルの回転行列を取得
	MATRIX yMat     = MGetRotY(DX_PI_F);
	// 各行列を合成
	MATRIX mixMat = MGetIdent();
	mixMat = MMult(transMat, mixMat);
	mixMat = MMult(frameMat, mixMat);
	mixMat = MMult(scaleMat, mixMat);
	mixMat = MMult(yMat, mixMat);
	// 合成した行列をモデルにセット
	MV1SetMatrix(m_bladeModel, mixMat);

	switch (m_state)
	{
	case MinionState::Find:
		FindUpdate(player);
		break;
	case MinionState::Chase:
		ChaseUpdate(player);
		break;
	case MinionState::Attack:
		AttackUpdate(player);
		break;
	case MinionState::Hit:
		HitUpdate(player);
		break;
	case MinionState::Dead:
		DeadUpdate(player);
		break;
	}
}

void Minion::Draw()
{
#if _DEBUG
	DrawSphere3D(VGet(m_pos.x, m_pos.y, m_pos.z), 10.0f, 16, 0x0000ff, 0x0000ff, true);
	DrawSphere3D(VGet(m_pos.x, m_pos.y, m_pos.z), m_findRadius, 16, 0xff00ff, 0xff00ff, false);
	DrawSphere3D(VGet(m_pos.x, m_pos.y, m_pos.z), m_attackRadius, 16, 0xff00ff, 0xff00ff, false);
	DrawCapsule3D(VGet(m_pos.x, m_pos.y + 20.0f, m_pos.z), VGet(m_pos.x, m_pos.y + 70.0f, m_pos.z),
		m_radius, 16, 0xff00ff, 0xff00ff, false);
#endif

	MV1DrawModel(m_minionModel);
	MV1DrawModel(m_bladeModel);
}

void Minion::OnDamage()
{
	m_hp -= 1;
}

void Minion::ChangeState(MinionState newState)
{
	// 現在の状態と次の状態が同じ場合return
	if (m_state == newState) return;

	m_state = newState;

	switch (m_state)
	{
	case MinionState::Find:
		m_anim.ChangeAnim(kFindAnimName, true);
		break;
	case MinionState::Chase:
		m_anim.ChangeAnim(kChaseAnimName, true);
		break;
	case MinionState::Attack:
		m_anim.ChangeAnim(kAttackAnimName, false);
		break;
	case MinionState::Hit:
		m_anim.ChangeAnim(kHitAnimName, false);
		break;
	case MinionState::Dead:
		m_anim.ChangeAnim(kDeadAnimName, false);
		break;
	}
}

void Minion::FindUpdate(std::shared_ptr<Player> player)
{
	float distance = (m_pos - player->GetPos()).Length();
	if (distance <= (m_findRadius + player->GetRadius()))
	{
		ChangeState(MinionState::Chase);
	}
}

void Minion::ChaseUpdate(std::shared_ptr<Player> player)
{
	float distance = (m_pos - player->GetPos()).Length();
	if (distance >= (m_findRadius + player->GetRadius()))
	{
		ChangeState(MinionState::Find);
	}

	if (distance <= (m_attackRadius + player->GetRadius()))
	{
		ChangeState(MinionState::Attack);
	}
}

void Minion::AttackUpdate(std::shared_ptr<Player> player)
{
	if (m_anim.GetNextAnim().isEnd)
	{
		float distance = (m_pos - player->GetPos()).Length();
		if (distance >= (m_findRadius + player->GetRadius()))
		{
			ChangeState(MinionState::Find);
		}
		else
		{
			ChangeState(MinionState::Chase);
		}
	}
}

void Minion::HitUpdate(std::shared_ptr<Player> player)
{
}

void Minion::DeadUpdate(std::shared_ptr<Player> player)
{
}
