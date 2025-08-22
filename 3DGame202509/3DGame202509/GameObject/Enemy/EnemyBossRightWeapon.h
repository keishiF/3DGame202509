#pragma once
#include "Collidable.h"
#include "EnemyBossAtkColTiming.h"

class EnemyBossRightWeapon : public Collidable
{
public:
	EnemyBossRightWeapon();
	~EnemyBossRightWeapon();

	void Init();
	void Update(int model, float currentFrame, const BossAtk::AtkTiming& timing);
	// ‘Ò‹@ó‘Ô
	void IdleUpdate(int model);
	// ŠeUŒ‚ó‘Ô
	void AttackUpdate(int model);
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	int m_model = -1;
};

