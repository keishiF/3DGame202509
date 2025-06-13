#pragma once

#include "EnemyBase.h"
#include "Vec3.h"
#include "Animation.h"
#include <memory>

class Player;
class EnemyMinion : public EnemyBase
{
public:
	// �R���X�g���N�^�ƃf�X�g���N�^
	EnemyMinion();
	virtual ~EnemyMinion();

	// �X�V�A�`��
	void Update(std::shared_ptr<Player> player) override;
	void Draw() override;

	//// �v���C���[�̈ʒu���擾
	//Vec3 GetPos() const { return m_pos; }

	//// �_���[�W���󂯂����̏������܂Ƃ߂�֐�
	//void OnDamage();

	//bool IsDead() const { return m_isDead; }

	// �G�̏��
private:
	enum class MinionState
	{
		Find,
		Chase,
		Attack,
		Hit,
		Dead
	};

	MinionState m_state;
	void ChangeState(MinionState newState);

	// �ҋ@���
	void FindUpdate(std::shared_ptr<Player> player) override;

	// �������
	void ChaseUpdate(std::shared_ptr<Player> player) override;

	// �U�����
	void AttackUpdate(std::shared_ptr<Player> player) override;

	// ��e���
	void HitUpdate(std::shared_ptr<Player> player) override;

	// ���S���
	void DeadUpdate(std::shared_ptr<Player> player) override;

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

	Animation m_anim;
};

