#pragma once
#include "ColliderData.h"
#include "Vec3.h"

/// <summary>
/// ボックスの当たり判定に必要なデータをまとめたクラス
/// </summary>
class BoxColliderData : public ColliderData
{
public:
	BoxColliderData();
	~BoxColliderData();

	Vec3 m_min;
	Vec3 m_max;
};

