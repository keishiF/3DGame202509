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

	Vec3 m_size;
};

