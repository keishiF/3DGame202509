#pragma once
#include "Collidable.h"

struct AttackTiming
{
	float start;
	float end;
};

class EnemyBossAxe : public Collidable
{
public:
	EnemyBossAxe();
	~EnemyBossAxe();

	void Init(std::shared_ptr<Physics> physics);
	void Update(int model, float currentFrame, const AttackTiming& timing);
	// ‘Ò‹@ó‘Ô
	void IdleUpdate(int model);
	// ŠeUŒ‚ó‘Ô
	void AttackUpdate(int model);
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	int m_model;
};

