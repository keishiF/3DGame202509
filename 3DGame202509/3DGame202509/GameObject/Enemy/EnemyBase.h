#pragma once

#include "Vec3.h"
#include "Collidable.h"
#include "Animation.h"
#include <memory>

class Player;

class EnemyBase : public Collidable
{
public:
	EnemyBase();
	virtual ~EnemyBase();

	virtual void Update(std::shared_ptr<Player> player) abstract;
	virtual void Draw() abstract;

	Vec3 GetPos() const { return m_pos; }
	void OnDamage();
	bool IsDead() const { return m_isDead; }

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
	void ChangeState(EnemyState newState);

	virtual void FindUpdate(std::shared_ptr<Player> player) abstract;
	virtual void ChaseUpdate(std::shared_ptr<Player> player) abstract;
	virtual void AttackUpdate(std::shared_ptr<Player> player) abstract;
	virtual void HitUpdate(std::shared_ptr<Player> player) abstract;
	virtual void DeadUpdate(std::shared_ptr<Player> player) abstract;

protected:
	int m_charModel;
	int m_weaponModel;


	Vec3 m_pos;
	float m_findRadius;
	float m_attackRadius;

	int m_hp;

	bool m_isDead;

	Animation m_anim;
};

