#pragma once

#include "DxLib.h"
#include "Input.h"
#include "Vec3.h"
#include <memory>

class Player
{
public:
	// �R���X�g���N�^�ƃf�X�g���N�^
	Player();
	virtual ~Player();

	// �X�V�A�`��
	void Update(Input& input);
	void Draw();

	// �v���C���[�̈ʒu���擾
	Vec3 GetPos() const { return m_pos; }

	// �_���[�W���󂯂����̏������܂Ƃ߂�֐�
	void OnDamage();

private:
	// �v���C���[�̃��f��
	int m_model;
	// �v���C���[�̈ʒu
	Vec3 m_pos;
	// �v���C���[�̈ړ���
	Vec3 m_vec;
	// �v���C���[��HP
	int m_hp;
	float m_jumpSpeed;
	float m_gravity;

	// �o�߃t���[���𑪂�
	float m_frameCount;
};

