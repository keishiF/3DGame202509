#pragma once

#include "Vec3.h"
#include "Collidable.h"
#include <memory>

class Animation;
class Player;
class Enemy : public Collidable
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

	void OnCollide(const Collidable& collider) override;
	// �_���[�W���󂯂����̏������܂Ƃ߂�֐�
	void OnDamage();

	bool IsDead() const { return m_isDead; }

	// �G�̏��
private:
	enum class EnemyState
	{
		Find,
		Chase,
		Attack,
		Hit,
		Dead
	};

	EnemyState m_state;
	void ChangeState(EnemyState newState);

	// �ҋ@���
	void FindUpdate(std::shared_ptr<Player> player);

	// �������
	void ChaseUpdate(std::shared_ptr<Player> player);

	// �U�����
	void AttackUpdate(std::shared_ptr<Player> player);

	// ��e���
	void HitUpdate(std::shared_ptr<Player> player);

	// ���S���
	void DeadUpdate(std::shared_ptr<Player> player);

private:
	int m_minionModel;
	int m_bladeModel;

	Vec3 m_pos;
	float m_radius;
	float m_findRadius;
	float m_attackRadius;

	int m_hp;

	bool m_isDead;
	bool m_isFind;
	bool m_isAttack;

	Animation *m_anim;
};

