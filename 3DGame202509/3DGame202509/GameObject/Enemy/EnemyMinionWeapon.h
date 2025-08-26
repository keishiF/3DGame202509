#pragma once
#include "Collidable.h"
#include "EnemyMinionAtkColTiming.h"

class CharacterBase;
class EnemyMinionWeapon : public Collidable
{
public:
	EnemyMinionWeapon();
	~EnemyMinionWeapon();

	void Init();
	void Update(int model, float currentFrame, const MinionAtk::AtkTiming& timing);
	// 待機状態
	void IdleUpdate(int model);
	// 各攻撃状態
	void AttackUpdate(int model);
	void Draw();
	// 持ち主をセット
	void SetOwner(std::shared_ptr<CharacterBase> owner);

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
	virtual float GetAtk() const override;

	int m_model = -1;
};

