#pragma once
#include "EnemyBase.h"

class EnemyMinionWeapon;
class EnemyMinion : public EnemyBase
{
public:
	EnemyMinion();
	~EnemyMinion();

	virtual void Init(Vector3& pos, Vector3& rot, Vector3& scale) override;
	virtual void Draw() override;

	virtual void Update(std::shared_ptr<Player> player) override;

	// 敵の状態
private:
	const char* GetAnimName(EnemyState state) const override;
	virtual float GetAnimPlaySpeed(EnemyState state) const override;
	bool IsLoopAnim(EnemyState state) const override;

	// 待機状態
	void FindUpdate(std::shared_ptr<Player> player) override;
	// 歩き状態
	void WalkUpdate(std::shared_ptr<Player> player) override;
	// 発見状態
	void ChaseUpdate(std::shared_ptr<Player> player) override;
	// 攻撃状態
	void AttackUpdate(std::shared_ptr<Player> player) override;
	// 被弾状態
	void HitUpdate(std::shared_ptr<Player> player) override;
	// 死亡状態
	void DeadUpdate(std::shared_ptr<Player> player) override;

	std::shared_ptr<EnemyMinionWeapon> m_weapon;
};

