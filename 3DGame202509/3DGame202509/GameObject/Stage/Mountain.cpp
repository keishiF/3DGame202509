#include "Mountain.h"
#include "PolygonColliderData.h"
#include <cassert>
#include <DxLib.h>

Mountain::Mountain()
{
}

Mountain::~Mountain()
{
}

void Mountain::Init(const std::string& modelPath, Vec3 pos, Vec3 rot, Vec3 scale)
{
	Collidable::Init();
	m_rigidbody.Init();
	m_rigidbody.SetPos(pos);

	m_model= MV1LoadModel(modelPath.c_str());
	assert(m_model >= 0);

	MV1SetAmbColorScale(m_model, GetColorF(1.0f, 1.0f, 1.0f, 1.0f));
	MV1SetPosition(m_model, pos.ToDxVECTOR());
	MV1SetRotationXYZ(m_model, rot.ToDxVECTOR());
	MV1SetScale(m_model, VGet(scale.x * 0.01f, scale.y * 0.01f, scale.z * 0.01f));

	auto colData = std::dynamic_pointer_cast<PolygonColliderData>(m_colliderData);
	colData->m_modelHandle = m_model;
}

void Mountain::Draw()
{
	if (m_model >= 0)
	{
		MV1DrawModel(m_model);
	}
}
