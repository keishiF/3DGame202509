#include "CapsuleColliderData.h"

CapsuleColliderData::CapsuleColliderData() :
	ColliderData(ColliderData::Kind::Capsule),
	m_isStartPos(true)
{
}

CapsuleColliderData::~CapsuleColliderData()
{
}
