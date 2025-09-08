#pragma once
#include "Collidable.h"
#include "PlayerAtkColTiming.h"

class PlayerWeapon : public Collidable
{
public:
	PlayerWeapon();
	~PlayerWeapon();

	// 初期化
	void Init();
	// 更新
	void Update(int model, 
		int currentFrame, 
		const AtkTiming& timing, 
		bool specialFlag, 
		float attackPower);
	// 待機状態
	void IdleUpdate(int model);
	// 攻撃状態
	void AttackUpdate(int model);
	// 必殺技状態
	void SpecialUpdate(int model);
	// 描画
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
	virtual float GetAttackPower() const override;

	int m_model;

private:
	float m_currentAttackPower = 0.0f;
};

