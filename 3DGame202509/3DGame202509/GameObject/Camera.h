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
	// �X�V����
	void Update(std::shared_ptr<Player> player);
	// �J�����̏�����
	void SetCamera(std::shared_ptr<Player> player);

private:
	// �J�����̈ʒu
	Vec3 m_pos;
	// �J�����̒����_
	Vec3 m_lookAtPos;
	// �J�����̎���p
	float m_fov;
	// ���b�N�I���t���O
	bool m_isLockOn;
};

