#pragma once
#include "Vector3.h"

// エフェクトクラス
class Effect
{
public:
	Effect();
	~Effect();

	void Init(const int handle, const Vector3& initPos);

	// 位置設定
	void SetPos(const Vector3& pos);
	// 回転
	void SetRotate(const Vector3& rad);
	// 拡縮
	void SetScale(const Vector3& scale);
	// 再生速度
	void SetPlaySpeed(const float speed);
	// 動的パラメータを設定する
	void SetDynamicParam(const float x, const float y, const float z, const float w);
	// 色
	void SetColor(const int r, const int g, const int b, const int a);

	const int  GetHandle() const { return m_handle; }
	const bool IsPlaying() const;
	// このエフェクトを削除
	void Kill();
protected:
	int m_handle;
};

