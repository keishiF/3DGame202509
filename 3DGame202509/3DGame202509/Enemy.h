#pragma once

#include "Vec3.h"
#include "Animation.h"
#include <memory>

class Player;
class Enemy
{
public:
	// �R���X�g���N�^�ƃf�X�g���N�^
	Enemy();
	virtual ~Enemy();

	// �X�V�A�`��
	void Update(std::shared_ptr<Player> player);
	void Draw();

	// �v���C���[�̈ʒu���擾
	Vec3 GetPos() const { return m_pos; }

	// �_���[�W���󂯂����̏������܂Ƃ߂�֐�
	void OnDamage();

	bool IsDead() const { return m_isDead; }

	// ���
private:
	// �ҋ@���
	void IdleInit(std::shared_ptr<Player> player);
	void IdleUpdate(std::shared_ptr<Player> player);

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
	int m_model;

	Vec3 m_pos;
	float m_findRadius;

	int m_hp;

	bool m_isDead;

	using EnemyState = void(Enemy::*)(std::shared_ptr<Player> player);
	EnemyState m_state;

	Animation m_anim;
};

