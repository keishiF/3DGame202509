#include "Floor.h"
#include "PolygonColliderData.h"
#include <cassert>
#include <DxLib.h>

Floor::Floor()
{

}

Floor::~Floor()
{
}

void Floor::Init(const std::string& modelPath, Vec3 pos, Vec3 rot, Vec3 scale)
{
	Collidable::Init();

	m_modelHandle = MV1LoadModel(modelPath.c_str());
	assert(m_modelHandle >= 0);

	MV1SetPosition(m_modelHandle, pos.ToDxVECTOR());
	MV1SetRotationXYZ(m_modelHandle, rot.ToDxVECTOR());
	MV1SetScale(m_modelHandle, VGet(scale.x, scale.y, scale.z));

	auto colData = std::dynamic_pointer_cast<PolygonColliderData>(m_colliderData);
	m_colliderData = colData;
}

void Floor::Draw()
{
	if (m_modelHandle >= 0)
	{
		MV1DrawModel(m_modelHandle);
	}
}
