#include "Player.h"

#include "DxLib.h"
#include <cassert>
#include "Animation.h"

namespace
{
	// アニメーション名
	// 待機
	const char* kIdleAnimName         = "Idle";
	// 走り
	const char* kRunAnimName          = "Running_A";
	// 攻撃
	const char* kChopAttackAnimName	  = "1H_Melee_Attack_Chop";
	const char* kSliceAttackAnimName  = "1H_Melee_Attack_Slice_Diagonal";
	const char* kStabAttackAnimName   = "1H_Melee_Attack_Stab";

	// 4方向回避
	const char* kLeftDodgeAnimName    = "Dodge_Left";     // 左回避
	const char* kRightDodgeAnimName   = "Dodge_Right";    // 右回避
	const char* kForwardDodgeAnimName = "Dodge_Forward";  // 前回避
	const char* kBackDodgeAnimName    = "Dodge_Backward"; // 後ろ回避

	// HPの初期値
	constexpr int kHp = 100;
	// 移動速度
	constexpr float kSpeed = 3.5f;
	// ジャンプ力
	constexpr float kJumpSpeed = 20.0f;
	// 重力
	constexpr float kGravity = 0.75f;
	// プレイヤーのモデルの拡大値
	constexpr float kPlayerModelScale = 45.0f;
}

Player::Player() :
	m_model(-1),
	m_blendRate(0.0f),
	m_pos(0.0f, 0.0f, 0.0f),
	m_vec(0.0f, 0.0f, 0.0f),
	m_hp(kHp),
	m_jumpSpeed(kJumpSpeed),
	m_gravity(kGravity),
	m_isChop(false),
	m_isSlice(false),
	m_isStab(false),
	m_isRun(false),
	m_frameCount(0.0f),
	m_state(&Player::IdleInit)
{
	m_model = MV1LoadModel("Data/Model/Player/Player1.mv1");
	assert(m_model >= 0);
	AttachAnim(m_nextAnim, kIdleAnimName, true);
	m_blendRate = 1.0f;
	MV1SetScale(m_model, VGet(kPlayerModelScale, kPlayerModelScale, kPlayerModelScale));
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
}

void Player::IdleUpdate(Input& input)
{
	// 左スティックの入力があれば走り状態に移行するためのフラグを立てる
	if (input.IsPress(PAD_INPUT_LEFT))
	{
		m_isRun = true;
	}
	if (input.IsPress(PAD_INPUT_RIGHT))
	{
		m_isRun = true;
	}
	if (input.IsPress(PAD_INPUT_UP))
	{
		m_isRun = true;
	}
	if (input.IsPress(PAD_INPUT_DOWN))
	{
		m_isRun = true;
	}

	// 1ボタンの入力があれば攻撃状態に移行するためのフラグを立てる
	if (input.IsTrigger(PAD_INPUT_1))
	{
		m_isChop = true;
	}

	// 走り状態に移行
	if (m_isRun)
	{
		m_state = &Player::RunInit;
	}
	// 攻撃状態に移行
	if (m_isChop)
	{
		m_isChop = false;
		m_state = &Player::ChopAttackInit;
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
	if (input.IsPress(PAD_INPUT_LEFT))
	{
		m_vec.x = -kSpeed;
	}
	// 右入力
	else if (input.IsPress(PAD_INPUT_RIGHT))
	{
		m_vec.x = kSpeed;
	}
	// 横方向の入力なし
	else
	{
		m_vec.x = 0.0f;
	}
	// 上入力
	if (input.IsPress(PAD_INPUT_UP))
	{
		m_vec.z = kSpeed;
	}
	// 下入力
	else if (input.IsPress(PAD_INPUT_DOWN))
	{
		m_vec.z = -kSpeed;
	}
	// 縦方向の入力なし
	else
	{
		m_vec.z = 0.0f;
	}

	// ベクトルを正規化し移動速度をかけポジションに加算
	m_vec.Normalize();
	m_vec *= kSpeed;
	m_pos += m_vec;
	MV1SetPosition(m_model, VGet(m_pos.x, m_pos.y, m_pos.z));

	// 進行方向が0でなければ回転
	if (m_vec.x != 0.0f || m_vec.z != 0.0f)
	{
		// atan2でY軸回転角を計算（Zが前、Xが右の座標系の場合）
		float angleY = std::atan2(m_vec.x, -m_vec.z);
		MV1SetRotationXYZ(m_model, VGet(0.0f, -angleY, 0.0f));
	}

	// 1ボタンの入力があれば攻撃状態に移行するためのフラグを立てる
	if (input.IsTrigger(PAD_INPUT_1))
	{
		m_isChop = true;
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
		m_isChop = false;
		m_state = &Player::ChopAttackInit;
	}

}

void Player::ChopAttackInit(Input& input)
{
	// 攻撃アニメーションに変更
	ChangeAnim(kChopAttackAnimName, false);
	m_state = &Player::ChopAttackUpdate;
	(this->*m_state)(input);
}

void Player::ChopAttackUpdate(Input& input)
{
	// 1ボタンの入力があれば攻撃状態に移行するためのフラグを立てる
	if (input.IsTrigger(PAD_INPUT_1))
	{
		m_isSlice = true;
	}

	// アニメーション終了時にフラグが立っていれば次の攻撃状態に移行
	if (m_nextAnim.isEnd && m_isSlice)
	{
		m_isSlice = false;
		m_state = &Player::SliceAttackInit;
	}

	// アニメーションが終了したら待機状態に戻る
	if (m_nextAnim.isEnd)
	{
		m_state = &Player::IdleInit;
	}
}

void Player::SliceAttackInit(Input& input)
{
	// 攻撃アニメーションに変更
	ChangeAnim(kSliceAttackAnimName, false);
	m_state = &Player::SliceAttackUpdate;
	(this->*m_state)(input);
}

void Player::SliceAttackUpdate(Input& input)
{
	// 1ボタンの入力があれば攻撃状態に移行するためのフラグを立てる
	if (input.IsTrigger(PAD_INPUT_1))
	{
		m_isStab = true;
	}

	// アニメーション終了時にフラグが立っていれば次の攻撃状態に移行
	if (m_nextAnim.isEnd && m_isStab)
	{
		m_isStab = false;
		m_state = &Player::StabAttackInit;
	}

	// アニメーションが終了したら待機状態に戻る
	if (m_nextAnim.isEnd)
	{
		m_state = &Player::IdleInit;
	}
}

void Player::StabAttackInit(Input& input)
{
	// 攻撃アニメーションに変更
	ChangeAnim(kStabAttackAnimName, false);
	m_state = &Player::StabAttackUpdate;
	(this->*m_state)(input);
}

void Player::StabAttackUpdate(Input& input)
{
	// 1ボタンの入力があれば攻撃状態に移行するためのフラグを立てる
	if (input.IsTrigger(PAD_INPUT_1))
	{
		m_isChop = true;
	}

	// アニメーション終了時にフラグが立っていれば次の攻撃状態に移行
	if (m_nextAnim.isEnd && m_isChop)
	{
		m_isChop = false;
		m_state = &Player::ChopAttackInit;
	}

	// アニメーションが終了したら待機状態に戻る
	if (m_nextAnim.isEnd)
	{
		m_state = &Player::IdleInit;
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

