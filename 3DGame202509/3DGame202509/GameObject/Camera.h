#pragma once
#include "Vec3.h"
#include "DxLib.h"
#include "Input.h"
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

private:
	// カメラの位置
	Vec3 m_pos;
	// カメラの注視点
	Vec3 m_lookAtPos;
	// カメラの視野角
	float m_fov;
	// ロックオンフラグ
	bool m_isLockOn;
};

