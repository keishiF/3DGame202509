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
	// ‘Ò‹@ó‘Ô
	void IdleUpdate(int model);
	// ŠeUŒ‚ó‘Ô
	void AttackUpdate(int model);
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	int m_model;
};

