#pragma once
#include "ColliderData.h"
#include "Vec3.h"

/// <summary>
/// 平面の当たり判定に必要なデータをまとめたクラス
/// </summary>
class BoxColliderData : public ColliderData
{
public:
	BoxColliderData();
	~BoxColliderData();
	// 平面の一辺の半分
	Vec3 m_halfWidth;
};

