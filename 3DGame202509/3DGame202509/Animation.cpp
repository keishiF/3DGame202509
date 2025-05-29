#include "Animation.h"
#include "DxLib.h"

void Animation::Init(AnimationData prev, AnimationData next, int model, float blendRate)
{
	m_prev = prev;
	m_next = next;
	m_model = model;
	m_blendRate = blendRate;
}

void Animation::AttachAnim(AnimationData next, const char* animName, bool isLoop)
{
	// アタッチしたいアニメの番号を取得
	int index = MV1GetAnimIndex(m_model, animName);
	// モデルにアニメーションをアタッチ
	next.attachNo = MV1AttachAnim(m_model, index, -1, false);
	// アニメカウンタ初期化
	next.frame = 0.0f;
	// アニメーションのループ設定
	next.isLoop = isLoop;
	// 非ループアニメの終了フラグを落としておく
	next.isEnd = false;
}

void Animation::PrevUpdateAnim()
{
	// アニメーションがアタッチされていない場合は何もしない
	if (m_prev.attachNo == -1)
	{
		return;
	}

	// アニメーションを進める
	m_prev.frame += 0.5f;

	// 現在再生中のアニメーションの総時間を取得する
	const float totalTime = MV1GetAttachAnimTotalTime(m_model, m_prev.attachNo);

	// アニメーションの設定によってループさせるか最後のフレームで止めるかを判定
	if (m_prev.isLoop)
	{
		// アニメーションをループさせる
		while (m_prev.frame > totalTime)
		{
			m_prev.frame -= totalTime;
		}
	}
	else
	{
		// 最後のフレームで停止させる
		if (m_prev.frame > totalTime)
		{
			m_prev.frame = totalTime;
			m_prev.isEnd = true;
		}
	}

	// 進行させたアニメーションをモデルに適用する
	MV1SetAttachAnimTime(m_model, m_prev.attachNo, m_prev.frame);
}

void Animation::NextUpdateAnim()
{
	// アニメーションがアタッチされていない場合は何もしない
	if (m_next.attachNo == -1)
	{
		return;
	}

	// アニメーションを進める
	m_next.frame += 0.5f;

	// 現在再生中のアニメーションの総時間を取得する
	const float totalTime = MV1GetAttachAnimTotalTime(m_model, m_next.attachNo);

	// アニメーションの設定によってループさせるか最後のフレームで止めるかを判定
	if (m_next.isLoop)
	{
		// アニメーションをループさせる
		while (m_next.frame > totalTime)
		{
			m_next.frame -= totalTime;
		}
	}
	else
	{
		// 最後のフレームで停止させる
		if (m_next.frame > totalTime)
		{
			m_next.frame = totalTime;
			m_next.isEnd = true;
		}
	}

	// 進行させたアニメーションをモデルに適用する
	MV1SetAttachAnimTime(m_model, m_next.attachNo, m_next.frame);
}

void Animation::UpdateAnimBlend()
{
	// 両方にアニメが設定されていない場合は変化させない
	if (m_next.attachNo == -1) return;
	if (m_prev.attachNo == -1) return;

	// m_blendRateを0.0f -> 1.0fに変化させる
	m_blendRate += 1.0f / 8.0f;
	if (m_blendRate > 1.0f) m_blendRate = 1.0f;

	MV1SetAttachAnimBlendRate(m_model, m_prev.attachNo, 1.0f - m_blendRate);
	MV1SetAttachAnimBlendRate(m_model, m_next.attachNo, m_blendRate);
}

void Animation::ChangeAnim(const char* animName, bool isLoop)
{
	// ひとつ前のデータは今後管理できなくなるのであらかじめデタッチしておく
	MV1DetachAnim(m_model, m_prev.attachNo);

	// 現在再生中のアニメーションは一つ古いデータ扱いになる
	m_prev = m_next;

	// 新たにショットのアニメーションをアタッチする
	AttachAnim(m_next, animName, isLoop);

	// アニメのブレンド率は0.0>1.0に変化するので0.0で初期化
	m_blendRate = 0.0f;

	// m_blendRateをアニメーションに適用する
	MV1SetAttachAnimBlendRate(m_model, m_prev.attachNo, 1.0f - m_blendRate);
	MV1SetAttachAnimBlendRate(m_model, m_next.attachNo, m_blendRate);
}
