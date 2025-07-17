#pragma once
#include "ColliderData.h"
#include "Vec3.h"

/// <summary>
/// 壁(ボックス)の当たり判定に必要なデータをまとめたクラス
/// </summary>
class BoxColliderData : public ColliderData
{
public:
	BoxColliderData();
	~BoxColliderData();

	Vec3 GetCenter() const { return m_center; }
	Vec3 GetSize() const { return m_size; }
	void SetCenter(const Vec3& center) { m_center = center; }
	void SetSize(const Vec3& size) { m_size = size; }

	Vec3 GetMin() const { return m_center - m_size * 0.5f; }
	Vec3 GetMax() const { return m_center + m_size * 0.5f; }

	Vec3 m_center;
	Vec3 m_size;
};

