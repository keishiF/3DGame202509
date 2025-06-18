#include "Player.h"

#include "CapsuleColliderData.h"
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
	// HPの初期値
	constexpr int kHp = 100;
	// 移動速度
	constexpr float kWalkSpeed = 1.5f;
	constexpr float kRunSpeed = 4.5f;
	// プレイヤーのモデルの拡大値
	constexpr float kModelScale = 45.0f;
}

Player::Player() :
	m_model(-1),
	m_pos(0.0f, 0.0f, -500.0f),
	m_radius(kRadius),
	m_vec(0.0f, 0.0f, 0.0f),
	m_hp(kHp),
	m_isCombo(false),
	m_isDead(false),
	m_frameCount(0.0f),
	m_state(PlayerState::Idle),
	Collidable(GameObjectTag::Player, ColliderData::Kind::Capsule)
{
	m_model = MV1LoadModel("Data/Player/Player.mv1");
	assert(m_model >= 0);
	MV1SetScale(m_model, VGet(kModelScale, kModelScale, kModelScale));
	MV1SetPosition(m_model, VGet(m_pos.x, m_pos.y, m_pos.z));

	m_anim.Init(m_model);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kFindAnimName, true);
}

Player::~Player()
{
	MV1DeleteModel(m_model);
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
}

void Player::Draw()
{
#if _DEBUG
	DrawSphere3D(VGet(m_pos.x, m_pos.y, m_pos.z), m_radius, 16, 0x00ff00, 0x00ff00, false);
	/*DrawCapsule3D(VGet(m_pos.x, m_pos.y + 10.0f, m_pos.z), VGet(m_pos.x, m_pos.y + 90.0f, m_pos.z),
		m_radius, 16, 0xff00ff, 0xff00ff, false);*/
#endif

	MV1DrawModel(m_model);
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
	// 左スティックで移動
	// 左入力
	if (Input::Instance().IsPress("LEFT"))
	{
		m_vec.x = -kWalkSpeed;
	}
	// 右入力
	else if (Input::Instance().IsPress("RIGHT"))
	{
		m_vec.x = kWalkSpeed;
	}
	// 横方向の入力なし
	else
	{
		m_vec.x = 0.0f;
	}
	// 上入力
	if (Input::Instance().IsPress("UP"))
	{
		m_vec.z = kWalkSpeed;
	}
	// 下入力
	else if (Input::Instance().IsPress("DOWN"))
	{
		m_vec.z = -kWalkSpeed;
	}
	// 縦方向の入力なし
	else
	{
		m_vec.z = 0.0f;
	}

	// ベクトルを正規化し移動速度をかけポジションに加算
	m_vec.Normalize();
	m_vec *= kWalkSpeed;
	m_pos += m_vec;
	MV1SetPosition(m_model, VGet(m_pos.x, m_pos.y, m_pos.z));

	// 進行方向が0でなければ回転
	if (m_vec.x != 0.0f || m_vec.z != 0.0f)
	{
		// atan2でY軸回転角を計算（Zが前、Xが右の座標系の場合）
		float angleY = std::atan2(m_vec.x, -m_vec.z);
		MV1SetRotationXYZ(m_model, VGet(0.0f, -angleY, 0.0f));
	}

	// 左スティックの入力がない場合待機状態に移行
	if (m_vec.x == 0.0f && m_vec.z == 0.0f)
	{
		ChangeState(PlayerState::Idle);
	}

	// Lスティック押し込みの入力があればダッシュ状態に移行する
	if (Input::Instance().IsTrigger("LPush"))
	{
		ChangeState(PlayerState::Run);
	}

	// Aボタンの入力があれば攻撃状態に移行する
	if (Input::Instance().IsTrigger("A"))
	{
		ChangeState(PlayerState::Chop);
	}

	// Xボタンの入力があれば今日攻撃状態に移行する
	if (Input::Instance().IsTrigger("X"))
	{
		ChangeState(PlayerState::Spin);
	}

	// Bボタンの入力があれば回避状態に移行する
	if (Input::Instance().IsTrigger("B"))
	{
		ChangeState(PlayerState::Dodge);
	}
}

void Player::RunUpdate()
{
	// 左スティックで移動
	// 左入力
	if (Input::Instance().IsPress("LEFT"))
	{
		m_vec.x = -kRunSpeed;
	}
	// 右入力
	else if (Input::Instance().IsPress("RIGHT"))
	{
		m_vec.x = kRunSpeed;
	}
	// 横方向の入力なし
	else
	{
		m_vec.x = 0.0f;
	}
	// 上入力
	if (Input::Instance().IsPress("UP"))
	{
		m_vec.z = kRunSpeed;
	}
	// 下入力
	else if (Input::Instance().IsPress("DOWN"))
	{
		m_vec.z = -kRunSpeed;
	}
	// 縦方向の入力なし
	else
	{
		m_vec.z = 0.0f;
	}

	// ベクトルを正規化し移動速度をかけポジションに加算
	m_vec.Normalize();
	m_vec *= kRunSpeed;
	m_pos += m_vec;
	MV1SetPosition(m_model, VGet(m_pos.x, m_pos.y, m_pos.z));

	// 進行方向が0でなければ回転
	if (m_vec.x != 0.0f || m_vec.z != 0.0f)
	{
		// atan2でY軸回転角を計算（Zが前、Xが右の座標系の場合）
		float angleY = std::atan2(m_vec.x, -m_vec.z);
		MV1SetRotationXYZ(m_model, VGet(0.0f, -angleY, 0.0f));
	}

	// 左スティックの入力がない場合待機状態に移行する
	if (m_vec.x == 0.0f && m_vec.z == 0.0f)
	{
		ChangeState(PlayerState::Idle);
	}

	// 左スティック押し込みの入力があれば歩き状態に移行する
	if (Input::Instance().IsTrigger("LPush"))
	{
		ChangeState(PlayerState::Walk);
	}

	// Aボタンの入力があれば攻撃状態に移行する
	if (Input::Instance().IsTrigger("A"))
	{
		ChangeState(PlayerState::Chop);
	}

	// Xボタンの入力があれば強攻撃状態に移行する
	if (Input::Instance().IsTrigger("X"))
	{
		ChangeState(PlayerState::Spin);
	}

	// Bボタンの入力があれば回避状態に移行する
	if (Input::Instance().IsTrigger("B"))
	{
		ChangeState(PlayerState::Dodge);
	}
}

void Player::ChopUpdate()
{
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
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::UltimateUpdate()
{
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
	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
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

