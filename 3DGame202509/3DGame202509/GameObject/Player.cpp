#include "Player.h"

#include "DxLib.h"
#include <cassert>

namespace
{
	// アニメーション名
	// 待機
	const char* kIdleAnimName         = "Idle";
	// 歩き
	const char* kWalkAnimName         = "Walking_B";
	// 走り
	const char* kRunAnimName          = "Running_A";
	// 攻撃
	const char* kChopAnimName	      = "1H_Melee_Attack_Chop";
	const char* kSliceAnimName        = "1H_Melee_Attack_Slice_Diagonal";
	const char* kStabAnimName         = "1H_Melee_Attack_Stab";
	const char* kSpinAnimName         = "2H_Melee_Attack_Spin";
	// 回避
	const char* kDodgeAnimName		  = "Dodge_Forward";
	// 死亡
	const char* kDeadAnimName         = "Death_B";

	// HPの初期値
	constexpr int kHp = 100;
	// 移動速度
	constexpr float kWalkSpeed = 1.5f;
	constexpr float kRunSpeed = 4.5f;
	// プレイヤーのモデルの拡大値
	constexpr float kPlayerModelScale = 45.0f;
}

Player::Player() :
	m_model(-1),
	m_pos(0.0f, 0.0f, 0.0f),
	m_vec(0.0f, 0.0f, 0.0f),
	m_hp(kHp),
	m_isWalk(false),
	m_isRun(false),
	m_isChop(false),
	m_isSlice(false),
	m_isStab(false),
	m_isSpin(false),
	m_isDodge(false),
	m_isDead(false),
	m_frameCount(0.0f),
	m_walkFrameCount(0.0f),
	m_blendRate(0.0f),
	m_state(&Player::IdleInit)
{
	m_model = MV1LoadModel("Data/Model/Player/Player.mv1");
	assert(m_model >= 0);
	MV1SetScale(m_model, VGet(kPlayerModelScale, kPlayerModelScale, kPlayerModelScale));

	AttachAnim(m_nextAnim, kIdleAnimName, true);
	m_blendRate = 1.0f;
}

Player::~Player()
{
	MV1DeleteModel(m_model);
}

void Player::Update(Input& input)
{	
	// アニメーションの更新
	UpdateAnim(m_prevAnim);
	UpdateAnim(m_nextAnim);
	UpdateAnimBlend();
	(this->*m_state)(input);
}

void Player::Draw()
{
#if _DEBUG
	DrawSphere3D(VGet(m_pos.x, m_pos.y, m_pos.z), 10.0f, 16, 0x0000ff, 0x0000ff, true);
#endif

	MV1DrawModel(m_model);
}

void Player::OnDamage()
{
	m_hp--;
}

void Player::IdleInit(Input& input)
{
	// 待機アニメーションに変更
	ChangeAnim(kIdleAnimName, true);
	m_state = &Player::IdleUpdate;
	(this->*m_state)(input);
}

void Player::IdleUpdate(Input& input)
{
	// 左スティックの入力があれば走り状態に移行するためのフラグを立てる
	if (input.IsPress("LEFT") || input.IsPress("RIGHT") ||
		input.IsPress("UP")   || input.IsPress("DOWN"))
	{
		m_isWalk = true;
	}

	// Aボタンの入力があれば攻撃状態に移行するためのフラグを立てる
	if (input.IsTrigger("A"))
	{
		m_isChop = true;
	}

	// Xボタンの入力があれば強攻撃状態に移行するためのフラグを立てる
	if (input.IsTrigger("X"))
	{
		m_isSpin = true;
	}

	// 走り状態に移行
	if (m_isWalk)
	{
		m_state = &Player::WalkInit;
	}
	// 攻撃状態に移行
	if (m_isChop)
	{
		m_state = &Player::ChopInit;
	}
	// 強攻撃状態に移行
	if (m_isSpin)
	{
		m_state = &Player::SpinInit;
	}
#if _DEBUG
	// 死亡状態に移行
	if (input.IsTrigger("LB"))
	{
		m_state = &Player::DeadInit;
	}
#endif
}

void Player::WalkInit(Input& input)
{
	// 待機アニメーションに変更
	ChangeAnim(kWalkAnimName, true);
	m_state = &Player::WalkUpdate;
	m_walkFrameCount = 0.0f;
}

void Player::WalkUpdate(Input& input)
{
	// 左スティックで移動
	// 左入力
	if (input.IsPress("LEFT"))
	{
		m_vec.x = -kWalkSpeed;
	}
	// 右入力
	else if (input.IsPress("RIGHT"))
	{
		m_vec.x = kWalkSpeed;
	}
	// 横方向の入力なし
	else
	{
		m_vec.x = 0.0f;
	}
	// 上入力
	if (input.IsPress("UP"))
	{
		m_vec.z = kWalkSpeed;
	}
	// 下入力
	else if (input.IsPress("DOWN"))
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

	// 一定フレーム経過したら走り状態に移行するためのフラグを立てる
	++m_walkFrameCount;
	if (m_walkFrameCount > 60.0f)
	{
		m_isRun = true;
	}

	// Aボタンの入力があれば攻撃状態に移行するためのフラグを立てる
	if (input.IsTrigger("A"))
	{
		m_isChop = true;
	}

	// Xボタンの入力があれば今日攻撃状態に移行するためのフラグを立てる
	if (input.IsTrigger("X"))
	{
		m_isSpin = true;
	}

	// Bボタンの入力があれば回避状態に移行するためのフラグを立てる
	if (input.IsTrigger("B"))
	{
		m_isDodge = true;
	}

	// 左スティックの入力がない場合待機状態に移行
	if (m_vec.x == 0.0f && m_vec.z == 0.0f)
	{
		m_state  = &Player::IdleInit;
		m_isWalk = false;
	}

	// 走り状態に移行
	if (m_isRun)
	{
		m_state  = &Player::RunInit;
		m_isWalk = false;
		m_walkFrameCount = 0.0f;
	}

	// 攻撃状態に移行
	if (m_isChop)
	{
		m_state  = &Player::ChopInit;
		m_isWalk = false;
	}

	// 強攻撃状態に移行
	if (m_isSpin)
	{
		m_state  = &Player::SpinInit;
		m_isWalk = false;
	}
	// 回避状態に移行
	if (m_isDodge)
	{
		m_state  = &Player::DodgeInit;
		m_isWalk = false;
	}
}

void Player::RunInit(Input& input)
{
	// 走りアニメーションに変更
	ChangeAnim(kRunAnimName, true);
	m_state = &Player::RunUpdate;
	(this->*m_state)(input);
}

void Player::RunUpdate(Input& input)
{
	// 左スティックで移動
	// 左入力
	if (input.IsPress("LEFT"))
	{
		m_vec.x = -kRunSpeed;
	}
	// 右入力
	else if (input.IsPress("RIGHT"))
	{
		m_vec.x = kRunSpeed;
	}
	// 横方向の入力なし
	else
	{
		m_vec.x = 0.0f;
	}
	// 上入力
	if (input.IsPress("UP"))
	{
		m_vec.z = kRunSpeed;
	}
	// 下入力
	else if (input.IsPress("DOWN"))
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

	// Aボタンの入力があれば攻撃状態に移行するためのフラグを立てる
	if (input.IsTrigger("A"))
	{
		m_isChop = true;
	}

	// Xボタンの入力があれば強攻撃状態に移行するためのフラグを立てる
	if (input.IsTrigger("X"))
	{
		m_isSpin = true;
	}

	// Bボタンの入力があれば回避状態に移行するためのフラグを立てる
	if (input.IsTrigger("B"))
	{
		m_isDodge = true;
	}
	
	// 左スティックの入力がない場合待機状態に移行
	if (m_vec.x == 0.0f && m_vec.z == 0.0f)
	{
		m_state = &Player::IdleInit;
		m_isRun = false;
	}

	// 攻撃状態に移行
	if (m_isChop)
	{
		m_state = &Player::ChopInit;
		m_isRun = false;
	}

	// 強攻撃状態に移行
	if (m_isSpin)
	{
		m_state = &Player::SpinInit;
		m_isRun = false;
	}

	// 回避状態に移行
	if (m_isDodge)
	{
		m_state = &Player::DodgeInit;
		m_isRun = false;
	}
}

void Player::ChopInit(Input& input)
{
	// 攻撃アニメーションに変更
	ChangeAnim(kChopAnimName, false);
	m_state = &Player::ChopUpdate;
	(this->*m_state)(input);
}

void Player::ChopUpdate(Input& input)
{
	// 1ボタンの入力があれば攻撃状態に移行するためのフラグを立てる
	if (input.IsTrigger("A"))
	{
		m_isSlice = true;
	}

	// アニメーションが終了したら待機状態に戻る
	if (m_nextAnim.isEnd)
	{
		if (m_isSlice)
		{
			m_state = &Player::SliceInit;
			m_isChop = false;
		}
		else
		{
			m_state = &Player::IdleInit;
			m_isChop = false;
		}
	}
}

void Player::SliceInit(Input& input)
{
	// 攻撃アニメーションに変更
	ChangeAnim(kSliceAnimName, false);
	m_state = &Player::SliceUpdate;
	(this->*m_state)(input);
}

void Player::SliceUpdate(Input& input)
{
	// 1ボタンの入力があれば攻撃状態に移行するためのフラグを立てる
	if (input.IsTrigger("A"))
	{
		m_isStab = true;
	}

	// アニメーションが終了したら待機状態に戻る
	if (m_nextAnim.isEnd)
	{
		if (m_isStab)
		{
			m_state = &Player::StabInit;
			m_isSlice = false;
		}
		else
		{
			m_state = &Player::IdleInit;
			m_isSlice = false;
		}
	}
}

void Player::StabInit(Input& input)
{
	// 攻撃アニメーションに変更
	ChangeAnim(kStabAnimName, false);
	m_state = &Player::StabUpdate;
	(this->*m_state)(input);
}

void Player::StabUpdate(Input& input)
{
	// 1ボタンの入力があれば攻撃状態に移行するためのフラグを立てる
	if (input.IsTrigger("A"))
	{
		m_isChop = true;
	}

	// アニメーションが終了したら待機状態に戻る
	if (m_nextAnim.isEnd)
	{
		if (m_isChop)
		{
			m_state = &Player::ChopInit;
			m_isStab = false;
		}
		else
		{
			m_state = &Player::IdleInit;
			m_isStab = false;
		}
	}
}

void Player::SpinInit(Input& input)
{
	// 強攻撃アニメーションに変更
	ChangeAnim(kSpinAnimName, false);
	m_state = &Player::SpinUpdate;
}

void Player::SpinUpdate(Input& input)
{
	if (m_nextAnim.isEnd)
	{
		m_state = &Player::IdleInit;
		m_isSpin = false;
	}
}

void Player::DodgeInit(Input& input)
{
	// 回避アニメーションに変更
	ChangeAnim(kDodgeAnimName, false);
	m_state = &Player::DodgeUpdate;
	(this->*m_state)(input);
}

void Player::DodgeUpdate(Input& input)
{
	// アニメーションが終了したら待機状態に戻る
	if (m_nextAnim.isEnd)
	{
		m_state = &Player::IdleInit;
		m_isDodge = false;
	}
}

void Player::DeadInit(Input& input)
{
	// 回避アニメーションに変更
	ChangeAnim(kDeadAnimName, false);
	m_state = &Player::DeadUpdate;
	(this->*m_state)(input);
}

void Player::DeadUpdate(Input& input)
{
	// アニメーションが終了したら待機状態に戻る
	if (m_nextAnim.isEnd)
	{
		m_isDead = true;
	}
}

void Player::AttachAnim(AnimData& data, const char* animName, bool isLoop)
{
	// アタッチしたいアニメの番号を取得
	int index = MV1GetAnimIndex(m_model, animName);
	// モデルにアニメーションをアタッチ
	data.attachNo = MV1AttachAnim(m_model, index, -1, false);
	// アニメカウンタ初期化
	data.frame = 0.0f;
	// アニメーションのループ設定
	data.isLoop = isLoop;
	// 非ループアニメの終了フラグを落としておく
	data.isEnd = false;
}

void Player::UpdateAnim(AnimData& data)
{
	// アニメーションがアタッチされていない場合は何もしない
	if (data.attachNo == -1)
	{
		return;
	}

	// アニメーションを進める
	data.frame += 0.5f;

	// 現在再生中のアニメーションの総時間を取得する
	const float totalTime = MV1GetAttachAnimTotalTime(m_model, data.attachNo);

	printf("frame %.0f\r", totalTime);

	// アニメーションの設定によってループさせるか最後のフレームで止めるかを判定
	if (data.isLoop)
	{
		// アニメーションをループさせる
		while (data.frame > totalTime)
		{
			data.frame -= totalTime;
		}
	}
	else
	{
		// 最後のフレームで停止させる
		if (data.frame > totalTime)
		{
			data.frame = totalTime;
			data.isEnd = true;
		}
	}

	// 進行させたアニメーションをモデルに適用する
	MV1SetAttachAnimTime(m_model, data.attachNo, data.frame);
}

void Player::UpdateAnimBlend()
{
	// 両方にアニメが設定されていない場合は変化させない
	if (m_nextAnim.attachNo == -1) return;
	if (m_prevAnim.attachNo == -1) return;

	// m_blendRateを0.0f -> 1.0fに変化させる
	m_blendRate += 1.0f / 8.0f;
	if (m_blendRate > 1.0f) m_blendRate = 1.0f;

	MV1SetAttachAnimBlendRate(m_model, m_prevAnim.attachNo, 1.0f - m_blendRate);
	MV1SetAttachAnimBlendRate(m_model, m_nextAnim.attachNo, m_blendRate);
}

void Player::ChangeAnim(const char* animName, bool isLoop)
{
	// ひとつ前のデータは今後管理できなくなるのであらかじめデタッチしておく
	MV1DetachAnim(m_model, m_prevAnim.attachNo);

	// 現在再生中のアニメーションは一つ古いデータ扱いになる
	m_prevAnim = m_nextAnim;

	// 新たにショットのアニメーションをアタッチする
	AttachAnim(m_nextAnim, animName, isLoop);

	// アニメのブレンド率は0.0>1.0に変化するので0.0で初期化
	m_blendRate = 0.0f;

	// m_blendRateをアニメーションに適用する
	MV1SetAttachAnimBlendRate(m_model, m_prevAnim.attachNo, 1.0f - m_blendRate);
	MV1SetAttachAnimBlendRate(m_model, m_nextAnim.attachNo, m_blendRate);
}

