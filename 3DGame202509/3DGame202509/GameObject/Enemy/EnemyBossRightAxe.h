#pragma once
#include "Collidable.h"

struct RightAttackTiming
{
	float start;
	float end;
};

class EnemyBossRightAxe : public Collidable
{
public:
	EnemyBossRightAxe();
	~EnemyBossRightAxe();

	void Init();
	void Update(int model, float currentFrame, const RightAttackTiming& timing);
	// 待機状態
	void IdleUpdate(int model);
	// 各攻撃状態
	void AttackUpdate(int model);
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	int m_model;
};

