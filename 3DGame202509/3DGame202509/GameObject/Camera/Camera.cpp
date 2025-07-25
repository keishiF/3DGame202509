#include "Camera.h"
#include "Player/Player.h"
#include <cmath>
#include <algorithm>

namespace
{
	constexpr float kLerpRate = 0.05f;

	constexpr float kRotSpeed = 0.00005f;

	constexpr float kCameraPosX = 0.0f;
	constexpr float kCameraPosY = 700.0f;
	constexpr float kCameraPosZ = -600.0f;
	/*constexpr float kCameraPosY = 300.0f;
	constexpr float kCameraPosZ = -600.0f;*/
	constexpr float kCameraOffsetY = 150.0f;
}

Camera::Camera() :
	m_pos(0.0f, 0.0f, 0.0f),
	m_lookAtPos(0.0f, 0.0f, 0.0f),
	m_fov(DX_PI_F / 2.5f),
	m_isLockOn(false),
	m_cameraRotX(0.0f),
	m_cameraRotY(0.0f)
{
}

Camera::~Camera()
{
}

void Camera::Update(std::shared_ptr<Player> player)
{
	// プレイヤーの位置を取得し、カメラをプレイヤーの位置に合わせる
	Vector3 playerPos = player->GetPos();
	Vector3 cameraPos = { kCameraPosX, kCameraPosY, kCameraPosZ };
	m_pos = playerPos + cameraPos;

	// カメラの注視点をプレイヤーの位置に合わせる
	m_lookAtPos = { playerPos.x, playerPos.y + kCameraOffsetY, playerPos.z };

	Vector3 offset = { kCameraPosX, kCameraPosY, kCameraPosZ };

#ifdef _DEBUG
	int inputX, inputY;
	// 右スティックの入力を取得
	GetJoypadAnalogInputRight(&inputX, &inputY, DX_INPUT_PAD1);

	// カメラの回転をジョイパッドの入力に基づいて更新
	if (inputX != 0)
	{
		m_cameraRotX += inputX * kRotSpeed;
	}
	if (inputY != 0)
	{
		m_cameraRotY += inputY * kRotSpeed;
		// 縦回転に制限を付ける
		m_cameraRotY = std::clamp(m_cameraRotY, -DX_PI_F / 3.0f, DX_PI_F / 5.80f);
	}
#endif

	// カメラの位置を回転に基づいて更新
	Vector3 rotatedOffset = {
		offset.x * cos(m_cameraRotX) - offset.z * sin(m_cameraRotX),
		offset.y * cos(m_cameraRotY) - offset.z * sin(m_cameraRotY),
		offset.x * sin(m_cameraRotX) + offset.z * cos(m_cameraRotX)
	};
	m_pos = playerPos + rotatedOffset;

	DxLib::SetCameraPositionAndTarget_UpVecY(
		VGet(m_pos.x, m_pos.y, m_pos.z),
		VGet(m_lookAtPos.x, m_lookAtPos.y, m_lookAtPos.z));
}

void Camera::SetCamera(std::shared_ptr<Player> player)
{
	// プレイヤーの位置を取得し、カメラをプレイヤーの位置に合わせる
	Vector3 playerPos = player->GetPos();
	Vector3 cameraPos = { kCameraPosX, kCameraPosY, kCameraPosZ };
	m_pos = playerPos + cameraPos;

	// カメラの注視点をプレイヤーの位置に合わせる
	m_lookAtPos = { playerPos.x, playerPos.y + kCameraOffsetY, playerPos.z };

	SetupCamera_Perspective(m_fov);
	DxLib::SetCameraPositionAndTarget_UpVecY(
		VGet(m_pos.x, m_pos.y, m_pos.z),
		VGet(m_lookAtPos.x, m_lookAtPos.y, m_lookAtPos.z));
}
