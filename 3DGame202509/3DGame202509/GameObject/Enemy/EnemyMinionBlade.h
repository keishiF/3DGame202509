#pragma once
#include "Collidable.h"
#include <DxLib.h>

struct AttackTiming
{
	float start;
	float end;
};

class EnemyMinionBlade : public Collidable
{
public:
	EnemyMinionBlade();
	~EnemyMinionBlade();

	void Init();
	void Update(int model, float currentFrame, const AttackTiming& timing);
	// 待機状態
	void IdleUpdate(int model);
	// 各攻撃状態
	void AttackUpdate(int model);
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	int m_model;
};

