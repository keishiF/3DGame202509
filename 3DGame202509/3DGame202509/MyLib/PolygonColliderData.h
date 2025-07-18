#pragma once
#include "ColliderData.h"
#include "Vec3.h"
#include <vector>

class PolygonColliderData : public ColliderData
{
public:
	PolygonColliderData();
	~PolygonColliderData();

	std::vector<Vec3> m_vertices;
	Vec3 m_normal;
};

