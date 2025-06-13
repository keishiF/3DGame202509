#include "Camera.h"
#include "Player/Player.h"
#include <cmath>
#include <algorithm>

namespace
{
	constexpr float kLerpRate = 0.05f;

	constexpr float kCameraPosX = 0.0f;
	constexpr float kCameraPosY = 300.0f;
	constexpr float kCameraPosZ = -300.0f;
}

Camera::Camera() :
	m_pos(0.0f, 0.0f, 0.0f),
	m_lookAtPos(0.0f, 0.0f, 0.0f),
	m_fov(DX_PI_F / 3.0f),
	m_isLockOn(false)
{
}

Camera::~Camera()
{
}

void Camera::Update(std::shared_ptr<Player> player)
{
	// �v���C���[�̈ʒu���擾���A�J�������v���C���[�̈ʒu�ɍ��킹��
	Vec3 playerPos = player->GetPos();
	Vec3 cameraPos = { kCameraPosX, kCameraPosY, kCameraPosZ };
	m_pos = playerPos + cameraPos;

	// �J�����̒����_���v���C���[�̈ʒu�ɍ��킹��
	m_lookAtPos = { playerPos.x, playerPos.y + 150.0f, playerPos.z };

	SetCameraPositionAndTarget_UpVecY(
		VGet(m_pos.x, m_pos.y, m_pos.z),
		VGet(m_lookAtPos.x, m_lookAtPos.y, m_lookAtPos.z));
}

void Camera::SetCamera(std::shared_ptr<Player> player)
{
	// �v���C���[�̈ʒu���擾���A�J�������v���C���[�̈ʒu�ɍ��킹��
	Vec3 playerPos = player->GetPos();
	Vec3 cameraPos = { kCameraPosX, kCameraPosY, kCameraPosZ };
	m_pos = playerPos + cameraPos;

	// �J�����̒����_���v���C���[�̈ʒu�ɍ��킹��
	m_lookAtPos = { playerPos.x, playerPos.y + 150.0f, playerPos.z };

	SetCameraPositionAndTarget_UpVecY(
		VGet(m_pos.x, m_pos.y, m_pos.z),
		VGet(m_lookAtPos.x, m_lookAtPos.y, m_lookAtPos.z));
}
