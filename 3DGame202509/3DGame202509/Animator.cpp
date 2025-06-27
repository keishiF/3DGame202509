#include "Animator.h"
#include "DxLib.h"

Animator::Animator() :
	m_model(-1),
	m_blendRate(0.0f)
{
}

void Animator::Init(int model)
{
	m_model = model;
}

void Animator::AttachAnim(AnimData& data, const char* animName, float playSpeed, bool isLoop)
{
	// アタッチしたいアニメの番号を取得
	int index = MV1GetAnimIndex(m_model, animName);
	// モデルにアニメーションをアタッチ
	data.attachNo = MV1AttachAnim(m_model, index, -1, false);
	// アニメカウンタ初期化
	data.frame = 0.0f;
	// アニメーションの再生速度を設定
	data.playSpeed = playSpeed;
	// アニメーションのループ設定
	data.isLoop = isLoop;
	// 非ループアニメの終了フラグを落としておく
	data.isEnd = false;
}

void Animator::UpdateAnim(AnimData& data)
{
	// アニメーションがアタッチされていない場合は何もしない
	if (data.attachNo == -1)
	{
		return;
	}

	// アニメーションを進める
	data.frame += data.playSpeed;

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

void Animator::UpdateAnimBlend()
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

void Animator::ChangeAnim(const char* animName, float playSpeed, bool isLoop)
{
	// ひとつ前のデータは今後管理できなくなるのであらかじめデタッチしておく
	MV1DetachAnim(m_model, m_prevAnim.attachNo);

	// 現在再生中のアニメーションは一つ古いデータ扱いになる
	m_prevAnim = m_nextAnim;

	// 新たにショットのアニメーションをアタッチする
	AttachAnim(m_nextAnim, animName, playSpeed, isLoop);

	// アニメのブレンド率は0.0>1.0に変化するので0.0で初期化
	m_blendRate = 0.0f;

	// m_blendRateをアニメーションに適用する
	MV1SetAttachAnimBlendRate(m_model, m_prevAnim.attachNo, 1.0f - m_blendRate);
	MV1SetAttachAnimBlendRate(m_model, m_nextAnim.attachNo, m_blendRate);
}
