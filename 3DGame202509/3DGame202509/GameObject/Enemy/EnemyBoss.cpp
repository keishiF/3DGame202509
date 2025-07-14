#include "EnemyBoss.h"
#include "EnemyBossRightAxe.h"
#include "EnemyBossLeftAxe.h"
#include "Player/Player.h"

#include "CapsuleColliderData.h"
#include "Animator.h"

#include <DxLib.h>
#include <algorithm>
#include <cassert>
#include <unordered_map>

namespace
{
	// エネミーがプレイヤーを発見できる範囲
	constexpr float kFindRadius = 1500.0f;
	constexpr float kAttackRadius = 600.0f;

	// 初期HP
	constexpr int kHp = 5;

	// エネミーの速度
	constexpr float kWalkSpeed = 2.0f;
	constexpr float kChaseSpeed = 10.0f;

	// エネミーの当たり判定用半径
	constexpr float kColScale = 200.0f;
	constexpr float kColRadius = 75.0f;

	constexpr float kAttackFrame = 32.0f;

	// モデルの拡大率
	constexpr float kModelScale = 200.0f;

	// アニメーション名

	// 歩き
	const char* kWalkAnimName = "Walking_D_Skeletons";
	// 待機
	const char* kFindAnimName = "Idle";
	// 発見
	const char* kChaseAnimName = "Running_A";
	// 攻撃
	const char* kAttackAnimName      = "1H_Melee_Attack_Slice_Diagonal";
	const char* kChopAttackAnimName  = "Dualwield_Melee_Attack_Chop";
	const char* kSliceAttackAnimName = "Dualwield_Melee_Attack_Slice";
	const char* kStabAttackAnimName  = "Dualwield_Melee_Attack_Stab";
	const char* kSpinAttackAnimName  = "2H_Melee_Attack_Spinning";
	// 被弾
	const char* kHitAnimName  = "Hit_B";
	// 死亡
	const char* kDeadAnimName = "Death_B";

	// アニメーションの再生速度
	constexpr float kAnimSpeed      = 0.5f;
	constexpr float kChopAnimSpeed  = 0.4f;
	constexpr float kSliceAnimSpeed = 0.4f;
	constexpr float kStabAnimSpeed  = 0.4f;
	constexpr float kSpinAnimSpeed  = 0.4f;

	const std::unordered_map<EnemyState, RightAttackTiming> kRightColTimingTable =
	{
		{EnemyState::Walk,	 { 0,  0}},
		{EnemyState::Find,	 { 0,  0}},
		{EnemyState::Chase,	 { 0,  0}},
		{EnemyState::Attack, { 0, 48}},
		{EnemyState::Chop,   {12, 48}},
		{EnemyState::Slice,  {12, 56}},
		{EnemyState::Stab,   {12, 48}},
		{EnemyState::Spin,   { 0, 180}},
		{EnemyState::Hit,	 { 0,  0}},
		{EnemyState::Dead,	 { 0,  0}}
	};
	const std::unordered_map<EnemyState, LeftAttackTiming> kLeftColTimingTable =
	{
		{EnemyState::Walk,	 { 0,  0}},
		{EnemyState::Find,	 { 0,  0}},
		{EnemyState::Chase,	 { 0,  0}},
		{EnemyState::Attack, { 0, 48}},
		{EnemyState::Chop,   {12, 48}},
		{EnemyState::Slice,  {12, 56}},
		{EnemyState::Stab,   {12, 48}},
		{EnemyState::Spin,   { 0, 180}},
		{EnemyState::Hit,	 { 0,  0}},
		{EnemyState::Dead,	 { 0,  0}}
	};
}

EnemyBoss::EnemyBoss()
{
}

EnemyBoss::~EnemyBoss()
{
}

void EnemyBoss::Init(std::shared_ptr<Physics> physics, Vec3& pos, const Vec3& rot, const Vec3& scale)
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
	m_walkFrame = 0.0f;

	m_charModel = MV1LoadModel("Data/Model/Enemy/Boss/Boss.mv1");
	assert(m_charModel >= 0);

	MV1SetScale(m_charModel, VGet(scale.x * kModelScale, scale.y * kModelScale, scale.z * kModelScale));
	MV1SetPosition(m_charModel, pos.ToDxVECTOR());
	MV1SetRotationXYZ(m_charModel, VGet(rot.x, rot.y, rot.z));

	m_anim.Init(m_charModel);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kFindAnimName, kAnimSpeed, true);

	m_rightWeapon = std::make_shared<EnemyBossRightAxe>();
	m_rightWeapon->Init(physics);

	m_leftWeapon = std::make_shared<EnemyBossLeftAxe>();
	m_leftWeapon->Init(physics);
}

void EnemyBoss::Update(std::shared_ptr<Player> player)
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
	case EnemyState::Walk:
		WalkUpdate(player);
		break;
	case EnemyState::Find:
		FindUpdate(player);
		break;
	case EnemyState::Chase:
		ChaseUpdate(player);
		break;
	case EnemyState::Attack:
		AttackUpdate(player);
		break;
	case EnemyState::Chop:
		ChopUpdate(player);
		break;
	case EnemyState::Slice:
		SliceUpdate(player);
		break;
	case EnemyState::Stab:
		StabUpdate(player);
		break;
	case EnemyState::Spin:
		SpinUpdate(player);
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

void EnemyBoss::Draw()
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
	m_rightWeapon->Draw();
	m_leftWeapon->Draw();

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

void EnemyBoss::OnDamage()
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

void EnemyBoss::WalkUpdate(std::shared_ptr<Player> player)
{
	SetActive(false);
	++m_walkFrame;

	// プレイヤーへの方向ベクトル
	Vec3 myPos = m_rigidbody.GetPos();
	Vec3 toPlayerDir = player->GetPos() - myPos;
	toPlayerDir.y = 0.0f;
	toPlayerDir.Normalize();
	m_rigidbody.SetVelo(toPlayerDir * kWalkSpeed);
	MV1SetPosition(m_charModel, myPos.ToDxVECTOR());

	// 進行方向が0でなければ回転
	if (m_rigidbody.GetVelo().x != 0.0f || m_rigidbody.GetVelo().z != 0.0f)
	{
		// atan2でY軸回転角を計算（Zが前、Xが右の座標系の場合）
		float angleY = std::atan2(m_rigidbody.GetVelo().x, -m_rigidbody.GetVelo().z);
		MV1SetRotationXYZ(m_charModel, VGet(0.0f, -angleY, 0.0f));
	}

	if (m_walkFrame >= 120.0f)
	{
		float distance = (myPos - player->GetPos()).Length();
		if (distance >= (m_findRadius + player->GetRadius()))
		{
			ChangeState(EnemyState::Find, kAnimSpeed);
		}

		if (distance <= (m_attackRadius + player->GetRadius()))
		{
			if (distance > 500.0f && m_prevState != EnemyState::Stab)
			{
				ChangeState(EnemyState::Stab, kStabAnimSpeed);
			}
			else if (distance > 400.0f && m_prevState != EnemyState::Chop)
			{
				ChangeState(EnemyState::Chop, kChopAnimSpeed);
			}
			else if (distance > 300.0f && m_prevState != EnemyState::Slice)
			{
				ChangeState(EnemyState::Slice, kSliceAnimSpeed);
			}
			else if (distance < 300.0f && m_prevState == EnemyState::Spin)
			{
				ChangeState(EnemyState::Slice, kSliceAnimSpeed);
			}
			else if (m_prevState != EnemyState::Spin)
			{
				ChangeState(EnemyState::Spin, kSpinAnimSpeed);
			}
		}
	}
}

void EnemyBoss::FindUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(EnemyState::Find));
	m_leftWeapon->Update(m_charModel, m_attackFrame, kLeftColTimingTable.at(EnemyState::Find));

	float distance = (m_rigidbody.GetPos() - player->GetPos()).Length();
	if (distance <= (m_findRadius + player->GetRadius()))
	{
		ChangeState(EnemyState::Chase, kAnimSpeed);
	}
}

void EnemyBoss::ChaseUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(EnemyState::Chase));
	m_leftWeapon->Update(m_charModel, m_attackFrame, kLeftColTimingTable.at(EnemyState::Chase));

	// プレイヤーへの方向ベクトル
	Vec3 myPos = m_rigidbody.GetPos();
	Vec3 toPlayerDir = player->GetPos() - myPos;
	toPlayerDir.y = 0.0f;

	// 距離が十分にある場合のみ移動
	if (toPlayerDir.Length() > 1.0f)
	{
		toPlayerDir.Normalize();
		m_rigidbody.SetVelo(toPlayerDir * kChaseSpeed);
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
		if (distance > 500.0f && m_prevState != EnemyState::Stab)
		{
			ChangeState(EnemyState::Stab, kStabAnimSpeed);
		}
		else if (distance > 400.0f && m_prevState != EnemyState::Chop)
		{
			ChangeState(EnemyState::Chop, kChopAnimSpeed);
		}
		else if (distance > 300.0f && m_prevState != EnemyState::Slice)
		{
			ChangeState(EnemyState::Slice, kSliceAnimSpeed);
		}
		else if (distance < 300.0f && m_prevState == EnemyState::Spin)
		{
			ChangeState(EnemyState::Slice, kSliceAnimSpeed);
		}
		else if (m_prevState != EnemyState::Spin)
		{
			ChangeState(EnemyState::Spin, kSpinAnimSpeed);
		}
	}
}

void EnemyBoss::AttackUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);
	++m_attackFrame;
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(EnemyState::Attack));
	m_leftWeapon->Update(m_charModel, m_attackFrame, kLeftColTimingTable.at(EnemyState::Attack));

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

void EnemyBoss::ChopUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);
	++m_attackFrame;
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(EnemyState::Chop));
	m_leftWeapon->Update(m_charModel, m_attackFrame, kLeftColTimingTable.at(EnemyState::Chop));

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
			ChangeState(EnemyState::Walk, kAnimSpeed);
		}
	}
}

void EnemyBoss::SliceUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);
	++m_attackFrame;
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(EnemyState::Slice));
	m_leftWeapon->Update(m_charModel, m_attackFrame, kLeftColTimingTable.at(EnemyState::Slice));

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
			ChangeState(EnemyState::Walk, kAnimSpeed);
		}
	}
}

void EnemyBoss::StabUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);
	++m_attackFrame;
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(EnemyState::Stab));
	m_leftWeapon->Update(m_charModel, m_attackFrame, kLeftColTimingTable.at(EnemyState::Stab));

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
			ChangeState(EnemyState::Walk, kAnimSpeed);
		}
	}
}

void EnemyBoss::SpinUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);
	++m_attackFrame;
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(EnemyState::Spin));
	m_leftWeapon->Update(m_charModel, m_attackFrame, kLeftColTimingTable.at(EnemyState::Spin));

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

	if (m_attackFrame >= 180.0f)
	{
		float distance = (myPos - player->GetPos()).Length();
		if (distance >= (m_findRadius + player->GetRadius()))
		{
			ChangeState(EnemyState::Find, kAnimSpeed);
		}
		else
		{
			ChangeState(EnemyState::Walk, kAnimSpeed);
		}
	}
}

void EnemyBoss::HitUpdate(std::shared_ptr<Player> player)
{
	SetActive(false);
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(EnemyState::Hit));
	m_leftWeapon->Update(m_charModel, m_attackFrame, kLeftColTimingTable.at(EnemyState::Hit));

	MV1SetPosition(m_charModel, m_rigidbody.GetPos().ToDxVECTOR());
	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(EnemyState::Find, kAnimSpeed);
	}
}

void EnemyBoss::DeadUpdate(std::shared_ptr<Player> player)
{
	SetActive(false);
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(EnemyState::Dead));
	m_leftWeapon->Update(m_charModel, m_attackFrame, kLeftColTimingTable.at(EnemyState::Dead));

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

const char* EnemyBoss::GetAnimName(EnemyState state) const
{
	switch (state)
	{
	case EnemyState::Walk:
		return kWalkAnimName;
	case EnemyState::Find:
		return kFindAnimName;
	case EnemyState::Chase:
		return kChaseAnimName;
	case EnemyState::Attack:
		return kAttackAnimName;
	case EnemyState::Chop:
		return kChopAttackAnimName;
	case EnemyState::Slice:
		return kSliceAttackAnimName;
	case EnemyState::Stab:
		return kStabAttackAnimName;
	case EnemyState::Spin:
		return kSpinAttackAnimName;
	case EnemyState::Hit:
		return kHitAnimName;
	case EnemyState::Dead:
		return kDeadAnimName;
	default:
		return "";
		assert(0 && "存在しないアニメーション");
	}
}

bool EnemyBoss::IsLoopAnim(EnemyState state) const
{
	switch (state)
	{
	case EnemyState::Walk:
		return true;
	case EnemyState::Find:
		return true;
	case EnemyState::Chase:
		return true;
	case EnemyState::Attack:
		return false;
	case EnemyState::Chop:
		return false;
	case EnemyState::Slice:
		return false;
	case EnemyState::Stab:
		return false;
	case EnemyState::Spin:
		return true;
	case EnemyState::Hit:
		return false;
	case EnemyState::Dead:
		return false;
	default:
		return "";
		assert(0 && "存在しないステート");
	}
}
