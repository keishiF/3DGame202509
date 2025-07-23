#include "Effect.h"
#include <EffekseerForDXLib.h>

Effect::Effect() :
	m_handle(-1)
{
}

Effect::~Effect()
{
	// このインスタンスが死んだとき、エフェクトも死ぬようにする
	Kill();
}

void Effect::Init(const int handle, const Vector3& initPos)
{
	m_handle = handle;

	SetPos(initPos);
}

void Effect::SetPos(const Vector3& pos)
{
	SetPosPlayingEffekseer3DEffect(m_handle, pos.x, pos.y, pos.z);
}

void Effect::SetRotate(const Vector3& rad)
{
	SetRotationPlayingEffekseer3DEffect(m_handle, rad.x, rad.y, rad.z);
}

void Effect::SetScale(const Vector3& scale)
{
	SetScalePlayingEffekseer3DEffect(m_handle, scale.x, scale.y, scale.z);
}

void Effect::SetPlaySpeed(const float speed)
{
	SetSpeedPlayingEffekseer3DEffect(m_handle, speed);
}

const bool Effect::IsPlaying() const
{
	// 再生中 = 0,停止中or再生終了 = -1
	return IsEffekseer3DEffectPlaying(m_handle) == 0;
}

void Effect::SetDynamicParam(const float x, const float y, const float z, const float w)
{
	SetDynamicInput3DEffect(m_handle, 0, x);
	SetDynamicInput3DEffect(m_handle, 1, y);
	SetDynamicInput3DEffect(m_handle, 2, z);
	SetDynamicInput3DEffect(m_handle, 3, w);
}

void Effect::SetColor(const int r, const int g, const int b, const int a)
{
	SetColorPlayingEffekseer3DEffect(m_handle, r, g, b, a);
}

void Effect::Kill()
{
	// とりあえず再生を止めればManagerが消してくれる
	// この仕様はのちに都合が悪くて変更するかも
	StopEffekseer3DEffect(m_handle);
}