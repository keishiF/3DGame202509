#include "CapsuleColliderData.h"
#include "EnemyMinionWeapon.h"
#include "SubPlayer/SubPlayer.h"
#include "SubEnemyMinion.h"

namespace
{
	// HPの初期値、最大値
	constexpr float kHP = 5.0f;
	// 攻撃力
	constexpr float kDefaultAtk = 1.0f;

	// 移動速度
	constexpr float kWalkSpeed = 2.5f;
	constexpr float kRunSpeed = 5.0f;

	// モデルの拡大値
	constexpr float kModelScale = 75.0f;

	// 攻撃フレーム
	constexpr float kAtkFrame = 32.0f;

	// 当たり判定
	// カプセルの半径
	constexpr float kCapsuleColRadius = 45.0f;
	// カプセルの長さ
	constexpr float kColScale = 140.0f;
	// 状態遷移に使う半径
	// プレイヤーを探知できる範囲
	constexpr float kPlayerFindRadius = 900.0f;
	// 走り状態に移行する範囲
	constexpr float kRunRadius = 450.0f;
	// 攻撃状態に移行する範囲
	constexpr float kAtkRadius = 100.0f;

	// アニメーション名
	// 待機
	const char* kFindAnimName = "2H_Melee_Idle";
	// 歩き
	const char* kWalkAnimName = "Walking_D_Skeletons";
	// 発見
	const char* kChaseAnimName = "Running_C";
	// 攻撃
	const char* kAttackAnimName = "1H_Melee_Attack_Slice_Diagonal";
	// 被弾
	const char* kHitAnimName = "Hit_B";
	// 死亡
	const char* kDeadAnimName = "Death_B";

	// アニメーションの再生速度
	// 通常速度
	constexpr float kDefaultAnimSpeed = 0.5f;
	constexpr float kHitAnimSpeed = 1.0f;

	// 倒されたときにプレイヤーの必殺技ゲージを増やす量
	constexpr int kSpecialGaugePoint = 10;
}

SubEnemyMinion::SubEnemyMinion() :
	SubEnemyBase(ObjectTag::Enemy, ObjectPriority::Low, ColliderData::Kind::Capsule)
{
}

SubEnemyMinion::~SubEnemyMinion()
{
	MV1DeleteModel(m_model);
}

void SubEnemyMinion::Init(Vector3& pos, Vector3& rot, Vector3& scale)
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
	m_weapon = std::make_shared<EnemyMinionWeapon>();
	m_weapon->Init();
}

void SubEnemyMinion::Draw()
{
	if (m_isDead && m_model < 0)
	{
		return;
	}

#if _DEBUG
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), 10.0f, 16, 0x0000ff, 0x0000ff, true);
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), m_playerFindRadius, 16, 0xff00ff, 0xff00ff, false);
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), kRunRadius, 16, 0xff00ff, 0xff00ff, false);
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), m_atkRadius, 16, 0xff00ff, 0xff00ff, false);

#endif
	MV1DrawModel(m_model);
	m_weapon->Draw();
}

void SubEnemyMinion::Update(std::shared_ptr<SubPlayer> player)
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
	case SubEnemyState::Find:
		FindUpdate(player);
		break;
	case SubEnemyState::Walk:
		WalkUpdate(player);
		break;
	case SubEnemyState::Chase:
		ChaseUpdate(player);
		break;
	case SubEnemyState::Attack:
		AttackUpdate(player);
		break;
	case SubEnemyState::Hit:
		HitUpdate(player);
		break;
	case SubEnemyState::Dead:
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

void SubEnemyMinion::FindUpdate(std::shared_ptr<SubPlayer> player)
{
	SetActive(true);
	m_weapon->Update(m_model, m_atkFrame, MinionAtk::kColTimingTable.at(SubEnemyState::Find));

	float distance = (m_rigidbody.GetPos() - player->GetPos()).Length();
	if (distance <= (m_playerFindRadius + player->GetRadius()))
	{
		ChangeState(SubEnemyState::Walk);
	}
}

void SubEnemyMinion::WalkUpdate(std::shared_ptr<SubPlayer> player)
{
	SetActive(true);
	m_weapon->Update(m_model, m_atkFrame, MinionAtk::kColTimingTable.at(SubEnemyState::Chase));

	// プレイヤーへの方向ベクトル
	Vector3 myPos = m_rigidbody.GetPos();
	Vector3 toPlayerDir = player->GetPos() - myPos;
	toPlayerDir.y = 0.0f;

	// 距離が十分にある場合のみ移動
	if (toPlayerDir.Length() > 1.0f)
	{
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
	}

	float distance = (myPos - player->GetPos()).Length();
	if (distance >= (m_playerFindRadius + player->GetRadius()))
	{
		ChangeState(SubEnemyState::Find);
	}
	else if (distance <= (kRunRadius + player->GetRadius()))
	{
		ChangeState(SubEnemyState::Chase);
	}
	else if (distance <= (m_atkRadius + player->GetRadius()))
	{
		ChangeState(SubEnemyState::Attack);
	}
}

void SubEnemyMinion::ChaseUpdate(std::shared_ptr<SubPlayer> player)
{
	SetActive(true);
	m_weapon->Update(m_model, m_atkFrame, MinionAtk::kColTimingTable.at(SubEnemyState::Chase));

	// プレイヤーへの方向ベクトル
	Vector3 myPos = m_rigidbody.GetPos();
	Vector3 toPlayerDir = player->GetPos() - myPos;
	toPlayerDir.y = 0.0f;

	// 距離が十分にある場合のみ移動
	if (toPlayerDir.Length() > 1.0f)
	{
		toPlayerDir.Normalize();
		m_rigidbody.SetVelo(toPlayerDir * kRunSpeed);
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
		ChangeState(SubEnemyState::Find);
	}

	if (distance <= (m_atkRadius + player->GetRadius()))
	{
		ChangeState(SubEnemyState::Attack);
	}
}

void SubEnemyMinion::AttackUpdate(std::shared_ptr<SubPlayer> player)
{
	SetActive(true);
	++m_atkFrame;
	m_weapon->Update(m_model, m_atkFrame, MinionAtk::kColTimingTable.at(SubEnemyState::Attack));

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
			ChangeState(SubEnemyState::Find);
		}
		else
		{
			ChangeState(SubEnemyState::Chase);
		}
	}
}

void SubEnemyMinion::HitUpdate(std::shared_ptr<SubPlayer> player)
{
	SetActive(false);
	m_weapon->Update(m_model, m_atkFrame, MinionAtk::kColTimingTable.at(SubEnemyState::Hit));

	MV1SetPosition(m_model, m_rigidbody.GetPos().ToDxVECTOR());
	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(SubEnemyState::Find);
	}
}

void SubEnemyMinion::DeadUpdate(std::shared_ptr<SubPlayer> player)
{
	SetActive(false);
	m_weapon->Update(m_model, m_atkFrame, MinionAtk::kColTimingTable.at(SubEnemyState::Dead));

	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		if (m_model >= 0)
		{
			MV1DeleteModel(m_model);
			m_model = -1;
		}
		player->SetSpecialGauge(kSpecialGaugePoint);
		m_isDead = true;
		return;
	}
}

const char* SubEnemyMinion::GetAnimName(SubEnemyState state) const
{
	switch(state)
	{
	case SubEnemyState::Find:
		return kFindAnimName;
	case SubEnemyState::Walk:
		return kWalkAnimName;
	case SubEnemyState::Chase:
		return kChaseAnimName;
	case SubEnemyState::Attack:
		return kAttackAnimName;
	case SubEnemyState::Hit:
		return kHitAnimName;
	case SubEnemyState::Dead:
		return kDeadAnimName;
	default:
		return "";
		assert(0 && "存在しないアニメーション");
	}
}

float SubEnemyMinion::GetAnimPlaySpeed(SubEnemyState state) const
{
	switch (state)
	{
	case SubEnemyState::Find:
		return kDefaultAnimSpeed;
	case SubEnemyState::Walk:
		return kDefaultAnimSpeed;
	case SubEnemyState::Chase:
		return kDefaultAnimSpeed;
	case SubEnemyState::Attack:
		return kDefaultAnimSpeed;
	case SubEnemyState::Hit:
		return kHitAnimSpeed;
	case SubEnemyState::Dead:
		return kDefaultAnimSpeed;
	}
}

bool SubEnemyMinion::IsLoopAnim(SubEnemyState state) const
{
	switch (state)
	{
	case SubEnemyState::Find:
		return true;
	case SubEnemyState::Walk:
		return true;
	case SubEnemyState::Chase:
		return true;
	case SubEnemyState::Attack:
		return false;
	case SubEnemyState::Hit:
		return false;
	case SubEnemyState::Dead:
		return false;
	default:
		return "";
		assert(0 && "存在しないステート");
	}
}
