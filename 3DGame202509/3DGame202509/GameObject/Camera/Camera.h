#pragma once
#include "Vector3.h"
#include <DxLib.h>
#include <memory>

class Player;
class Camera
{
public:
	Camera();
	~Camera();
	// 更新処理
	void Update(std::shared_ptr<Player> player);
	// カメラの初期化
	void SetCamera(std::shared_ptr<Player> player);

	Vector3 GetPos() const { return m_pos; }

private:
	// カメラの位置
	Vector3 m_pos;
	// カメラの注視点
	Vector3 m_lookAtPos;
	// カメラの視野角
	float m_fov;
	// ロックオンフラグ
	bool m_isLockOn;

	float m_cameraRotX;
	float m_cameraRotY;
};

