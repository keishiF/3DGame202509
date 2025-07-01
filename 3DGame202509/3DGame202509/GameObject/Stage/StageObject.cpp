#include "StageObject.h"
#include "DxLib.h"
#include <cassert>

void StageObject::Init(const std::string& modelPath, Vec3 pos, Vec3 rot, Vec3 scale)
{
	m_modelHandle = MV1LoadModel(modelPath.c_str());
	assert(m_modelHandle >= 0);

	MV1SetPosition(m_modelHandle, pos.ToDxVECTOR());
	MV1SetRotationXYZ(m_modelHandle, rot.ToDxVECTOR());
	MV1SetScale(m_modelHandle, VGet(scale.x, scale.y, scale.z));
}

void StageObject::Draw()
{
	if (m_modelHandle >= 0)
	{
		MV1DrawModel(m_modelHandle);
	}
}