#pragma once
#include "EnemyBase.h"

class EnemyMinionBlade;
class EnemyMinion : public EnemyBase
{
public:
	// コンストラクタとデストラクタ
	EnemyMinion();
	virtual ~EnemyMinion();

	// 更新、描画
	void Init(std::shared_ptr<Physics> physics, Vec3& pos, const Vec3& rot, const Vec3& scale) override;
	void Update(std::shared_ptr<Player> player) override;
	void Draw() override;

	virtual void OnDamage() override;

	// 敵の状態
private:
	const char* GetAnimName(EnemyState state) const override;
	bool IsLoopAnim(EnemyState state) const override;

	// 待機状態
	void FindUpdate(std::shared_ptr<Player> player) override;

	// 発見状態
	void ChaseUpdate(std::shared_ptr<Player> player) override;

	// 攻撃状態
	void AttackUpdate(std::shared_ptr<Player> player) override;

	// 被弾状態
	void HitUpdate(std::shared_ptr<Player> player) override;

	// 死亡状態
	void DeadUpdate(std::shared_ptr<Player> player) override;

	std::shared_ptr<EnemyMinionBlade> m_weapon;
};

