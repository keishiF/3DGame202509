#pragma once

#include "Vec3.h"
#include "Animation.h"

class Enemy
{
public:
	// �R���X�g���N�^�ƃf�X�g���N�^
	Enemy();
	virtual ~Enemy();

	// �X�V�A�`��
	void Update();
	void Draw();

	// �v���C���[�̈ʒu���擾
	Vec3 GetPos() const { return m_pos; }

	// �_���[�W���󂯂����̏������܂Ƃ߂�֐�
	void OnDamage();

	bool IsDead() const { return m_isDead; }

	// ���
private:
	// �ҋ@���
	void IdleInit();
	void IdleUpdate();

	// �������
	void FoundInit();
	void FoundUpdate();

	// �U�����
	void AttackInit();
	void AttackUpdate();

	// ��e���
	void HitInit();
	void HitUpdate();

	// ���S���
	void DeadInit();
	void DeadUpdate();

private:
	Vec3 m_pos;
	int m_hp;
	bool m_isDead;

	using EnemyState = void(Enemy::*)();
	EnemyState m_state;

	Animation m_anim;
};

