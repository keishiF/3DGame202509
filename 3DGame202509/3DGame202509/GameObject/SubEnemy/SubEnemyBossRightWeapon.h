#pragma once
#include "Collidable.h"
#include "EnemyBossAtkColTiming.h"

class SubEnemyBossRightWeapon : public Collidable
{
public:
	SubEnemyBossRightWeapon();
	~SubEnemyBossRightWeapon();

	void Init();
	void Update(int model, float currentFrame, const BossAtk::AtkTiming& timing);
	// ë“ã@èÛë‘
	void IdleUpdate(int model);
	// äeçUåÇèÛë‘
	void AttackUpdate(int model);
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	int m_model = -1;
};

