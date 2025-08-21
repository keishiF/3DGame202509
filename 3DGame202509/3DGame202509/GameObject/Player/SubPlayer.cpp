#include "CapsuleColliderData.h"
#include "Input.h"
#include "PlayerRightWeapon.h"
#include "SubPlayer.h"

namespace
{
	// HPの初期値、最大値
	constexpr float kHP = 20;
	// 攻撃力
	constexpr float kDefaultAtk = 1.0f;
	// スタミナの初期値、最大値
	constexpr float kStamina = 100.0f;
	// 必殺技ゲージの最大値
	constexpr float kMaxSpecialGauge = 100.0f;
	// 時間経過で増える必殺技ゲージの量
	constexpr float kTimeSpecialGauge = 2.0f;
	// 必殺技ゲージが増えるまでの時間
	constexpr float kSpecialGaugeTime = 60.0f;
	
	// モデルの拡大値
	constexpr float kModelScale = 70.0f;

	// 当たり判定
	// カプセルの半径
	constexpr float kCapsuleColRadius = 45.0f;
	// カプセルの長さ
	constexpr float kColScale = 140.0f;
	// 攻撃をある程度敵の方向に向かせれる範囲
	constexpr float kAttackOffsetRadius = 230.0f;

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
	// 通常速度
	constexpr float kDefaultAnimSpeed  = 1.0f;
	// 待機
	constexpr float kIdleAnimSpeed     = 0.5f;
	// 歩き
	constexpr float kWalkAnimSpeed     = 0.75f;
	// 遅い走り
	constexpr float kTiredRunAnimSpeed = 0.5f;
	// 必殺技
	constexpr float kSpecialAnimSpeed  = 0.5f;
}

SubPlayer::SubPlayer() :
	CharacterBase(ObjectTag::Player, ObjectPriority::High, ColliderData::Kind::Capsule)
{
}

SubPlayer::~SubPlayer()
{
}

void SubPlayer::Init(Vector3& pos, Vector3& rot, Vector3& scale)
{
	// Physicsに登録
	Collidable::Init();
	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);

	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->m_startPos = pos;
	colData->m_radius = kCapsuleColRadius;

	// モデルのロード
	m_model = MV1LoadModel("Data/Model/Player/Player.mv1");
	assert(m_model >= 0);
	// モデルのサイズ、位置
	VECTOR modelScale = VGet(scale.x * kModelScale, scale.x * kModelScale, scale.x * kModelScale);
	MV1SetScale(m_model, modelScale);
	MV1SetPosition(m_model, m_rigidbody.GetPos().ToDxVECTOR());

	// プレイヤーの初期状態を待機状態にする
	m_state = PlayerState::Idle;

	// 各ステータスの初期化
	m_hp = kHP;
	m_hpRate = m_hp / kHP;
	m_atk = kDefaultAtk;
	m_stamina = kStamina;
	m_staminaRate = m_stamina / kStamina;
	m_specialGauge = 0.0f;
	m_specialGaugeRate = m_specialGauge / kMaxSpecialGauge;

	// 持っている武器を生成
	m_rightWeapon = std::make_shared<PlayerRightWeapon>();
	m_rightWeapon->Init();

	// アニメーター
	m_anim.Init(m_model);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kIdleAnimName, kIdleAnimSpeed, true);
}

void SubPlayer::Draw()
{
	if (m_isDead && m_model < 0)
	{
		return;
	}

	// 点滅効果のための条件
	if ((m_blinkFrame / 3) % 2 == 0)
	{
#if _DEBUG
		DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), kCapsuleColRadius, 16, 0x00ff00, 0x00ff00, false);
		DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), kAttackOffsetRadius, 16, 0xff0000, 0xff0000, false);
#endif
		MV1DrawModel(m_model);
		m_rightWeapon->Draw();
	}
}

void SubPlayer::OnDamage(float atk)
{
	// ダメージを受ける
	m_hp -= 1.0f;
	// HP割合を計算する
	m_hpRate = m_hp / kHP;
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

void SubPlayer::ChangeState(PlayerState newState)
{
	// 現在の状態と次の状態が同じ場合return
	// Hitだけ例外処理
	if (m_state == newState && m_state != PlayerState::Hit) return;

	m_state = newState;

	// 状態遷移時に攻撃フレームなどを0にしてリセットする
	m_rigidbody.SetVelo({ 0.0f, 0.0f, 0.0f });
	m_atkFrame = 0.0f;
	m_blinkFrame = 0;
	m_isCombo = false;

	VECTOR rotVec = MV1GetRotationXYZ(m_model);
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
		m_anim.ChangeAnim(kRunAnimName, kDefaultAnimSpeed, true);
		break;
	case PlayerState::TiredRun:
		m_anim.ChangeAnim(kRunAnimName, kTiredRunAnimSpeed, true);
		break;
	case PlayerState::Tired:
		m_anim.ChangeAnim(kTiredAnimName, kDefaultAnimSpeed, true);
		break;
	case PlayerState::Chop:
		m_anim.ChangeAnim(kChopAnimName, kDefaultAnimSpeed, false);
		break;
	case PlayerState::Slice:
		m_anim.ChangeAnim(kSliceAnimName, kDefaultAnimSpeed, false);
		break;
	case PlayerState::Stab:
		m_anim.ChangeAnim(kStabAnimName, kDefaultAnimSpeed, false);
		break;
	case PlayerState::Spin:
		m_anim.ChangeAnim(kSpinAnimName, kDefaultAnimSpeed, false);
		break;
	case PlayerState::Shot:
		m_anim.ChangeAnim(kStabAnimName, kDefaultAnimSpeed, false);
		break;
	case PlayerState::Special:
		m_anim.ChangeAnim(kSpecialAnimName, kSpecialAnimSpeed, false);
		break;
	case PlayerState::Dodge:
		m_anim.ChangeAnim(kDodgeAnimName, kDefaultAnimSpeed, false);
		break;
	case PlayerState::Hit:
		m_anim.ChangeAnim(kHitAnimName, kDefaultAnimSpeed, false);
		break;
	case PlayerState::Dead:
		m_anim.ChangeAnim(kDeadAnimName, kDefaultAnimSpeed, false);
		break;
	}
}

void SubPlayer::Update()
{
	// 死んでいたらreturn
	if (m_isDead && m_model < 0)
	{
		return;
	}

	// 必殺技ゲージが最大でないとき
	if (m_specialGauge < kMaxSpecialGauge)
	{
		if (++m_frame >= kSpecialGaugeTime)
		{
			m_specialGauge += kTimeSpecialGauge;
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

	//当たり判定
	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	Vector3 colPos = m_rigidbody.GetPos();
	colPos.y += kColScale;
	colData->m_startPos = colPos;

	VECTOR rotVec = MV1GetRotationXYZ(m_model);
	float angleY = -rotVec.y;
	m_forward = Vector3(std::sin(angleY), 0.0f, -std::cos(angleY));
	m_forward.Normalize();
}

void SubPlayer::IdleUpdate()
{
}

void SubPlayer::WalkUpdate()
{
}

void SubPlayer::RunUpdate()
{
}

void SubPlayer::TiredRunUpdate()
{
}

void SubPlayer::TiredUpdate()
{
}

void SubPlayer::ChopUpdate()
{
}

void SubPlayer::SliceUpdate()
{
}

void SubPlayer::StabUpdate()
{
}

void SubPlayer::SpinUpdate()
{
}

void SubPlayer::ShotUpdate()
{
}

void SubPlayer::SpecialUpdate()
{
}

void SubPlayer::DodgeUpdate()
{
}

void SubPlayer::HitUpdate()
{
}

void SubPlayer::DeadUpdate()
{
}

void SubPlayer::RotateToNearestEnemy(float radius)
{
}

std::shared_ptr<EnemyBase> SubPlayer::FindNearestEnemy(float radius)
{
	return std::shared_ptr<EnemyBase>();
}
