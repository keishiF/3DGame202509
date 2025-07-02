#pragma once
#include "EnemyBase.h"

class EnemyMinionBlade;
class EnemyMinion : public EnemyBase
{
public:
	// �R���X�g���N�^�ƃf�X�g���N�^
	EnemyMinion();
	virtual ~EnemyMinion();

	// �X�V�A�`��
	void Init(std::shared_ptr<Physics> physics, Vec3& pos, const Vec3& rot, const Vec3& scale) override;
	void Update(std::shared_ptr<Player> player) override;
	void Draw() override;

	virtual void OnDamage() override;

	// �G�̏��
private:
	const char* GetAnimName(EnemyState state) const override;
	bool IsLoopAnim(EnemyState state) const override;

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

	std::shared_ptr<EnemyMinionBlade> m_weapon;
};

