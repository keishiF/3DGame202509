#pragma once
#include "ColliderData.h"

class SphereColliderData : public ColliderData
{
public:
	SphereColliderData();
	~SphereColliderData();
	float m_radius;
};

