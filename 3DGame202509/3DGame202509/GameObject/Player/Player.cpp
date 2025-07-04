#include "Animator.h"
#include "CapsuleColliderData.h"
#include "Physics.h"
#include "Player.h"
#include "PlayerRightWeapon.h"
#include "PlayerLeftWeapon.h"
#include "DxLib.h"
#include <algorithm>
#include <cassert>
#include <unordered_map>

namespace
{
	// HPの初期値
	constexpr int kHp = 100;
	// 移動速度
	constexpr float kWalkSpeed = 1.5f;
	constexpr float kRunSpeed = 5.0f;
	constexpr float kAttackMoveSpeed = 0.75f;
	// プレイヤーのモデルの拡大値
	constexpr float kModelScale = 45.0f;
	// プレイヤーの当たり判定
	constexpr float kRadius = 15.0f;
	constexpr float kColScale = 90.0f;
	constexpr float kAttackOffsetRadius = 100.0f;
	// プレイヤーの最大スタミナ
	constexpr float kMaxStamina = 100.0f;

	// アニメーション名
	// 待機
	const char* kIdleAnimName = "Idle";
	// 歩き
	const char* kWalkAnimName         = "Walking_B";
	// 走り
	const char* kRunAnimName          = "Running_A";
	// 攻撃
	const char* kChopAnimName	      = "1H_Melee_Attack_Chop";
	const char* kSliceAnimName        = "1H_Melee_Attack_Slice_Diagonal";
	const char* kStabAnimName         = "1H_Melee_Attack_Stab";
	const char* kSpinAnimName         = "2H_Melee_Attack_Spin";
	const char* kUltimateAnimName1    = "Dualwield_Melee_Attack_Chop";
	const char* kUltimateAnimName2    = "Dualwield_Melee_Attack_Slice";
	const char* kUltimateAnimName3    = "Dualwield_Melee_Attack_Stab";
	// 回避
	const char* kDodgeAnimName		  = "Dodge_Forward";
	// 被弾
	const char* kHitAnimName		  = "Hit_B";
	// 死亡
	const char* kDeadAnimName         = "Death_B";
	// アニメーションの再生速度
	constexpr float kAnimSpeed = 1.0f;
	constexpr float kIdleAnimSpeed = 0.5f;
	constexpr float kWalkAnimSpeed = 0.75f;
	constexpr float kChopAnimSpeed = 1.0f;
	constexpr float kDualAnimSpeed = 1.25f;

	const std::unordered_map<PlayerState, RightAttackTiming> kRightColTimingTable =
	{
		{PlayerState::Idle,		 { 0,  0}},
		{PlayerState::Walk,		 { 0,  0}},
		{PlayerState::Run,		 { 0,  0}},
		{PlayerState::Chop,		 {16, 28}},
		{PlayerState::Slice,	 {16, 28}},
		{PlayerState::Stab,		 {16, 28}},
		{PlayerState::Spin,      {16, 36}},
		{PlayerState::Special,   {16, 36}},
		{PlayerState::DualChop,  {16, 36}},
		{PlayerState::DualSlice, {16, 36}},
		{PlayerState::DualStab,  {16, 36}},
		{PlayerState::Dodge,	 { 0,  0}},
		{PlayerState::Hit,		 { 0,  0}},
		{PlayerState::Dead,		 { 0,  0}}
	};

	const std::unordered_map<PlayerState,LeftAttackTiming> kLeftColTimingTable =
	{
		{PlayerState::Idle,		 { 0,  0}},
		{PlayerState::Walk,		 { 0,  0}},
		{PlayerState::Run,		 { 0,  0}},
		{PlayerState::Chop,		 {16, 28}},
		{PlayerState::Slice,	 {16, 28}},
		{PlayerState::Stab,		 {16, 28}},
		{PlayerState::Spin,      {16, 36}},
		{PlayerState::Special,   {16, 36}},
		{PlayerState::DualChop,  {16, 36}},
		{PlayerState::DualSlice, {16, 36}},
		{PlayerState::DualStab,  {16, 36}},
		{PlayerState::Dodge,	 { 0,  0}},
		{PlayerState::Hit,		 { 0,  0}},
		{PlayerState::Dead,		 { 0,  0}}
	};
}

Player::Player() :
	m_charModel(-1),
	m_radius(kRadius),
	m_hp(kHp),
	m_isCombo(false),
	m_isDead(false),
	m_maxStamina(kMaxStamina),
	m_stamina(m_maxStamina),
	m_attackFrame(0.0f),
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

	m_stamina = m_maxStamina;

	//当たり判定
	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->m_startPos = pos;
	colData->m_radius = kRadius;

	m_charModel = MV1LoadModel("Data/Model/Player/Player.mv1");
	assert(m_charModel >= 0);
	MV1SetScale(m_charModel, VGet(kModelScale, kModelScale, kModelScale));
	MV1SetPosition(m_charModel, pos.ToDxVECTOR());

	m_anim.Init(m_charModel);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kIdleAnimName, kIdleAnimSpeed, true);

	m_rightWeapon = std::make_shared<PlayerRightWeapon>();
	m_rightWeapon->Init(physics);
}

void Player::Init(std::shared_ptr<Physics> physics, Vec3& pos, const Vec3& rot, const Vec3& scale)
{
	Collidable::Init(physics);

	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);

	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->m_startPos = pos;
	colData->m_radius = kRadius;

	m_charModel = MV1LoadModel("Data/Model/Player/Player.mv1");
	assert(m_charModel >= 0);

	MV1SetScale(m_charModel, VGet(scale.x * 50.0f, scale.y * 50.0f, scale.z * 50.0f));
	MV1SetPosition(m_charModel, pos.ToDxVECTOR());

	m_anim.Init(m_charModel);
	m_anim.AttachAnim(m_anim.GetNextAnim(), kIdleAnimName, kIdleAnimSpeed, true);

	m_rightWeapon = std::make_shared<PlayerRightWeapon>();
	m_rightWeapon->Init(physics);

	m_leftWeapon = std::make_shared<PlayerLeftWeapon>();
	m_leftWeapon->Init(physics);
}

void Player::Update()
{	
	if (m_isDead && m_charModel < 0)
	{
		return;
	}

	if (!m_isDead) 
	{
		// 徐々に回復
		m_stamina += 0.5f;
		m_stamina = std::clamp(m_stamina, 0.0f, m_maxStamina);
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
	Vec3 colPos = m_rigidbody.GetPos();
	colPos.y += kColScale;
	colData->m_startPos = colPos;
}

void Player::Draw()
{
	if (m_isDead && m_charModel < 0)
	{
		return;
	}

#if _DEBUG
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), m_radius, 16, 0x00ff00, 0x00ff00, false);
	DrawSphere3D(m_rigidbody.GetPos().ToDxVECTOR(), kAttackOffsetRadius, 16, 0xff0000, 0xff0000, false);
#endif
	MV1DrawModel(m_charModel);
	m_rightWeapon->Draw();

	if (m_state == PlayerState::Special)
	{
		m_leftWeapon->Draw();
	}

	const int gaugeWidth = 200;
	const int gaugeHeight = 20;
	const int gaugeX = 50;
	const int gaugeY = 50;

	// HPの割合
	float hpRate = static_cast<float>(m_hp) / kHp;
	hpRate = std::clamp(hpRate, 0.0f, 1.0f);

	// ゲージ色の決定
	int color;
	if (hpRate > 0.5f) 
	{
		color = 0x00ff00;
	}
	else if (hpRate > 0.25f) 
	{
		color = 0xffff00;
	}
	else 
	{
		color = 0xff0000;
	}

	// ゲージ背景（灰色）
	DrawBox(gaugeX, gaugeY,
		gaugeX + gaugeWidth,
		gaugeY + gaugeHeight,
		0x808080, true);

	// 現在のHP分の長さのゲージ
	int hpBarWidth = static_cast<int>(gaugeWidth * hpRate);
	DrawBox(gaugeX, gaugeY,
		gaugeX + hpBarWidth,
		gaugeY + gaugeHeight,
		color, true);

	// 枠線（黒）
	DrawBox(gaugeX, gaugeY,
		gaugeX + gaugeWidth,
		gaugeY + gaugeHeight,
		0x000000, false);


	const int segments = 50; // 精度
	const float angleStep = DX_PI * 2.0f / segments;

	// スタミナの割合
	float rate = m_stamina / m_maxStamina;
	rate = std::clamp(rate, 0.0f, 1.0f);

	// プレイヤーの頭上に表示する位置
	VECTOR worldPos = MV1GetPosition(m_charModel);
	worldPos.x -= 50.0f;
	worldPos.y += 100.0f;

	// 画面座標へ変換（正しい使い方）
	VECTOR screenPos = ConvWorldPosToScreenPos(worldPos);

	const int centerX = static_cast<int>(screenPos.x);
	const int centerY = static_cast<int>(screenPos.y);
	const float radius = 30.0f;

	for (int i = 0; i < segments * rate; ++i)
	{
		float angle1 = i * angleStep;
		float angle2 = (i + 1) * angleStep;

		int x1 = static_cast<int>(centerX + std::cos(angle1) * radius);
		int y1 = static_cast<int>(centerY + std::sin(angle1) * radius);
		int x2 = static_cast<int>(centerX + std::cos(angle2) * radius);
		int y2 = static_cast<int>(centerY + std::sin(angle2) * radius);

		DrawLine(x1, y1, x2, y2, 0x00ffff);
	}

	// 輪郭
	DrawCircle(centerX, centerY, static_cast<int>(radius), 0xffffff, false);

}

void Player::OnDamage()
{
	m_hp -= 10;

	if (m_hp <= 0 && !m_isDead)
	{
		ChangeState(PlayerState::Dead);
	}
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
	m_isCombo = false;

	switch (m_state)
	{
	case PlayerState::Idle:
		m_anim.ChangeAnim(kIdleAnimName, kIdleAnimSpeed, true);
		break;
	case PlayerState::Walk:
		m_anim.ChangeAnim(kWalkAnimName, kWalkAnimSpeed, true);
		break;
	case PlayerState::Run:
		m_anim.ChangeAnim(kRunAnimName, kAnimSpeed, true);
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
	case PlayerState::Special:
		// アニメーション順を登録
		while (!m_ultimateAnimQueue.empty()) m_ultimateAnimQueue.pop(); // 念のためクリア
		m_ultimateAnimQueue.push(kUltimateAnimName1);
		m_ultimateAnimQueue.push(kUltimateAnimName2);
		m_ultimateAnimQueue.push(kUltimateAnimName3);
		// 最初のアニメーションを設定
		if (!m_ultimateAnimQueue.empty()) 
		{
			m_currentSpecialAnim = m_ultimateAnimQueue.front();
			m_ultimateAnimQueue.pop();
			m_anim.ChangeAnim(m_currentSpecialAnim.c_str(), kDualAnimSpeed, false);
		}
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
	SetActive(true);
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Idle));
	m_leftWeapon->Update(m_charModel, m_attackFrame, kLeftColTimingTable.at(PlayerState::Idle));

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

	// RBボタンの入力があれば必殺技状態に移行する
	if (Input::Instance().IsTrigger("RB"))
	{
		ChangeState(PlayerState::Special);
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
	SetActive(true);
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Walk));
	m_leftWeapon->Update(m_charModel, m_attackFrame, kLeftColTimingTable.at(PlayerState::Walk));

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

void Player::RunUpdate()
{
	SetActive(true);
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Run));
	m_leftWeapon->Update(m_charModel, m_attackFrame, kLeftColTimingTable.at(PlayerState::Run));

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
	SetActive(true);
	++m_attackFrame;
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Chop));

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
	SetActive(true);
	++m_attackFrame;
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Slice));

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
		}
		else
		{
			ChangeState(PlayerState::Idle);
		}
	}
}

void Player::StabUpdate()
{
	SetActive(true);
	++m_attackFrame;
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Stab));

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
		}
		else
		{
			ChangeState(PlayerState::Idle);
		}
	}
}

void Player::SpinUpdate()
{
	SetActive(true);
	++m_attackFrame;
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Spin));

	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::SpecialUpdate()
{
	SetActive(false);
	++m_attackFrame;

	if (m_currentSpecialAnim == kUltimateAnimName1)
	{
		m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::DualChop));
		m_leftWeapon->Update(m_charModel, m_attackFrame, kLeftColTimingTable.at(PlayerState::DualChop));
	}
	else if (m_currentSpecialAnim == kUltimateAnimName2)
	{
		m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::DualSlice));
		m_leftWeapon->Update(m_charModel, m_attackFrame, kLeftColTimingTable.at(PlayerState::DualSlice));
	}
	else if (m_currentSpecialAnim == kUltimateAnimName3)
	{
		m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::DualStab));
		m_leftWeapon->Update(m_charModel, m_attackFrame, kLeftColTimingTable.at(PlayerState::DualStab));
	}

	// アニメーションが終了したら次のアニメーションを再生
	if (m_anim.GetNextAnim().isEnd)
	{
		if (!m_ultimateAnimQueue.empty())
		{
			m_currentSpecialAnim = m_ultimateAnimQueue.front();
			m_ultimateAnimQueue.pop();
			m_anim.ChangeAnim(m_currentSpecialAnim.c_str(), kDualAnimSpeed, false);
			m_attackFrame = 0.0f;
		}
		else
		{
			ChangeState(PlayerState::Idle);
		}
	}
}

void Player::DodgeUpdate()
{
	SetActive(false);
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Dodge));
	m_leftWeapon->Update(m_charModel, m_attackFrame, kLeftColTimingTable.at(PlayerState::Dodge));

	if (m_attackFrame == 0.0f && m_stamina >= 20.0f)
	{
		m_stamina -= 20.0f;  // Dodgeで消費
	}

	// アニメーションが終了したら待機状態に戻る
	if (m_anim.GetNextAnim().isEnd)
	{
		ChangeState(PlayerState::Idle);
	}
}

void Player::HitUpdate()
{
	SetActive(false);
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Hit));
	m_leftWeapon->Update(m_charModel, m_attackFrame, kLeftColTimingTable.at(PlayerState::Hit));

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
	m_rightWeapon->Update(m_charModel, m_attackFrame, kRightColTimingTable.at(PlayerState::Dead));
	m_leftWeapon->Update(m_charModel, m_attackFrame, kLeftColTimingTable.at(PlayerState::Dead));

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

