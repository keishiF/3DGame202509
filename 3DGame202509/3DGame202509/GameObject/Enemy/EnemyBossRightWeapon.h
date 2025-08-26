#pragma once
#include "Collidable.h"
#include "EnemyBossAtkColTiming.h"

class CharacterBase;
class EnemyBossRightWeapon : public Collidable
{
public:
	EnemyBossRightWeapon();
	~EnemyBossRightWeapon();

	void Init();
	void Update(int model, float currentFrame, const BossAtk::AtkTiming& timing);
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

private:
	std::weak_ptr<CharacterBase> m_owner;
};

