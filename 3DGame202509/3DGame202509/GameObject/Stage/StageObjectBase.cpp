#include "StageObjectBase.h"

StageObjectBase::StageObjectBase() :
	Collidable(ObjectTag::Stage, ObjectPriority::Static, ColliderData::Kind::Polygon),
	m_modelHandle()
{
}
