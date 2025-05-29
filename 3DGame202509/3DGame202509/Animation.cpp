#include "Animation.h"
#include "DxLib.h"

void Animation::AttachAnim(Animation& data, int model, const char* animName, bool isLoop)
{
	// アタッチしたいアニメの番号を取得
	int index = MV1GetAnimIndex(model, animName);
	// モデルにアニメーションをアタッチ
	data.m_attachNo = MV1AttachAnim(model, index, -1, false);
	// アニメカウンタ初期化
	data.m_frame = 0.0f;
	// アニメーションのループ設定
	data.m_isLoop = isLoop;
	// 非ループアニメの終了フラグを落としておく
	data.m_isEnd = false;
}

void Animation::UpdateAnim(Animation& data, int model)
{
	// アニメーションがアタッチされていない場合は何もしない
	if (data.m_attachNo == -1)
	{
		return;
	}

	// アニメーションを進める
	data.m_frame += 0.5f;

	// 現在再生中のアニメーションの総時間を取得する
	const float totalTime = MV1GetAttachAnimTotalTime(model, data.m_attachNo);

	// アニメーションの設定によってループさせるか最後のフレームで止めるかを判定
	if (data.m_isLoop)
	{
		// アニメーションをループさせる
		while (data.m_frame > totalTime)
		{
			data.m_frame -= totalTime;
		}
	}
	else
	{
		// 最後のフレームで停止させる
		if (data.m_frame > totalTime)
		{
			data.m_frame = totalTime;
			data.m_isEnd = true;
		}
	}

	// 進行させたアニメーションをモデルに適用する
	MV1SetAttachAnimTime(model, data.m_attachNo, data.m_frame);
}

void Animation::UpdateAnimBlend(Animation& prevAnim, Animation& nextAnim, int model)
{
	// 両方にアニメが設定されていない場合は変化させない
	if (nextAnim.m_attachNo == -1) return;
	if (prevAnim.m_attachNo == -1) return;

	// m_blendRateを0.0f -> 1.0fに変化させる
	m_blendRate += 1.0f / 8.0f;
	if (m_blendRate > 1.0f) m_blendRate = 1.0f;

	MV1SetAttachAnimBlendRate(model, prevAnim.m_attachNo, 1.0f - m_blendRate);
	MV1SetAttachAnimBlendRate(model, nextAnim.m_attachNo, m_blendRate);
}

void Animation::ChangeAnim(int model,const char* animName, bool isLoop)
{
	// ひとつ前のデータは今後管理できなくなるのであらかじめデタッチしておく
	MV1DetachAnim(model, prevAnim.attachNo);

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
