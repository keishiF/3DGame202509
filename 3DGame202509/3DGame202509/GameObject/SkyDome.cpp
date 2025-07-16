#include "Camera.h"
#include "SkyDome.h"
#include <cassert>
#include <DxLib.h>

namespace
{
	constexpr float kModelScale = 5.0f;
	constexpr float kRotSpeed = 0.0005f;
}

SkyDome::SkyDome() :
	m_model(-1),
	m_rotY(0.0f)
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
	MV1SetRotationXYZ(m_model, VGet(0.0f, m_rotY, 0.0f));
}

void SkyDome::Update(std::shared_ptr<Camera> camera)
{
	m_pos = camera->GetPos(); 
	m_rotY += kRotSpeed;

	// 0?2πでループさせる（数値精度の問題を防ぐ）
	if (m_rotY > DX_PI_F * 2.0f)
	{
		m_rotY -= DX_PI_F * 2.0f;
	}

	MV1SetPosition(m_model, m_pos.ToDxVECTOR());
	MV1SetRotationXYZ(m_model, VGet(0.0f, m_rotY, 0.0f));
}

void SkyDome::Draw()
{
	MV1DrawModel(m_model);
}
