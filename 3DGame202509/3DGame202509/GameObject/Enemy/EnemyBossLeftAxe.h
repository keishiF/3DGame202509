#pragma once
#include "Collidable.h"

struct LeftAttackTiming
{
	float start;
	float end;
};

class EnemyBossLeftAxe : public Collidable
{
public:
	EnemyBossLeftAxe();
	~EnemyBossLeftAxe();

	void Init();
	void Update(int model, float currentFrame, const LeftAttackTiming& timing);
	// 待機状態
	void IdleUpdate(int model);
	// 各攻撃状態
	void AttackUpdate(int model);
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	int m_model;
};

