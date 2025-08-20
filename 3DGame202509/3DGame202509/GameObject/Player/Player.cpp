#include "Animator.h"
#include "CapsuleColliderData.h"
#include "Enemy/EnemyBase.h"
#include "GameObjectManager.h"
#include "game.h"
#include "Physics.h"
#include "Player.h"
#include "PlayerBullet.h"
#include "PlayerLeftWeapon.h"
#include "PlayerRightWeapon.h"
#include <algorithm>
#include <cassert>
#include <DxLib.h>
#include <EffekseerForDxLib.h>
#include <unordered_map>

namespace
{
	// HPの初期値
	constexpr float kHp = 20.0f;
	// スタミナの最大値
	constexpr float kMaxStamina = 100.0f;
	// 必殺技ゲージの最大値
	constexpr float kMaxSpecialGauge = 100.0f;
	// スタミナ消費量
	// 待機状態、歩き状態で毎フレーム回復するスタミナ
	constexpr float kRegeneStamina = 0.34f;
	// 疲れ状態で毎フレーム回復するスタミナ
	constexpr float kTiredRegeneStamina = 0.17f;
	// 走っている際に消費するスタミナ
	constexpr float kRunStamina = 0.25f;
	// 強攻撃をした際に消費するスタミナ
	constexpr float kSpinStamina = 15.0f;
	// スタミナがこの値を下回ったら走るのが遅くなるライン
	constexpr float kRunSpeedDownStaminaRate = 35.0f;

	// 移動速度
	constexpr float kWalkSpeed = 8.5f;
	constexpr float kNormalRunSpeed = 17.5f;
	constexpr float kTiredRunSpeed = 5.0f;
	constexpr float kAttackMoveSpeed = 1.0f;
	constexpr float kDodgeSpped = 40.0f;
	// プレイヤーのモデルの拡大値
	constexpr float kModelScale = 70.0f;
	// プレイヤーの当たり判定
	constexpr float kRadius = 45.0f;
	constexpr float kColScale = 140.0f;
	constexpr float kAttackOffsetRadius = 230.0f;

	constexpr float kLerpT = 0.2f;

	// アニメーション名
	// 待機
	const char* kIdleAnimName    = "Idle";
	// 歩き
	const char* kWalkAnimName    = "Walking_B";
	// 走り
	const char* kRunAnimName     = "Running_A";
	// 疲れ
	const char* kTiredAnimName   = "Sit_Floor_Idle";
	// 攻撃
	const char* kChopAnimName    = "1H_Melee_Attack_Chop";
	const char* kSliceAnimName   = "1H_Melee_Attack_Slice_Diagonal";
	const char* kStabAnimName    = "1H_Melee_Attack_Stab";
	const char* kSpinAnimName    = "2H_Melee_Attack_Spin";
	const char* kShotAnimName    = "2H_Melee_Attack_Stab";
	const char* kSpecialAnimName = "2H_Melee_Attack_Stab";
	// 回避
	const char* kDodgeAnimName   = "Dodge_Forward";
	// 被弾
	const char* kHitAnimName     = "Hit_B";
	// 死亡
	const char* kDeadAnimName    = "Death_B";
	// アニメーションの再生速度
	constexpr float kAnimSpeed = 1.0f;
	constexpr float kIdleAnimSpeed = 0.5f;
	constexpr float kWalkAnimSpeed = 0.75f;
	constexpr float kSlowRunAnimSpeed = 0.5f;
	constexpr float kChopAnimSpeed = 1.0f;
	constexpr float kSpecialAttackAnimSpeed = 0.5f;

	constexpr float kShotTiming = 10.0f;

	const std::unordered_map<PlayerState, RightAttackTiming> kRightColTimingTable =
	{
		{PlayerState::Idle,		 { 0,  0}},
		{PlayerState::Walk,		 { 0,  0}},
		{PlayerState::NormalRun, { 0,  0}},
		{PlayerState::Tired,	 { 0,  0}},
		{PlayerState::Chop,		 {16, 28}},
		{PlayerState::Slice,	 {16, 28}},
		{PlayerState::Stab,		 {16, 28}},
		{PlayerState::Spin,      {16, 36}},
		{PlayerState::Shot,      { 0,  0}},
		{PlayerState::Special,   { 0, 36}},
		{PlayerState::Dodge,	 { 0,  0}},
		{PlayerState::Hit,		 { 0,  0}},
		{PlayerState::Dead,		 { 0,  0}}
	};

	const std::unordered_map<PlayerState, LeftAttackTiming> kLeftColTimingTable =
	{
		{PlayerState::Idle,		 { 0,  0}},
		{PlayerState::Walk,		 { 0,  0}},
		{PlayerState::NormalRun, { 0,  0}},
		{PlayerState::Tired,	 { 0,  0}},
		{PlayerState::Chop,		 {16, 28}},
		{PlayerState::Slice,	 {16, 28}},
		{PlayerState::Stab,		 {16, 28}},
		{PlayerState::Spin,      {16, 36}},
		{PlayerState::Shot,      { 0,  0}},
		{PlayerState::Special,   {16, 36}},
		{PlayerState::Dodge,	 { 0,  0}},
		{PlayerState::Hit,		 { 0,  0}},
		{PlayerState::Dead,		 { 0,  0}}
	};
}

Player::Player() :
	m_forward(0.0f, 0.0f, 1.0f),
	m_charModel(-1),
	m_radius(kRadius),
	m_hp(kHp),
	m_hpRate(m_hp / kHp),
	m_stamina(kMaxStamina),
	m_staminaRate(m_stamina / kMaxStamina),
	m_specialGauge(0),
	m_specialGaugeRate(m_specialGauge / kMaxSpecialGauge),
	m_isCombo(false),
	m_isDead(false),
	m_attackPower(1),
	m_frame(0.0),
	m_blinkFrame(0.0f),
	m_attackFrame(0.0f),
	m_specialEffect(-1),
	m_state(PlayerState::Idle),
	Collidable(ObjectTag::Player, ObjectPriority::High, ColliderData::Kind::Capsule)
{
}

Player::~Player()
{
	MV1DeleteModel(m_charModel);
}

void Player::Init(Vector3& pos, const Vector3& rot, const Vector3& scale)
{
	Collidable::Init();

	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);

	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->m_startPos = pos;
	colData->m_radius = kRadius;

	m_charModel = MV1LoadModel("Data/Model/Player/Player.mv1");
	assert(m_charModel >= 0);

	MV1SetScale(m_charModel, VGet(scale.x * kModelScale, scale.y * kModelScale, scale.z * kModelScale));
	MV1SetPosition(m_charModel, pos.ToDxVECTOR());

	/*m_specialEffect = LoadEffekseerEffect("Data/Effect/PlayerSpecialAttack.efkefc", 100.0f);
	assert(m_specialEffect >= 0);*/

	m_anim.Init(m_charModel);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kIdleAnimName, kIdleAnimSpeed, true);

	m_rightWeapon = std::make_shared<PlayerRightWeapon>();
	m_rightWeapon->Init();

	/*m_leftWeapon = std::make_shared<PlayerLeftWeapon>();
	m_leftWeapon->Init();*/
}

void Player::Update()
{
	if (m_isDead && m_charModel < 0)
	{
		return;
	}

	// 必殺技ゲージが最大でないとき
	if (m_specialGauge < kMaxSpecialGauge)
	{
		if (++m_frame >= 60)
		{
			m_specialGauge += 2.0f;
			m_specialGaugeRate = m_specialGauge / kMaxSpecialGauge;
			m_specialGaugeRate = std::clamp(m_specialGaugeRate, 0.0f, 1.0f);

			m_frame = 0.0f;
		}
	}

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
	case PlayerState::NormalRun:
		RunUpdate();
		break;
	case PlayerState::TiredRun:
		TiredRunUpdate();
		break;
	case PlayerState::Tired:
		TiredUpdate();
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
	case PlayerState::Shot:
		ShotUpdate();
		break;
	case PlayerState::Special:
		SpecialUpdate();
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

	for (auto& bullet : m_bullets)
	{
		bullet->Update();
	}
	m_bullets.erase(std::remove_if(m_bullets.begin(), m_bullets.end(), [this](const std::shared_ptr<PlayerBullet>& b)
		{
			if (b->IsDead())
			{
				b->Final();
				return true;
			}
			return false;
		}),
		m_bullets.end()
	);

	//当たり判定
	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	Vector3 colPos = m_rigidbody.GetPos();
	colPos.y += kColScale;
	colData->m_startPos = colPos;

	VECTOR rotVec = MV1GetRotationXYZ(m_charModel);
	float angleY = -rotVec.y;
	m_forward = Vector3(std::sin(angleY), 0.0f, -std::cos(angleY));
	m_forward.Normalize();
}

void Player::Draw()
{
	if (m_isDead && m_charModel < 0)
	{
		return;
	}

	// 点滅効果のための条件
	if ((m_blinkFrame / 3) % 2 == 0)
	{
#if _DEBUG
		DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), m_radius, 16, 0x00ff00, 0x00ff00, false);
		DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), kAttackOffsetRadius, 16, 0xff0000, 0xff0000, false);
#endif
		MV1DrawModel(m_charModel);
		m_rightWeapon->Draw();
	}
}

void Player::SetSpecialGauge(int specialGaugePoint)
{
	m_specialGauge += specialGaugePoint;

	m_specialGaugeRate = m_specialGauge / kMaxSpecialGauge;
	m_specialGaugeRate = std::clamp(m_specialGaugeRate, 0.0f, 1.0f);
}

void Player::OnDamage()
{
	// ダメージを受ける
	m_hp -= 1.0f;
	// HP割合を計算する
	m_hpRate = m_hp / kHp;
	m_hpRate = std::clamp(m_hpRate, 0.0f, 1.0f);

	// HPが0になったのなら死亡状態に移行する
	if (m_hp <= 0 && !m_isDead)
	{
		ChangeState(PlayerState::Dead);
	}
	// そうでないなら被弾状態に移行する
	else
	{
		ChangeState(PlayerState::Hit);
	}
}

void Player::OnCollide(std::shared_ptr<Collidable> collider)
{
	OnDamage();
}

void Player::ChangeState(PlayerState newState)
{
	// 現在の状態と次の状態が同じ場合return
	// Hitだけ例外処理
	if (m_state == newState && m_state != PlayerState::Hit) return;

	m_state = newState;

	m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
	m_attackFrame = 0.0f;
	m_blinkFrame = 0;
	m_isCombo = false;

	VECTOR rotVec = MV1GetRotationXYZ(m_charModel);
	float angleY = -rotVec.y;
	Vector3 axis(0.0f, 1.0f, 0.0f);
	m_currentRot.AngleAxis(angleY, axis);

	switch (m_state)
	{
	case PlayerState::Idle:
		m_anim.ChangeAnim(kIdleAnimName, kIdleAnimSpeed, true);
		break;
	case PlayerState::Walk:
		m_anim.ChangeAnim(kWalkAnimName, kWalkAnimSpeed, true);
		break;
	case PlayerState::NormalRun:
		m_anim.ChangeAnim(kRunAnimName, kAnimSpeed, true);
		break;
	case PlayerState::TiredRun:
		m_anim.ChangeAnim(kRunAnimName, kSlowRunAnimSpeed, true);
		break;
	case PlayerState::Tired:
		m_anim.ChangeAnim(kTiredAnimName, kAnimSpeed, true);
		break;
	case PlayerState::Chop:
		m_anim.ChangeAnim(kChopAnimName, kChopAnimSpeed, false);
		break;
	case PlayerState::Slice:
		m_anim.ChangeAnim(kSliceAnimName, kAnimSpeed, false);
		break;
	case PlayerState::Stab:
		m_anim.ChangeAnim(kStabAnimName, kAnimSpeed, false);
		break;
	case PlayerState::Spin:
		m_anim.ChangeAnim(kSpinAnimName, kAnimSpeed, false);
		break;
	case PlayerState::Shot:
		m_anim.ChangeAnim(kStabAnimName, kAnimSpeed, false);
		break;
	case PlayerState::Special:
		m_anim.ChangeAnim(kSpecialAnimName, kSpecialAttackAnimSpeed, false);
		break;
	case PlayerState::Dodge:
		m_anim.ChangeAnim(kDodgeAnimName, kAnimSpeed, false);
		break;
	case PlayerState::Hit:
		m_anim.ChangeAnim(kHitAnimName, kAnimSpeed, false);
		break;
	case PlayerState::Dead:
		m_anim.ChangeAnim(kDeadAnimName, kAnimSpeed, false);
		break;
	}
}

void Player::IdleUpdate()
{
	auto& input = Input::GetInstance();
	// プレイヤー自身の当たり判定をオンにする
	SetActive(true);

	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Idle), false);

	// スタミナが最大じゃないときは徐々に回復する
	if (m_stamina < kMaxStamina)
	{
		m_stamina += kRegeneStamina;
		m_staminaRate = m_stamina / kMaxStamina;
		m_staminaRate = std::clamp(m_staminaRate, 0.0f, 1.0f);
	}

	// 左スティックの入力があれば歩き状態に移行する
	if (input.IsPress("LEFT") || input.IsPress("RIGHT") ||
		input.IsPress("UP") || input.IsPress("DOWN"))
	{
		ChangeState(PlayerState::Walk);
	}

	// Aボタンの入力があれば攻撃状態に移行する
	if (input.IsTrigger("A"))
	{
		ChangeState(PlayerState::Chop);
	}

	// Xボタンの入力があれば強攻撃状態に移行する
	if (input.IsTrigger("X"))
	{
		if (m_stamina >= kSpinStamina)
		{
			m_stamina -= kSpinStamina;
			m_staminaRate = m_stamina / kMaxStamina;
			m_staminaRate = std::clamp(m_staminaRate, 0.0f, 1.0f);
			ChangeState(PlayerState::Spin);
		}
	}

	// Yボタンの入力があれば射撃状態に移行する
	if (input.IsTrigger("Y"))
	{
		ChangeState(PlayerState::Shot);
	}

#ifdef _DEBUG
	// 左スティックを押し込んだ時に必殺技ゲージが最大でなければ最大にする
	if (input.IsPress("LPush"))
	{
		if (m_specialGauge < kMaxSpecialGauge)
		{
			m_specialGauge += kMaxSpecialGauge;
			m_specialGaugeRate = m_specialGauge / kMaxSpecialGauge;
			m_specialGaugeRate = std::clamp(m_specialGaugeRate, 0.0f, 1.0f);
		}
	}
#endif

	// LBボタンの入力があれば必殺技状態に移行する
	if (input.IsTrigger("Special"))
	{
		if (m_specialGauge < kMaxSpecialGauge)
		{
#ifdef _DEBUG
			printfDx("必殺技打てないよ！\n");
#endif
		}
		if (m_specialGauge >= kMaxSpecialGauge)
		{
#ifdef _DEBUG
			printfDx("必殺技発動！\n");
#endif
			ChangeState(PlayerState::Special);
		}
	}
}

void Player::WalkUpdate()
{
	auto& input = Input::GetInstance();
	// プレイヤー自身の当たり判定をオンにする
	SetActive(true);

	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Walk), false);

	// スタミナが最大じゃないときは徐々に回復する
	if (m_stamina < kMaxStamina)
	{
		m_stamina += kRegeneStamina;
		m_staminaRate = m_stamina / kMaxStamina;
		m_staminaRate = std::clamp(m_staminaRate, 0.0f, 1.0f);
	}

	Vector3 dir = { 0.0f, 0.0f,0.0f };
	// 左スティックで移動
	// 左入力
	if (input.IsPress("LEFT"))
	{
		dir.x = -kWalkSpeed;
	}
	// 右入力
	else if (input.IsPress("RIGHT"))
	{
		dir.x = kWalkSpeed;
	}
	// 横方向の入力なし
	else
	{
		dir.x = 0.0f;
	}
	// 上入力
	if (input.IsPress("UP"))
	{
		dir.z = kWalkSpeed;
	}
	// 下入力
	else if (input.IsPress("DOWN"))
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

	// 進行方向にモデルを回転させる
	Vector3 velocity = m_rigidbody.GetVelo();
	if (velocity.x != 0.0f || velocity.z != 0.0f)
	{
		Vector3 axis(0.0f, 1.0f, 0.0f);
		float angle = std::atan2(velocity.x, -velocity.z);

		Quaternion targetRot;
		targetRot.AngleAxis(angle, axis);

		m_currentRot = Quaternion::Slerp(m_currentRot, targetRot, kLerpT);

		float angleY = m_currentRot.ToEulerY();
		MV1SetRotationXYZ(m_charModel, VGet(0.0f, -angleY, 0.0f));
	}

	// 左スティックの入力がない場合待機状態に移行
	if (m_rigidbody.GetVelo().x == 0.0f && m_rigidbody.GetVelo().z == 0.0f)
	{
		ChangeState(PlayerState::Idle);
	}

	// RBの入力があればダッシュ状態に移行する
	if (input.IsTrigger("RB"))
	{
		if (m_stamina >= kRunSpeedDownStaminaRate)
		{
			ChangeState(PlayerState::NormalRun);
		}
		else
		{
			ChangeState(PlayerState::TiredRun);
		}
	}

	// Aボタンの入力があれば攻撃状態に移行する
	if (input.IsTrigger("A"))
	{
		ChangeState(PlayerState::Chop);
	}

	// Xボタンの入力があれば強攻撃状態に移行する
	if (input.IsTrigger("X"))
	{
		if (m_stamina >= kSpinStamina)
		{
			m_stamina -= kSpinStamina;
			m_staminaRate = m_stamina / kMaxStamina;
			m_staminaRate = std::clamp(m_staminaRate, 0.0f, 1.0f);
			ChangeState(PlayerState::Spin);
		}
	}

	// Yボタンの入力があれば射撃状態に移行する
	if (input.IsTrigger("Y"))
	{
		ChangeState(PlayerState::Shot);
	}

	// Bボタンの入力があれば回避状態に移行する
	if (input.IsTrigger("B"))
	{
		ChangeState(PlayerState::Dodge);
	}
}

void Player::RunUpdate()
{
	auto& input = Input::GetInstance();
	// プレイヤー自身の当たり判定をオンにする
	SetActive(true);
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::NormalRun), false);
	Vector3 dir = { 0.0f, 0.0f, 0.0f };

	m_stamina -= kRunStamina;
	m_staminaRate = m_stamina / kMaxStamina;
	m_staminaRate = std::clamp(m_staminaRate, 0.0f, 1.0f);

	// 左スティックで移動
	// 左入力
	if (input.IsPress("LEFT"))
	{
		dir.x = -kNormalRunSpeed;
	}
	// 右入力
	else if (input.IsPress("RIGHT"))
	{
		dir.x = kNormalRunSpeed;
	}
	// 横方向の入力なし
	else
	{
		dir.x = 0.0f;
	}
	// 上入力
	if (input.IsPress("UP"))
	{
		dir.z = kNormalRunSpeed;
	}
	// 下入力
	else if (input.IsPress("DOWN"))
	{
		dir.z = -kNormalRunSpeed;
	}
	// 縦方向の入力なし
	else
	{
		dir.z = 0.0f;
	}

	// ベクトルを正規化し移動速度をかけポジションに加算
	dir.Normalize();
	m_rigidbody.SetVelo(dir * kNormalRunSpeed);
	MV1SetPosition(m_charModel, m_rigidbody.GetPos().ToDxVECTOR());

	// 進行方向にモデルを回転させる
	Vector3 velocity = m_rigidbody.GetVelo();
	if (velocity.x != 0.0f || velocity.z != 0.0f)
	{
		Vector3 axis(0.0f, 1.0f, 0.0f);
		float angle = std::atan2(velocity.x, -velocity.z);

		Quaternion targetRot;
		targetRot.AngleAxis(angle, axis);

		m_currentRot = Quaternion::Slerp(m_currentRot, targetRot, kLerpT);

		float angleY = m_currentRot.ToEulerY();
		MV1SetRotationXYZ(m_charModel, VGet(0.0f, -angleY, 0.0f));
	}

	// 左スティックの入力がない場合待機状態に移行する
	if (m_rigidbody.GetVelo().x == 0.0f && m_rigidbody.GetVelo().z == 0.0f)
	{
		ChangeState(PlayerState::Idle);
	}

	// RBの入力があれば歩き状態に移行する
	if (input.IsTrigger("RB"))
	{
		ChangeState(PlayerState::Walk);
	}

	// スタミナが減ってきたら遅い走り状態に移行する
	if (m_stamina < kRunSpeedDownStaminaRate)
	{
		ChangeState(PlayerState::TiredRun);
	}
	// スタミナがなくなったら疲れ状態に移行する
	if (m_stamina <= 0)
	{
		ChangeState(PlayerState::Tired);
	}

	// Aボタンの入力があれば攻撃状態に移行する
	if (input.IsTrigger("A"))
	{
		ChangeState(PlayerState::Chop);
	}

	// Xボタンの入力があれば強攻撃状態に移行する
	if (input.IsTrigger("X"))
	{
		if (m_stamina >= kSpinStamina)
		{
			m_stamina -= kSpinStamina;
			ChangeState(PlayerState::Spin);
		}
	}

	// Yボタンの入力があれば射撃状態に移行する
	if (input.IsTrigger("Y"))
	{
		ChangeState(PlayerState::Shot);
	}

	// Bボタンの入力があれば回避状態に移行する
	if (input.IsTrigger("B"))
	{
		ChangeState(PlayerState::Dodge);
	}
}

void Player::TiredRunUpdate()
{
	auto& input = Input::GetInstance();
	// プレイヤー自身の当たり判定をオンにする
	SetActive(true);
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::NormalRun), false);
	Vector3 dir = { 0.0f, 0.0f, 0.0f };

	m_stamina -= kRunStamina;
	m_staminaRate = m_stamina / kMaxStamina;
	m_staminaRate = std::clamp(m_staminaRate, 0.0f, 1.0f);

	// 左スティックで移動
	// 左入力
	if (input.IsPress("LEFT"))
	{
		dir.x = -kTiredRunSpeed;
	}
	// 右入力
	else if (input.IsPress("RIGHT"))
	{
		dir.x = kTiredRunSpeed;
	}
	// 横方向の入力なし
	else
	{
		dir.x = 0.0f;
	}
	// 上入力
	if (input.IsPress("UP"))
	{
		dir.z = kTiredRunSpeed;
	}
	// 下入力
	else if (input.IsPress("DOWN"))
	{
		dir.z = -kTiredRunSpeed;
	}
	// 縦方向の入力なし
	else
	{
		dir.z = 0.0f;
	}

	// ベクトルを正規化し移動速度をかけポジションに加算
	dir.Normalize();
	m_rigidbody.SetVelo(dir * kTiredRunSpeed);
	MV1SetPosition(m_charModel, m_rigidbody.GetPos().ToDxVECTOR());

	// 進行方向にモデルを回転させる
	Vector3 velocity = m_rigidbody.GetVelo();
	if (velocity.x != 0.0f || velocity.z != 0.0f)
	{
		Vector3 axis(0.0f, 1.0f, 0.0f);
		float angle = std::atan2(velocity.x, -velocity.z);

		Quaternion targetRot;
		targetRot.AngleAxis(angle, axis);

		m_currentRot = Quaternion::Slerp(m_currentRot, targetRot, kLerpT);

		float angleY = m_currentRot.ToEulerY();
		MV1SetRotationXYZ(m_charModel, VGet(0.0f, -angleY, 0.0f));
	}

	// 左スティックの入力がない場合待機状態に移行する
	if (m_rigidbody.GetVelo().x == 0.0f && m_rigidbody.GetVelo().z == 0.0f)
	{
		ChangeState(PlayerState::Idle);
	}

	// RBの入力があれば歩き状態に移行する
	if (input.IsTrigger("RB"))
	{
		ChangeState(PlayerState::Walk);
	}

	// スタミナがなくなったら疲れ状態に移行する
	if (m_stamina <= 0)
	{
		ChangeState(PlayerState::Tired);
	}

	// Aボタンの入力があれば攻撃状態に移行する
	if (input.IsTrigger("A"))
	{
		ChangeState(PlayerState::Chop);
	}

	// Xボタンの入力があれば強攻撃状態に移行する
	if (input.IsTrigger("X"))
	{
		if (m_stamina >= kSpinStamina)
		{
			m_stamina -= kSpinStamina;
			ChangeState(PlayerState::Spin);
		}
	}

	// Bボタンの入力があれば回避状態に移行する
	if (input.IsTrigger("B"))
	{
		ChangeState(PlayerState::Dodge);
	}
}

void Player::TiredUpdate()
{
	auto& input = Input::GetInstance();
	// 当たり判定をオンにする
	SetActive(true);
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Tired), false);

	m_stamina += kTiredRegeneStamina;
	m_staminaRate = m_stamina / kMaxStamina;
	m_staminaRate = std::clamp(m_staminaRate, 0.0f, 1.0f);
	// スタミナが一定値まで回復したら待機状態に移行
	if (m_stamina >= kMaxStamina * 0.5f)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::ChopUpdate()
{
	auto& input = Input::GetInstance();
	// プレイヤー自身の当たり判定をオンにする
	SetActive(true);

	++m_attackFrame;
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Chop), false);

	// 一番近い敵の方向に回転
	RotateToNearestEnemy(kAttackOffsetRadius);

	// 攻撃開始から一定フレームの間前進
	if (m_attackFrame <= 15.0f)
	{
		// 攻撃中に前進する処理（現在の回転方向に進む）
		VECTOR rotVec = MV1GetRotationXYZ(m_charModel);
		float angleY = -rotVec.y; // モデル回転と一致させるため-が必要
		Vector3 forward(std::sin(angleY), 0.0f, -std::cos(angleY));
		forward.Normalize();
		m_rigidbody.SetVelo(forward * kAttackMoveSpeed); // 攻撃中の速度（定数）

		MV1SetPosition(m_charModel, m_rigidbody.GetPos().ToDxVECTOR());
	}

	if (m_attackFrame > 16.0f && m_attackFrame < 32.0f)
	{
		// 1ボタンの入力があれば攻撃状態に移行する
		if (input.IsTrigger("A"))
		{
			m_isCombo = true;
		}
	}

	// アニメーションが終了時
	if (m_anim.GetNextAnim().isEnd)
	{
		// 1ボタンの入力があれば攻撃状態に移行する
		if (m_isCombo)
		{
			ChangeState(PlayerState::Slice);
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
	auto& input = Input::GetInstance();
	// プレイヤー自身の当たり判定をオンにする
	SetActive(true);

	++m_attackFrame;
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Slice), false);

	// 一番近い敵の方向に回転
	RotateToNearestEnemy(kAttackOffsetRadius);

	// 攻撃開始から一定フレームの間前進
	if (m_attackFrame <= 15.0f)
	{
		// 攻撃中に前進する処理（現在の回転方向に進む）
		VECTOR rotVec = MV1GetRotationXYZ(m_charModel);
		float angleY = -rotVec.y; // モデル回転と一致させるため-が必要
		Vector3 forward(std::sin(angleY), 0.0f, -std::cos(angleY));
		forward.Normalize();
		m_rigidbody.SetVelo(forward * kAttackMoveSpeed); // 攻撃中の速度（定数）

		MV1SetPosition(m_charModel, m_rigidbody.GetPos().ToDxVECTOR());
	}

	if (m_attackFrame > 15.0f && m_attackFrame < 30.0f)
	{
		// 1ボタンの入力があれば攻撃状態に移行する
		if (input.IsTrigger("A"))
		{
			m_isCombo = true;
		}
	}

	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		// 1ボタンの入力があれば攻撃状態に移行する
		if (m_isCombo)
		{
			ChangeState(PlayerState::Stab);
		}
		else
		{
			ChangeState(PlayerState::Idle);
		}
	}
}

void Player::StabUpdate()
{
	// プレイヤー自身の当たり判定をオンにする
	SetActive(true);

	++m_attackFrame;
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Stab), false);

	// 一番近い敵の方向に回転
	RotateToNearestEnemy(kAttackOffsetRadius);

	// 攻撃開始から一定フレームの間前進
	if (m_attackFrame <= 15.0f)
	{
		// 攻撃中に前進する処理（現在の回転方向に進む）
		VECTOR rotVec = MV1GetRotationXYZ(m_charModel);
		float angleY = -rotVec.y; // モデル回転と一致させるため-が必要
		Vector3 forward(std::sin(angleY), 0.0f, -std::cos(angleY));
		forward.Normalize();
		m_rigidbody.SetVelo(forward * kAttackMoveSpeed); // 攻撃中の速度（定数）

		MV1SetPosition(m_charModel, m_rigidbody.GetPos().ToDxVECTOR());
	}

	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::SpinUpdate()
{
	// プレイヤー自身の当たり判定をオンにする
	SetActive(true);

	++m_attackFrame;
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Spin), false);

	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::ShotUpdate()
{
	// 当たり判定をオンにする
	SetActive(true);
	++m_attackFrame;
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Shot), false);

	if (m_attackFrame == kShotTiming)
	{
		// 弾を生成
		Vector3 myPos = m_rigidbody.GetPos();
		myPos.y += 50.0f;

		auto bullet = std::make_shared<PlayerBullet>();
		bullet->Init(myPos, m_forward);
		m_bullets.push_back(bullet);
	}

	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::SpecialUpdate()
{
	++m_attackFrame;
	SetActive(false);
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Special), true);

	MV1SetPosition(m_charModel, m_rigidbody.GetPos().ToDxVECTOR());

	/*PlayEffekseer3DEffect(m_specialEffect);
	SetPosPlayingEffekseer3DEffect(m_specialEffect,
		m_rigidbody.GetPos().x, m_rigidbody.GetPos().y, m_rigidbody.GetPos().z);*/

	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		m_specialGauge = 0;
		m_specialGaugeRate = m_specialGauge / kMaxSpecialGauge;
		m_specialGaugeRate = std::clamp(m_specialGaugeRate, 0.0f, 1.0f);
		ChangeState(PlayerState::Idle);
	}
}

void Player::DodgeUpdate()
{
	SetActive(true);
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Dodge), false);

	// m_forward に基づいて移動ベクトルを設定（前方向へ）
	Vector3 dodgeDir = m_forward;
	dodgeDir.Normalize();

	m_rigidbody.SetVelo(dodgeDir * kDodgeSpped);

	// 現在位置にモデルを反映
	MV1SetPosition(m_charModel, m_rigidbody.GetPos().ToDxVECTOR());

	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::HitUpdate()
{
	SetActive(false);
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Hit), false);

	++m_blinkFrame;

	MV1SetPosition(m_charModel, m_rigidbody.GetPos().ToDxVECTOR());
	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::DeadUpdate()
{
	SetActive(false);
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Dead), false);

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

void Player::RotateToNearestEnemy(float radius)
{
	// 攻撃開始から一定フレームで敵の方向に回転補間を始める
	if (m_attackFrame <= 7.0f)
	{
		// プレイヤーから一定範囲内の敵で一番近い敵を探す
		auto nearestEnemy = FindNearestEnemy(kAttackOffsetRadius);
		if (nearestEnemy)
		{
			// 一番近い敵へのベクトル
			Vector3 toEnemy = nearestEnemy->GetPos() - m_rigidbody.GetPos();
			toEnemy.y = 0.0f;

			// 一番近い敵の方向に回転
			static Quaternion currentRot;
			if (toEnemy.x != 0.0f || toEnemy.z != 0.0f)
			{
				// 敵方向への目標回転
				float targetAngle = std::atan2(toEnemy.x, -toEnemy.z);
				Vector3 axis(0.0f, 1.0f, 0.0f);
				Quaternion targetRot;
				targetRot.AngleAxis(targetAngle, axis);

				currentRot = Quaternion::Slerp(currentRot, targetRot, kLerpT);

				// 回転を適用
				float angleY = currentRot.ToEulerY();
				MV1SetRotationXYZ(m_charModel, VGet(0.0f, -angleY, 0.0f));
			}
		}
	}
}

std::shared_ptr<EnemyBase> Player::FindNearestEnemy(float radius)
{
	std::shared_ptr<EnemyBase> nearest = nullptr;
	float minDist = radius;

	auto enemies = GameObjectManager::Instance().GetEnemies();
	Vector3 myPos = m_rigidbody.GetPos();

	for (auto& enemy : enemies)
	{
		if (enemy->IsDead()) continue;
		float dist = (enemy->GetPos() - myPos).Length();
		if (dist < minDist)
		{
			minDist = dist;
			nearest = enemy;
		}
	}
	return nearest;
}

