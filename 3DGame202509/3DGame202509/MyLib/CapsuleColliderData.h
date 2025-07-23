#pragma once
#include "ColliderData.h"
#include "Vector3.h"

/// <summary>
/// カプセルの当たり判定に必要なデータをまとめたクラス
/// </summary>
class CapsuleColliderData : public ColliderData
{
public:
	CapsuleColliderData();
	~CapsuleColliderData();

	//次の座標
	Vector3 GetNextStartPos(Vector3 vec) const { return m_startPos + vec; };

	// 始点
	Vector3 m_startPos;
	// 半径
	float m_radius;
	bool m_isStartPos;
};

