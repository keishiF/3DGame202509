#include "PolygonColliderData.h"

PolygonColliderData::PolygonColliderData() :
	ColliderData(ColliderData::Kind::Polygon),
	m_modelHandle(-1),
	m_nearWallHitPos(),
	m_nearFloorHitPos(),
	m_hitDim()
{
}

PolygonColliderData::~PolygonColliderData()
{
}
