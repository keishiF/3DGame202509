#include "Camera.h"
#include "Player.h"
#include <cmath>
#include <algorithm>

namespace
{
	constexpr float kLerpRate = 0.05f;
	constexpr float kRotSpeed = 0.00005f;
}

Camera::Camera() :
	m_pos(0.0f, 300.0f, 300.0f),
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

void Camera::Update(std::shared_ptr<Player> player)
{
	Vec3 playerPos = player->GetPos();
	m_pos += playerPos;

	SetCameraPositionAndTarget_UpVecY(
		VGet(m_pos.x, m_pos.y, m_pos.z),
		VGet(m_lookAtPos.x, m_lookAtPos.y, m_lookAtPos.z));
}

void Camera::SetCamera(std::shared_ptr<Player> player)
{
	// プレイヤーの位置を取得し、カメラをプレイヤーの位置に合わせる
	Vec3 playerPos = player->GetPos();
	m_pos += playerPos;

	SetCameraPositionAndTarget_UpVecY(
		VGet(m_pos.x, m_pos.y, m_pos.z),
		VGet(m_lookAtPos.x, m_lookAtPos.y, m_lookAtPos.z));
}
