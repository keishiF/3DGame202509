#pragma once
#include "Collidable.h"
#include "EnemyAtkColTiming.h"

class EnemyMinionWeapon : public Collidable
{
public:
	EnemyMinionWeapon();
	~EnemyMinionWeapon();

	void Init();
	void Update(int model, float currentFrame, const AtkTiming& timing);
	// 待機状態
	void IdleUpdate(int model);
	// 各攻撃状態
	void AttackUpdate(int model);
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	int m_model = -1;
};

