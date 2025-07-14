#pragma once
#include "ColliderData.h"
#include "Vec3.h"

/// <summary>
/// カプセルの当たり判定に必要なデータをまとめたクラス
/// </summary>
class CapsuleColliderData : public ColliderData
{
public:
	CapsuleColliderData();
	~CapsuleColliderData();
	// 始点
	Vec3 m_startPos;
	// 半径
	float m_radius;
	bool m_isStartPos;
};

