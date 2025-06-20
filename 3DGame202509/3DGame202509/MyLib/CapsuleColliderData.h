#pragma once
#include "ColliderData.h"
#include "Vec3.h"

class CapsuleColliderData : public ColliderData
{
public:
	CapsuleColliderData();
	~CapsuleColliderData();
	Vec3 m_startPos;
	float m_radius;
	bool m_isStartPos;
};

