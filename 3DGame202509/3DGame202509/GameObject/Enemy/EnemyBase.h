#pragma once

#include "Vec3.h"
#include "Collidable.h"
#include "Physics.h"
#include "Animator.h"
#include <memory>

class Player;
class EnemyBase : public Collidable
{
public:
	EnemyBase();
	virtual ~EnemyBase();

	virtual void Init(std::shared_ptr<Physics> physics) abstract;
	virtual void Update(std::shared_ptr<Player> player) abstract;
	virtual void Draw() abstract;

	//Vec3 GetPos() const { return m_pos; }
	virtual void OnDamage() abstract;
	bool IsDead() const { return m_isDead; }

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

protected:
	enum class EnemyState
	{
		Find,
		Chase,
		Attack,
		Hit,
		Dead
	};

	EnemyState m_state;
	virtual const char* GetAnimName(EnemyState state) const abstract;
	virtual bool IsLoopAnim(EnemyState state) const abstract;
	void ChangeState(EnemyState newState, float playSpeed);

	virtual void FindUpdate(std::shared_ptr<Player> player) abstract;
	virtual void ChaseUpdate(std::shared_ptr<Player> player) abstract;
	virtual void AttackUpdate(std::shared_ptr<Player> player) abstract;
	virtual void HitUpdate(std::shared_ptr<Player> player) abstract;
	virtual void DeadUpdate(std::shared_ptr<Player> player) abstract;

protected:
	// �L�����ƕ���̃��f��
	int m_charModel;
	int m_weaponModel;

	// �v���C���[��T�m����͈�
	float m_findRadius;
	// �U����ԂɈڍs�ł���͈�
	float m_attackRadius;
	// HP
	int m_hp;
	// ����ł��邩�ǂ���
	bool m_isDead;
	// �U���t���[��
	float m_attackFrame;
	// �A�j���[�V�����Ǘ�
	Animator m_anim;
};

