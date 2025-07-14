#pragma once
#include "ColliderData.h"

/// <summary>
/// 球の当たり判定に必要なデータをまとめたクラス
/// </summary>
class SphereColliderData : public ColliderData
{
public:
	SphereColliderData();
	~SphereColliderData();
	// 球の半径
	float m_radius;
};

