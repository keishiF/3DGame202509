#include "Camera.h"
#include <cmath>
#include <algorithm>

namespace
{
	constexpr float kLerpRate = 0.05f;
	constexpr float kRotSpeed = 0.00005f;

	constexpr float kCameraOffsetX = 0.0f;
	constexpr float kCameraOffsetY = 1000.0f;
	constexpr float kCameraOffsetZ = 1000.0f;
}

Camera::Camera() :
	m_pos(0.0f, 100.0f, 500.0f),
	m_lookAtPos(0.0f, 0.0f, 0.0f),
	m_fov(DX_PI_F / 3.0f),
	m_cameraRotX(0.0f),
	m_cameraRotY(0.0f),
	m_isLockOn(false)
{
}

Camera::~Camera()
{
}

void Camera::Update()
{
	SetCameraPositionAndTarget_UpVecY(
		VGet(m_pos.x, m_pos.y, m_pos.z),
		VGet(m_lookAtPos.x, m_lookAtPos.y, m_lookAtPos.z));
}

void Camera::SetCamera()
{
	SetCameraPositionAndTarget_UpVecY(
		VGet(m_pos.x, m_pos.y, m_pos.z),
		VGet(m_lookAtPos.x, m_lookAtPos.y, m_lookAtPos.z));
}
