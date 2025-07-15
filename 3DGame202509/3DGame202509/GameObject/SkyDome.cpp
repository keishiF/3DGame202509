#include "Camera.h"
#include "SkyDome.h"
#include <cassert>
#include <DxLib.h>

namespace
{
	constexpr float kModelScale = 5.0f;
}

SkyDome::SkyDome() :
	m_model(-1)
{
}

SkyDome::~SkyDome()
{
}

void SkyDome::Init(std::shared_ptr<Camera> camera)
{
	m_pos = camera->GetPos();

	m_model = MV1LoadModel("Data/Model/Sky/Sky_Daylight01.mv1");
	assert(m_model >= 0);
	MV1SetPosition(m_model, m_pos.ToDxVECTOR());
	MV1SetScale(m_model, VGet(kModelScale, kModelScale, kModelScale));
}

void SkyDome::Update(std::shared_ptr<Camera> camera)
{
	m_pos = camera->GetPos();
	MV1SetPosition(m_model, m_pos.ToDxVECTOR());
}

void SkyDome::Draw()
{
	MV1DrawModel(m_model);
}
