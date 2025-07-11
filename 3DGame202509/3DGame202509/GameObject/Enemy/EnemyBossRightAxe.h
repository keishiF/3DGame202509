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

	void Init(std::shared_ptr<Physics> physics);
	void Update(int model, float currentFrame, const RightAttackTiming& timing);
	// ‘Ò‹@ó‘Ô
	void IdleUpdate(int model);
	// ŠeUŒ‚ó‘Ô
	void AttackUpdate(int model);
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	int m_model;
};

