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

	//次の座標
	Vec3 GetNextStartPos(Vec3 vec) const { return m_startPos + vec; };

	// 始点
	Vec3 m_startPos;
	// 半径
	float m_radius;
	bool m_isStartPos;
};

