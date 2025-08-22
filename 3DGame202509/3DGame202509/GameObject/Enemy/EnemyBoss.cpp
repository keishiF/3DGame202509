#include "CapsuleColliderData.h"
#include "EnemyBoss.h"
#include "EnemyBossLeftWeapon.h"
#include "EnemyBossRightWeapon.h"
#include "Player/Player.h"

namespace
{
	// HPの初期値、最大値
	constexpr float kHP = 15.0f;
	// 攻撃力
	constexpr float kDefaultAtk = 1.0f;

	// 移動速度
	constexpr float kWalkSpeed = 2.0f;
	constexpr float kChaseSpeed = 10.0f;

	// モデルの拡大値
	constexpr float kModelScale = 200.0f;

	// 攻撃フレーム
	constexpr float kAtkFrame = 32.0f;

	// 当たり判定
	// カプセルの半径
	constexpr float kCapsuleColRadius = 75.0f;
	// カプセルの長さ
	constexpr float kColScale = 200.0f;
	// 状態遷移に使う半径
	// プレイヤーを探知できる範囲
	constexpr float kPlayerFindRadius = 1500.0f;
	// 攻撃状態に移行する範囲
	constexpr float kAtkRadius = 600.0f;

	// アニメーション名
	// 待機
	const char* kFindAnimName = "Idle";
	// 歩き
	const char* kWalkAnimName = "Walking_D_Skeletons";
	// 発見
	const char* kChaseAnimName = "Running_A";
	// 攻撃
	const char* kAttackAnimName = "1H_Melee_Attack_Slice_Diagonal";
	const char* kChopAttackAnimName = "Dualwield_Melee_Attack_Chop";
	const char* kSliceAttackAnimName = "Dualwield_Melee_Attack_Slice";
	const char* kStabAttackAnimName = "Dualwield_Melee_Attack_Stab";
	const char* kSpinAttackAnimName = "2H_Melee_Attack_Spinning";
	// 被弾
	const char* kHitAnimName = "Hit_B";
	// 死亡
	const char* kDeadAnimName = "Death_B";

	// アニメーションの再生速度
	// 通常速度
	constexpr float kDefaultAnimSpeed = 0.5f;
	constexpr float kChopAnimSpeed = 0.4f;
	constexpr float kSliceAnimSpeed = 0.4f;
	constexpr float kStabAnimSpeed = 0.4f;
	constexpr float kSpinAnimSpeed = 0.4f;

	// 倒されたときにプレイヤーの必殺技ゲージを増やす量
	constexpr int kSpecialGaugePoint = 10;
}

EnemyBoss::EnemyBoss() :
	m_walkFrame(0.0f),
	EnemyBase(ObjectTag::Boss, ObjectPriority::Middle, ColliderData::Kind::Capsule)
{
}

EnemyBoss::~EnemyBoss()
{
	MV1DeleteModel(m_model);
}

void EnemyBoss::Init(Vector3& pos, Vector3& rot, Vector3& scale)
{
	// Physicsに登録
	Collidable::Init();
	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);

	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->m_startPos = pos;
	colData->m_radius = kCapsuleColRadius;

	m_playerFindRadius = kPlayerFindRadius;
	m_atkRadius = kAtkRadius;

	// 各ステータスの初期化
	m_status.m_hp = kHP;
	m_status.m_maxHP = kHP;
	m_status.m_atk = kDefaultAtk;
	m_atkFrame = 0.0f;
	m_isDead = false;

	// モデルのロード
	m_model = MV1LoadModel("Data/Model/Enemy/Minion/Minion.mv1");
	assert(m_model >= 0);

	VECTOR modelScale = VGet(scale.x * kModelScale, scale.y * kModelScale, scale.z * kModelScale);
	MV1SetScale(m_model, modelScale);
	MV1SetPosition(m_model, pos.ToDxVECTOR());
	MV1SetRotationXYZ(m_model, VGet(rot.x, rot.y, rot.z));

	// アニメーター
	m_anim.Init(m_model);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kFindAnimName, kDefaultAnimSpeed, true);

	// 武器
	m_leftWeapon = std::make_shared<EnemyBossLeftWeapon>();
	m_leftWeapon->Init();
	m_rightWeapon = std::make_shared<EnemyBossRightWeapon>();
	m_rightWeapon->Init();
}

void EnemyBoss::Draw()
{
	if (m_isDead && m_model < 0)
	{
		return;
	}

#if _DEBUG
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), 10.0f, 16, 0x0000ff, 0x0000ff, true);
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), m_playerFindRadius, 16, 0xff00ff, 0xff00ff, false);
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), m_atkRadius, 16, 0xff00ff, 0xff00ff, false);
#endif

	MV1DrawModel(m_model);
	m_leftWeapon->Draw();
	m_rightWeapon->Draw();
}

void EnemyBoss::Update(std::shared_ptr<Player> player)
{
	if (m_isDead && m_model < 0)
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
	case EnemyState::Walk:
		WalkUpdate(player);
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
	Vector3 colPos = m_rigidbody.GetPos();
	colPos.y += kColScale;
	colData->m_startPos = colPos;

	MV1SetPosition(m_model, m_rigidbody.GetPos().ToDxVECTOR());
}

void EnemyBoss::FindUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);
	m_rightWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Find));
	m_leftWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Find));

	float distance = (m_rigidbody.GetPos() - player->GetPos()).Length();
	if (distance <= (m_playerFindRadius + player->GetRadius()))
	{
		ChangeState(EnemyState::Chase);
	}
}

void EnemyBoss::WalkUpdate(std::shared_ptr<Player> player)
{
	SetActive(false);
	m_rightWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Walk));
	m_leftWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Walk));

	++m_walkFrame;

	// プレイヤーへの方向ベクトル
	Vector3 myPos = m_rigidbody.GetPos();
	Vector3 toPlayerDir = player->GetPos() - myPos;
	toPlayerDir.y = 0.0f;
	toPlayerDir.Normalize();
	m_rigidbody.SetVelo(toPlayerDir * kWalkSpeed);
	MV1SetPosition(m_model, myPos.ToDxVECTOR());

	// 進行方向が0でなければ回転
	if (m_rigidbody.GetVelo().x != 0.0f || m_rigidbody.GetVelo().z != 0.0f)
	{
		// atan2でY軸回転角を計算（Zが前、Xが右の座標系の場合）
		float angleY = std::atan2(m_rigidbody.GetVelo().x, -m_rigidbody.GetVelo().z);
		MV1SetRotationXYZ(m_model, VGet(0.0f, -angleY, 0.0f));
	}

	if (m_walkFrame >= 120.0f)
	{
		float distance = (myPos - player->GetPos()).Length();
		if (distance >= (m_playerFindRadius + player->GetRadius()))
		{
			ChangeState(EnemyState::Find);
		}

		if (distance <= (m_atkRadius + player->GetRadius()))
		{
			if (distance > 500.0f && m_prevState != EnemyState::Stab)
			{
				ChangeState(EnemyState::Stab);
			}
			else if (distance > 400.0f && m_prevState != EnemyState::Chop)
			{
				ChangeState(EnemyState::Chop);
			}
			else if (distance > 300.0f && m_prevState != EnemyState::Slice)
			{
				ChangeState(EnemyState::Slice);
			}
			else if (distance < 300.0f && m_prevState == EnemyState::Spin)
			{
				ChangeState(EnemyState::Slice);
			}
			else if (m_prevState != EnemyState::Spin)
			{
				ChangeState(EnemyState::Spin);
			}
		}
	}
}

void EnemyBoss::ChaseUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);
	m_rightWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Chase));
	m_leftWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Chase));

	// プレイヤーへの方向ベクトル
	Vector3 myPos = m_rigidbody.GetPos();
	Vector3 toPlayerDir = player->GetPos() - myPos;
	toPlayerDir.y = 0.0f;

	// 距離が十分にある場合のみ移動
	if (toPlayerDir.Length() > 1.0f)
	{
		toPlayerDir.Normalize();
		m_rigidbody.SetVelo(toPlayerDir * kChaseSpeed);
		MV1SetPosition(m_model, myPos.ToDxVECTOR());

		// 進行方向が0でなければ回転
		if (m_rigidbody.GetVelo().x != 0.0f || m_rigidbody.GetVelo().z != 0.0f)
		{
			// atan2でY軸回転角を計算（Zが前、Xが右の座標系の場合）
			float angleY = std::atan2(m_rigidbody.GetVelo().x, -m_rigidbody.GetVelo().z);
			MV1SetRotationXYZ(m_model, VGet(0.0f, -angleY, 0.0f));
		}
	}

	float distance = (myPos - player->GetPos()).Length();
	if (distance >= (m_playerFindRadius + player->GetRadius()))
	{
		ChangeState(EnemyState::Find);
	}

	if (distance <= (m_atkRadius + player->GetRadius()))
	{
		if (distance > 500.0f && m_prevState != EnemyState::Stab)
		{
			ChangeState(EnemyState::Stab);
		}
		else if (distance > 400.0f && m_prevState != EnemyState::Chop)
		{
			ChangeState(EnemyState::Chop);
		}
		else if (distance > 300.0f && m_prevState != EnemyState::Slice)
		{
			ChangeState(EnemyState::Slice);
		}
		else if (distance < 300.0f && m_prevState == EnemyState::Spin)
		{
			ChangeState(EnemyState::Slice);
		}
		else if (m_prevState != EnemyState::Spin)
		{
			ChangeState(EnemyState::Spin);
		}
	}
}

void EnemyBoss::AttackUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);
	++m_atkFrame;
	m_rightWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Attack));
	m_leftWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Attack));

	// プレイヤーへの方向ベクトル
	Vector3 myPos = m_rigidbody.GetPos();
	Vector3 dir = player->GetPos() - myPos;
	dir.y = 0.0f;

	if (m_atkFrame <= kAtkFrame)
	{
		++m_atkFrame;
		if (dir.x != 0.0f || dir.z != 0.0f)
		{
			// atan2でY軸回転角を計算（Zが前、Xが右の座標系の場合）
			float angleY = std::atan2(dir.x, -dir.z);
			MV1SetRotationXYZ(m_model, VGet(0.0f, -angleY, 0.0f));
		}
	}

	if (m_anim.GetNextAnim().isEnd)
	{
		float distance = (myPos - player->GetPos()).Length();
		if (distance >= (m_playerFindRadius + player->GetRadius()))
		{
			ChangeState(EnemyState::Find);
		}
		else
		{
			ChangeState(EnemyState::Chase);
		}
	}
}

void EnemyBoss::ChopUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);
	++m_atkFrame;
	m_rightWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Chop));
	m_leftWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Chop));

	// プレイヤーへの方向ベクトル
	Vector3 myPos = m_rigidbody.GetPos();
	Vector3 dir = player->GetPos() - myPos;
	dir.y = 0.0f;

	if (m_atkFrame <= kAtkFrame)
	{
		++m_atkFrame;
		if (dir.x != 0.0f || dir.z != 0.0f)
		{
			// atan2でY軸回転角を計算（Zが前、Xが右の座標系の場合）
			float angleY = std::atan2(dir.x, -dir.z);
			MV1SetRotationXYZ(m_model, VGet(0.0f, -angleY, 0.0f));
		}
	}

	if (m_anim.GetNextAnim().isEnd)
	{
		float distance = (myPos - player->GetPos()).Length();
		if (distance >= (m_playerFindRadius + player->GetRadius()))
		{
			ChangeState(EnemyState::Find);
		}
		else
		{
			ChangeState(EnemyState::Walk);
		}
	}
}

void EnemyBoss::SliceUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);
	++m_atkFrame;
	m_rightWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Slice));
	m_leftWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Slice));

	// プレイヤーへの方向ベクトル
	Vector3 myPos = m_rigidbody.GetPos();
	Vector3 dir = player->GetPos() - myPos;
	dir.y = 0.0f;

	if (m_atkFrame <= kAtkFrame)
	{
		++m_atkFrame;
		if (dir.x != 0.0f || dir.z != 0.0f)
		{
			// atan2でY軸回転角を計算（Zが前、Xが右の座標系の場合）
			float angleY = std::atan2(dir.x, -dir.z);
			MV1SetRotationXYZ(m_model, VGet(0.0f, -angleY, 0.0f));
		}
	}

	if (m_anim.GetNextAnim().isEnd)
	{
		float distance = (myPos - player->GetPos()).Length();
		if (distance >= (m_playerFindRadius + player->GetRadius()))
		{
			ChangeState(EnemyState::Find);
		}
		else
		{
			ChangeState(EnemyState::Walk);
		}
	}
}

void EnemyBoss::StabUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);
	++m_atkFrame;
	m_rightWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Stab));
	m_leftWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Stab));

	// プレイヤーへの方向ベクトル
	Vector3 myPos = m_rigidbody.GetPos();
	Vector3 dir = player->GetPos() - myPos;
	dir.y = 0.0f;

	if (m_atkFrame <= kAtkFrame)
	{
		++m_atkFrame;
		if (dir.x != 0.0f || dir.z != 0.0f)
		{
			// atan2でY軸回転角を計算（Zが前、Xが右の座標系の場合）
			float angleY = std::atan2(dir.x, -dir.z);
			MV1SetRotationXYZ(m_model, VGet(0.0f, -angleY, 0.0f));
		}
	}

	if (m_anim.GetNextAnim().isEnd)
	{
		float distance = (myPos - player->GetPos()).Length();
		if (distance >= (m_playerFindRadius + player->GetRadius()))
		{
			ChangeState(EnemyState::Find);
		}
		else
		{
			ChangeState(EnemyState::Walk);
		}
	}
}

void EnemyBoss::SpinUpdate(std::shared_ptr<Player> player)
{
	SetActive(true);
	++m_atkFrame;
	m_rightWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Spin));
	m_leftWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Spin));

	// プレイヤーへの方向ベクトル
	Vector3 myPos = m_rigidbody.GetPos();
	Vector3 dir = player->GetPos() - myPos;
	dir.y = 0.0f;

	if (m_atkFrame <= kAtkFrame)
	{
		++m_atkFrame;
		if (dir.x != 0.0f || dir.z != 0.0f)
		{
			// atan2でY軸回転角を計算（Zが前、Xが右の座標系の場合）
			float angleY = std::atan2(dir.x, -dir.z);
			MV1SetRotationXYZ(m_model, VGet(0.0f, -angleY, 0.0f));
		}
	}

	if (m_atkFrame >= 180.0f)
	{
		float distance = (myPos - player->GetPos()).Length();
		if (distance >= (m_playerFindRadius + player->GetRadius()))
		{
			ChangeState(EnemyState::Find);
		}
		else
		{
			ChangeState(EnemyState::Walk);
		}
	}
}

void EnemyBoss::HitUpdate(std::shared_ptr<Player> player)
{
	SetActive(false);
	m_rightWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Hit));
	m_leftWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Hit));

	MV1SetPosition(m_model, m_rigidbody.GetPos().ToDxVECTOR());
	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(EnemyState::Find);
	}
}

void EnemyBoss::DeadUpdate(std::shared_ptr<Player> player)
{
	SetActive(false);
	player->SetSpecialGauge(kSpecialGaugePoint);
	m_rightWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Dead));
	m_leftWeapon->Update(m_model, m_atkFrame, BossAtk::kColTimingTable.at(EnemyState::Dead));

	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		if (m_model >= 0)
		{
			MV1DeleteModel(m_model);
			m_model = -1;
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

float EnemyBoss::GetAnimPlaySpeed(EnemyState state) const
{
	switch (state)
	{
	case EnemyState::Find:
		return kDefaultAnimSpeed;
	case EnemyState::Walk:
		return kDefaultAnimSpeed;
	case EnemyState::Chase:
		return kDefaultAnimSpeed;
	case EnemyState::Attack:
		return kDefaultAnimSpeed;
	case EnemyState::Chop:
		return kChopAnimSpeed;
	case EnemyState::Slice:
		return kSliceAnimSpeed;
	case EnemyState::Stab:
		return kStabAnimSpeed;
	case EnemyState::Spin:
		return kSpinAnimSpeed;
	case EnemyState::Hit:
		return kDefaultAnimSpeed;
	case EnemyState::Dead:
		return kDefaultAnimSpeed;
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
