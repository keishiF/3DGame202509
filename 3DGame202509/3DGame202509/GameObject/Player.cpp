#include "Player.h"
#include <cassert>

namespace
{
	// アニメーション名
	// 待機
	const char* kIdleAnimName         = "Idle";
	// 走り
	const char* kRunAnimName          = "Running_A";
	// 攻撃
	const char* kAttackAnimName       = "1H_Melee_Attack_Slice_Diagonal";
	// 4方向回避
	const char* kLeftDodgeAnimName    = "Dodge_Left";
	const char* kRightDodgeAnimName   = "Dodge_Right";
	const char* kForwardDodgeAnimName = "Dodge_Forward";
	const char* kBackDodgeAnimName    = "Dodge_Backward";

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
	m_isAttack(false),
	m_isRun(false),
	m_frameCount(0.0f)
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
	UpdateAnim(m_prevAnim);
	UpdateAnim(m_nextAnim);
	UpdateAnimBlend();
	if (input.IsPress(PAD_INPUT_LEFT))
	{
		m_vec.x = -kSpeed;
	}
	else if (input.IsPress(PAD_INPUT_RIGHT))
	{
		m_vec.x = kSpeed;
	}
	else
	{
		m_vec.x = 0.0f;
	}

	if (input.IsPress(PAD_INPUT_UP))
	{
		m_vec.z = kSpeed;
	}
	else if (input.IsPress(PAD_INPUT_DOWN))
	{
		m_vec.z = -kSpeed;
	}
	else
	{
		m_vec.z = 0.0f;
	}

	// xまたはzが0.0fじゃないときにm_isRunをtrueにする
	m_isRun = (m_vec.x != 0.0f) || (m_vec.z != 0.0f);
	m_isAttack = input.IsTrigger(PAD_INPUT_1);

	// 走っているときに走りアニメーションに変更する
	if (m_isRun)
	{
		ChangeAnim(kRunAnimName, false);
	}
	// 攻撃しているときに攻撃アニメーションに変更する
	if (m_isAttack)
	{
		ChangeAnim(kAttackAnimName, false);
	}
	// 走りアニメーションが終わったら待機アニメーションに戻す
	if (m_nextAnim.isEnd)
	{
		ChangeAnim(kIdleAnimName, true);
	}

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

