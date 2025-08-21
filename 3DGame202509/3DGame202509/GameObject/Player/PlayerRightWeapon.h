#pragma once
#include "Collidable.h"
#include <DxLib.h>

struct RightAttackTiming
{
	float start;
	float end;
};

class PlayerRightWeapon : public Collidable
{
public:
	PlayerRightWeapon();
	~PlayerRightWeapon();

	// 初期化
	void Init();
	// 更新
	void Update(int model, float currentFrame, const RightAttackTiming& timing, bool specialFlag);
	// 待機状態
	void IdleUpdate(int model);
	// 攻撃状態
	void AttackUpdate(int model);
	// 必殺技状態
	void SpecialUpdate(int model);
	// 描画
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	int m_model;
};

