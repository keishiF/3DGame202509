#pragma once
#include "Collidable.h"
#include <DxLib.h>

struct LeftAttackTiming
{
	float start;
	float end;
};

class PlayerLeftWeapon : public Collidable
{
public:
	PlayerLeftWeapon();
	~PlayerLeftWeapon();

	// 初期化
	void Init();
	// 更新
	void Update(int model, float currentFrame, const LeftAttackTiming& timing);
	// 待機状態
	void IdleUpdate(int model);
	// 攻撃状態
	void AttackUpdate(int model);
	// 描画
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	int m_model;
};

