#include "EnemyMinion.h"
#include "EnemyMinionBlade.h"
#include "Player/Player.h"

#include "CapsuleColliderData.h"
#include "Animator.h"

#include "DxLib.h"
#include <algorithm>
#include <cassert>
#include <unordered_map>

namespace
{
	// エネミーがプレイヤーを発見できる範囲
	constexpr float kFindRadius   = 500.0f;
	constexpr float kAttackRadius = 100.0f;

	// 初期HP
	constexpr int kHp = 5;

	// エネミーの速度
	constexpr float kSpeed = 1.0f;

	// エネミーの当たり判定用半径
	constexpr float kColScale = 70.0f;
	constexpr float kColRadius = 25.0f;

	constexpr float kAttackFrame = 32.0f;

	// モデルの拡大率
	constexpr float kModelScale = 45.0f;

	// アニメーション名
	// 待機
	const char* kFindAnimName = "2H_Melee_Idle";
	// 発見
	const char* kChaseAnimName = "Running_C";
	// 攻撃
	const char* kAttackAnimName = "1H_Melee_Attack_Slice_Diagonal";
	// 被弾
	const char* kHitAnimName = "Hit_B";
	// 死亡
	const char* kDeadAnimName = "Death_B";

	// アニメーションの再生速度
	constexpr float kAnimSpeed = 1.0f;

	const std::unordered_map<EnemyState, AttackTiming> kRightColTimingTable =
	{
		{EnemyState::Find,	 { 0,  0}},
		{EnemyState::Chase,	 { 0,  0}},
		{EnemyState::Attack, {24, 48}},
		{EnemyState::Hit,	 { 0,  0}},
		{EnemyState::Dead,	 { 0,  0}}
	};
}

EnemyMinion::EnemyMinion()
{	
}

EnemyMinion::~EnemyMinion()
{
}

void EnemyMinion::Init(std::shared_ptr<Physics> physics, Vec3& pos, const Vec3& rot, const Vec3& scale)
{
	Collidable::Init(physics);

	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);

	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->m_startPos = pos;
	colData->m_radius = kColRadius;

	// スピードの初期化
	m_findRadius = kFindRadius;
	m_attackRadius = kAttackRadius;
	m_hp = kHp;
	m_isDead = false;
	m_attackFrame = 0.0f;

	m_charModel = MV1LoadModel("Data/Model/Enemy/Minion/Minion.mv1");
	assert(m_charModel >= 0);

	MV1SetScale(m_charModel, VGet(scale.x * 50.0f, scale.y * 50.0f, scale.z * 50.0f));
	MV1SetPosition(m_charModel, pos.ToDxVECTOR());

	m_anim.Init(m_charModel);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kFindAnimName, kAnimSpeed, true);

	m_weapon = std::make_shared<EnemyMinionBlade>();
	m_weapon->Init(physics);
}

void EnemyMinion::Update(std::shared_ptr<Player> player)
{
	if (m_isDead && m_charModel < 0)
	{
		return;
	}

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

	//当たり判定
	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	Vec3 colPos = m_rigidbody.GetPos();
	colPos.y += kColScale;
	colData->m_startPos = colPos;

	MV1SetPosition(m_charModel, m_rigidbody.GetPos().ToDxVECTOR());
}

void EnemyMinion::Draw()
{
	if (m_isDead && m_charModel < 0)
	{
		return;
	}

#if _DEBUG
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), 10.0f, 16, 0x0000ff, 0x0000ff, true);
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), m_findRadius, 16, 0xff00ff, 0xff00ff, false);
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), m_attackRadius, 16, 0xff00ff, 0xff00ff, false);

#endif
	MV1DrawModel(m_charModel);
	m_weapon->Draw();
	
	Vec3 worldPos = m_rigidbody.GetPos();
	worldPos.y += 120.0f; // 頭上の高さ調整

	VECTOR worldVec = worldPos.ToDxVECTOR();

	// 3D→2D座標変換（戻り値がスクリーン座標）
	VECTOR screenVec = ConvWorldPosToScreenPos(worldVec);

	const int gaugeWidth = 100;
	const int gaugeHeight = 10;

	int gaugeX = static_cast<int>(screenVec.x - gaugeWidth / 2);
	int gaugeY = static_cast<int>(screenVec.y - gaugeHeight / 2);

	float hpRate = static_cast<float>(m_hp) / kHp;
	hpRate = std::clamp(hpRate, 0.0f, 1.0f);

	DrawBox(gaugeX, gaugeY,
		gaugeX + gaugeWidth,
		gaugeY + gaugeHeight,
		0x808080, true);

	int hpBarWidth = static_cast<int>(gaugeWidth * hpRate);
	DrawBox(gaugeX, gaugeY,
		gaugeX + hpBarWidth,
		gaugeY + gaugeHeight,
		0xff0000, true);

	DrawBox(gaugeX, gaugeY,
		gaugeX + gaugeWidth,
		gaugeY + gaugeHeight,
		0x000000, false);
}

void EnemyMinion::OnDamage()
{
	m_hp -= 1;

	if (m_hp <= 0 && !m_isDead)
	{
		ChangeState(EnemyState::Dead, kAnimSpeed);
	}
	else
	{
		ChangeState(EnemyState::Hit, kAnimSpeed);
	}
}

const char* EnemyMinion::GetAnimName(EnemyState state) const
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
	default:
		return "";
		assert(0 && "存在しないアニメーション");
	}
}

bool EnemyMinion::IsLoopAnim(EnemyState state) const
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
	default:
		return "";
		assert(0 && "存在しないステート");
	}
}

void EnemyMinion::FindUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);
	m_weapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(EnemyState::Find));

	float distance = (m_rigidbody.GetPos() - player->GetPos()).Length();
	if (distance <= (m_findRadius + player->GetRadius()))
	{
		ChangeState(EnemyState::Chase, kAnimSpeed);
	}
}

void EnemyMinion::ChaseUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);
	m_weapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(EnemyState::Chase));

	// プレイヤーへの方向ベクトル
	Vec3 myPos = m_rigidbody.GetPos();
	Vec3 toPlayerDir = player->GetPos() - myPos;
	toPlayerDir.y = 0.0f;

	// 距離が十分にある場合のみ移動
	if (toPlayerDir.Length() > 1.0f)
	{
		toPlayerDir.Normalize();
		m_rigidbody.SetVelo(toPlayerDir * kSpeed);
		MV1SetPosition(m_charModel, myPos.ToDxVECTOR());

		// 進行方向が0でなければ回転
		if (m_rigidbody.GetVelo().x != 0.0f || m_rigidbody.GetVelo().z != 0.0f)
		{
			// atan2でY軸回転角を計算（Zが前、Xが右の座標系の場合）
			float angleY = std::atan2(m_rigidbody.GetVelo().x, -m_rigidbody.GetVelo().z);
			MV1SetRotationXYZ(m_charModel, VGet(0.0f, -angleY, 0.0f));
		}
	}

	float distance = (myPos - player->GetPos()).Length();
	if (distance >= (m_findRadius + player->GetRadius()))
	{
		ChangeState(EnemyState::Find, kAnimSpeed);
	}

	if (distance <= (m_attackRadius + player->GetRadius()))
	{
		ChangeState(EnemyState::Attack, kAnimSpeed);
	}
}

void EnemyMinion::AttackUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);
	++m_attackFrame;
	m_weapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(EnemyState::Attack));

	// プレイヤーへの方向ベクトル
	Vec3 myPos = m_rigidbody.GetPos();
	Vec3 dir = player->GetPos() - myPos;
	dir.y = 0.0f;

	if (m_attackFrame <= kAttackFrame)
	{
		++m_attackFrame;
		if (dir.x != 0.0f || dir.z != 0.0f)
		{
			// atan2でY軸回転角を計算（Zが前、Xが右の座標系の場合）
			float angleY = std::atan2(dir.x, -dir.z);
			MV1SetRotationXYZ(m_charModel, VGet(0.0f, -angleY, 0.0f));
		}
	}

	if (m_anim.GetNextAnim().isEnd)
	{
		float distance = (myPos - player->GetPos()).Length();
		if (distance >= (m_findRadius + player->GetRadius()))
		{
			ChangeState(EnemyState::Find, kAnimSpeed);
		}
		else
		{
			ChangeState(EnemyState::Chase, kAnimSpeed);
		}
	}
}

void EnemyMinion::HitUpdate(std::shared_ptr<Player> player)
{
	SetActive(false);
	m_weapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(EnemyState::Hit));

	MV1SetPosition(m_charModel, m_rigidbody.GetPos().ToDxVECTOR());
	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(EnemyState::Find, kAnimSpeed);
	}
}

void EnemyMinion::DeadUpdate(std::shared_ptr<Player> player)
{
	SetActive(false);
	m_weapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(EnemyState::Dead));

	// アニメーションが終了したら待機状態に戻る
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
