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

void Floor::Init(const std::string& modelPath, Vector3 pos, Vector3 rot, Vector3 scale)
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
