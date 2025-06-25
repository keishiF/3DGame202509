#include "Player.h"
#include "PlayerWeapon.h"

#include "CapsuleColliderData.h"
#include "Physics.h"
#include "Animation.h"

#include "DxLib.h"
#include <cassert>

namespace
{
	// アニメーション名
	// 待機
	const char* kFindAnimName         = "Idle";
	// 歩き
	const char* kWalkAnimName         = "Walking_B";
	// 走り
	const char* kRunAnimName          = "Running_A";
	// 攻撃
	const char* kChopAnimName	      = "1H_Melee_Attack_Chop";
	const char* kSliceAnimName        = "1H_Melee_Attack_Slice_Diagonal";
	const char* kStabAnimName         = "1H_Melee_Attack_Stab";
	const char* kSpinAnimName         = "2H_Melee_Attack_Spin";
	const char* kUltimateAnimName     = "";
	// 回避
	const char* kDodgeAnimName		  = "Dodge_Forward";
	// 被弾
	const char* kHitAnimName		  = "Hit_B";
	// 死亡
	const char* kDeadAnimName         = "Death_B";

	// プレイヤーの当たり判定
	constexpr float kRadius = 15.0f;
	constexpr float kColScale = 90.0f;
	// HPの初期値
	constexpr int kHp = 100;
	// 移動速度
	constexpr float kWalkSpeed = 1.5f;
	constexpr float kRunSpeed = 4.5f;
	// プレイヤーのモデルの拡大値
	constexpr float kModelScale = 45.0f;

	constexpr float kBladeModelScale = 0.01f;
}

Player::Player(std::shared_ptr<Physics> physics) :
	m_charModel(-1),
	m_radius(kRadius),
	m_hp(kHp),
	m_isCombo(false),
	m_isDead(false),
	m_frameCount(0.0f),
	m_state(PlayerState::Idle),
	Collidable(ObjectTag::Player, ObjectPriority::High, ColliderData::Kind::Capsule)
{
}

Player::~Player()
{
	MV1DeleteModel(m_charModel);
}

void Player::Init(std::shared_ptr<Physics> physics)
{
	Collidable::Init(physics);
	Vec3 pos = { 0.0f, 0.0f, -500.0f };
	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);

	//当たり判定
	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->m_startPos = pos;
	colData->m_radius = kRadius;

	m_charModel = MV1LoadModel("Data/Player/PlayerNoSword.mv1");
	assert(m_charModel >= 0);
	MV1SetScale(m_charModel, VGet(kModelScale, kModelScale, kModelScale));
	MV1SetPosition(m_charModel, pos.ToDxVECTOR());

	m_anim.Init(m_charModel);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kFindAnimName, true);

	m_weapon = std::make_shared<PlayerWeapon>();
	m_weapon->Init(physics);
}

void Player::Update()
{	
	// アニメーションの更新
	m_anim.UpdateAnim(m_anim.GetPrevAnim());
	m_anim.UpdateAnim(m_anim.GetNextAnim());
	m_anim.UpdateAnimBlend();

	switch (m_state)
	{
	case PlayerState::Idle:
		IdleUpdate();
		break;
	case PlayerState::Walk:
		WalkUpdate();
		break;
	case PlayerState::Run:
		RunUpdate();
		break;
	case PlayerState::Chop:
		ChopUpdate();
		break;
	case PlayerState::Slice:
		SliceUpdate();
		break;
	case PlayerState::Stab:
		StabUpdate();
		break;
	case PlayerState::Spin:
		SpinUpdate();
		break;
	case PlayerState::Ultimate:
		UltimateUpdate();
		break;
	case PlayerState::Dodge:
		DodgeUpdate();
		break;
	case PlayerState::Hit:
		HitUpdate();
		break;
	case PlayerState::Dead:
		DeadUpdate();
		break;
	}

	//当たり判定
	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	Vec3 colPos = m_rigidbody.GetPos();
	colPos.y += kColScale;
	colData->m_startPos = colPos;

	m_weapon->IdleUpdate(m_charModel);
}

void Player::Draw()
{
#if _DEBUG
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), m_radius, 16, 0x00ff00, 0x00ff00, false);
#endif
	MV1DrawModel(m_charModel);
	m_weapon->Draw();
}

void Player::OnDamage()
{
	ChangeState(PlayerState::Hit);
	m_hp--;
}

void Player::OnCollide(std::shared_ptr<Collidable> collider)
{
	OnDamage();
}

void Player::ChangeState(PlayerState newState)
{
	// 現在の状態と次の状態が同じ場合return
	if (m_state == newState) return;

	m_state = newState;

	switch (m_state)
	{
	case PlayerState::Idle:
		m_anim.ChangeAnim(kFindAnimName, true);
		break;
	case PlayerState::Walk:
		m_anim.ChangeAnim(kWalkAnimName, true);
		break;
	case PlayerState::Run:
		m_anim.ChangeAnim(kRunAnimName, true);
		break;
	case PlayerState::Chop:
		m_anim.ChangeAnim(kChopAnimName, false);
		break;
	case PlayerState::Slice:
		m_anim.ChangeAnim(kSliceAnimName, false);
		break;
	case PlayerState::Stab:
		m_anim.ChangeAnim(kStabAnimName, false);
		break;
	case PlayerState::Spin:
		m_anim.ChangeAnim(kSpinAnimName, false);
		break;
	case PlayerState::Ultimate:
		m_anim.ChangeAnim(kUltimateAnimName, false);
		break;
	case PlayerState::Dodge:
		m_anim.ChangeAnim(kDodgeAnimName, false);
		break;
	case PlayerState::Hit:
		m_anim.ChangeAnim(kHitAnimName, false);
		break;
	case PlayerState::Dead:
		m_anim.ChangeAnim(kDeadAnimName, false);
		break;
	}
}

void Player::IdleUpdate()
{
	// 左スティックの入力があれば歩き状態に移行する
	if (Input::Instance().IsPress("LEFT") || Input::Instance().IsPress("RIGHT") ||
		Input::Instance().IsPress("UP")   || Input::Instance().IsPress("DOWN"))
	{
		ChangeState(PlayerState::Walk);
	}

	// Aボタンの入力があれば攻撃状態に移行する
	if (Input::Instance().IsTrigger("A"))
	{
		ChangeState(PlayerState::Chop);
	}

	// Xボタンの入力があれば強攻撃状態に移行するためのフラグを立てる
	if (Input::Instance().IsTrigger("X"))
	{
		ChangeState(PlayerState::Spin);
	}
#if _DEBUG
	// 死亡状態に移行
	if (Input::Instance().IsTrigger("LB"))
	{
		ChangeState(PlayerState::Dead);
	}
#endif
}

void Player::WalkUpdate()
{
	Vec3 dir = { 0.0f, 0.0f,0.0f };
	// 左スティックで移動
	// 左入力
	if (Input::Instance().IsPress("LEFT"))
	{
		dir.x = -kWalkSpeed;
	}
	// 右入力
	else if (Input::Instance().IsPress("RIGHT"))
	{
		dir.x = kWalkSpeed;
	}
	// 横方向の入力なし
	else
	{
		dir.x = 0.0f;
	}
	// 上入力
	if (Input::Instance().IsPress("UP"))
	{
		dir.z = kWalkSpeed;
	}
	// 下入力
	else if (Input::Instance().IsPress("DOWN"))
	{
		dir.z = -kWalkSpeed;
	}
	// 縦方向の入力なし
	else
	{
		dir.z = 0.0f;
	}

	// ベクトルを正規化し移動速度をかけポジションに加算
	dir.Normalize();
	m_rigidbody.SetVelo(dir * kWalkSpeed);
	MV1SetPosition(m_charModel, m_rigidbody.GetPos().ToDxVECTOR());

	// 進行方向が0でなければ回転
	if (m_rigidbody.GetVelo().x != 0.0f || m_rigidbody.GetVelo().z != 0.0f)
	{
		// atan2でY軸回転角を計算（Zが前、Xが右の座標系の場合）
		float angleY = std::atan2(m_rigidbody.GetVelo().x, -m_rigidbody.GetVelo().z);
		MV1SetRotationXYZ(m_charModel, VGet(0.0f, -angleY, 0.0f));
	}

	// 左スティックの入力がない場合待機状態に移行
	if (m_rigidbody.GetVelo().x == 0.0f && m_rigidbody.GetVelo().z == 0.0f)
	{
		ChangeState(PlayerState::Idle);
	}

	// Lスティック押し込みの入力があればダッシュ状態に移行する
	if (Input::Instance().IsTrigger("LPush"))
	{
		ChangeState(PlayerState::Run);
		m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
	}

	// Aボタンの入力があれば攻撃状態に移行する
	if (Input::Instance().IsTrigger("A"))
	{
		ChangeState(PlayerState::Chop);
		m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
	}

	// Xボタンの入力があれば強攻撃状態に移行する
	if (Input::Instance().IsTrigger("X"))
	{
		ChangeState(PlayerState::Spin);
		m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
	}

	// Bボタンの入力があれば回避状態に移行する
	if (Input::Instance().IsTrigger("B"))
	{
		ChangeState(PlayerState::Dodge);
		m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
	}
}

void Player::RunUpdate()
{
	Vec3 dir = { 0.0f, 0.0f, 0.0f };
	// 左スティックで移動
	// 左入力
	if (Input::Instance().IsPress("LEFT"))
	{
		dir.x = -kRunSpeed;
	}
	// 右入力
	else if (Input::Instance().IsPress("RIGHT"))
	{
		dir.x = kRunSpeed;
	}
	// 横方向の入力なし
	else
	{
		dir.x = 0.0f;
	}
	// 上入力
	if (Input::Instance().IsPress("UP"))
	{
		dir.z = kRunSpeed;
	}
	// 下入力
	else if (Input::Instance().IsPress("DOWN"))
	{
		dir.z = -kRunSpeed;
	}
	// 縦方向の入力なし
	else
	{
		dir.z = 0.0f;
	}

	// ベクトルを正規化し移動速度をかけポジションに加算
	dir.Normalize();
	m_rigidbody.SetVelo(dir * kRunSpeed);
	MV1SetPosition(m_charModel, m_rigidbody.GetPos().ToDxVECTOR());

	// 進行方向が0でなければ回転
	if (m_rigidbody.GetVelo().x != 0.0f || m_rigidbody.GetVelo().z != 0.0f)
	{
		// atan2でY軸回転角を計算（Zが前、Xが右の座標系の場合）
		float angleY = std::atan2(m_rigidbody.GetVelo().x, -m_rigidbody.GetVelo().z);
		MV1SetRotationXYZ(m_charModel, VGet(0.0f, -angleY, 0.0f));
	}

	// 左スティックの入力がない場合待機状態に移行する
	if (m_rigidbody.GetVelo().x == 0.0f && m_rigidbody.GetVelo().z == 0.0f)
	{
		ChangeState(PlayerState::Idle);
	}

	// 左スティック押し込みの入力があれば歩き状態に移行する
	if (Input::Instance().IsTrigger("LPush"))
	{
		ChangeState(PlayerState::Walk);
		m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
	}

	// Aボタンの入力があれば攻撃状態に移行する
	if (Input::Instance().IsTrigger("A"))
	{
		ChangeState(PlayerState::Chop);
		m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
	}

	// Xボタンの入力があれば強攻撃状態に移行する
	if (Input::Instance().IsTrigger("X"))
	{
		ChangeState(PlayerState::Spin);
		m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
	}

	// Bボタンの入力があれば回避状態に移行する
	if (Input::Instance().IsTrigger("B"))
	{
		ChangeState(PlayerState::Dodge);
		m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
	}
}

void Player::ChopUpdate()
{
	m_weapon->AttackUpdate(m_charModel);

	// 1ボタンの入力があれば攻撃状態に移行する
	if (Input::Instance().IsTrigger("A"))
	{
		m_isCombo = true;
	}

	// アニメーションが終了時
	if (m_anim.GetNextAnim().isEnd)
	{
		// 1ボタンの入力があれば攻撃状態に移行する
		if (m_isCombo)
		{
			ChangeState(PlayerState::Slice);
			m_isCombo = false;
		}
		// 何もなければ待機状態に移行する
		else
		{
			ChangeState(PlayerState::Idle);
		}
	}
}

void Player::SliceUpdate()
{
	m_weapon->AttackUpdate(m_charModel);

	// 1ボタンの入力があれば攻撃状態に移行する
	if (Input::Instance().IsTrigger("A"))
	{
		m_isCombo = true;
	}

	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		// 1ボタンの入力があれば攻撃状態に移行する
		if (m_isCombo)
		{
			ChangeState(PlayerState::Stab);
			m_isCombo = false;
		}
		else
		{
			ChangeState(PlayerState::Idle);
		}
	}
}

void Player::StabUpdate()
{
	m_weapon->AttackUpdate(m_charModel);

	// 1ボタンの入力があれば攻撃状態に移行する
	if (Input::Instance().IsTrigger("A"))
	{
		m_isCombo = true;
	}

	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		// 1ボタンの入力があれば攻撃状態に移行する
		if (m_isCombo)
		{
			ChangeState(PlayerState::Chop);
			m_isCombo = false;
		}
		else
		{
			ChangeState(PlayerState::Idle);
		}
	}
}

void Player::SpinUpdate()
{
	m_weapon->AttackUpdate(m_charModel);

	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::UltimateUpdate()
{
	m_weapon->AttackUpdate(m_charModel);

	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::DodgeUpdate()
{
	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::HitUpdate()
{
	MV1SetPosition(m_charModel, m_rigidbody.GetPos().ToDxVECTOR());
	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
		m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
	}
}

void Player::DeadUpdate()
{
	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		m_isDead = true;
	}
}

