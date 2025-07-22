#include "Floor.h"
#include "PolygonColliderData.h"
#include <cassert>
#include <DxLib.h>

Floor::Floor()
{

}

Floor::~Floor()
{
	MV1DeleteModel(m_model);
}

void Floor::Init(const std::string& modelPath, Vec3 pos, Vec3 rot, Vec3 scale)
{
	m_model = MV1LoadModel(modelPath.c_str());
	assert(m_model>= 0);

	MV1SetPosition(m_model, pos.ToDxVECTOR());
	MV1SetRotationXYZ(m_model, rot.ToDxVECTOR());
	MV1SetScale(m_model, VGet(scale.x, scale.y, scale.z));
}

void Floor::Draw()
{
	if (m_model>= 0)
	{
		MV1DrawModel(m_model);
	}
}
