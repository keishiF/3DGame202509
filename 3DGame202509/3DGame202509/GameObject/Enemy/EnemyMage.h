#pragma once

#include "EnemyBase.h"
#include <vector>
#include <memory>

class EnemyMageBullet;
class Physics;
class EnemyMage : public EnemyBase
{
public:
	EnemyMage();
	virtual ~EnemyMage();

	void Init(std::shared_ptr<Physics> physics) override;
	void Update(std::shared_ptr<Player> player) override;
	void Draw() override;

private:
	const char* GetAnimName(EnemyState state) const override;
	bool IsLoopAnim(EnemyState state) const override;

	// ‘Ò‹@ó‘Ô
	void FindUpdate(std::shared_ptr<Player> player) override;

	// ”­Œ©ó‘Ô
	void ChaseUpdate(std::shared_ptr<Player> player) override;

	// UŒ‚ó‘Ô
	void AttackUpdate(std::shared_ptr<Player> player) override;

	// ”í’eó‘Ô
	void HitUpdate(std::shared_ptr<Player> player) override;

	// €–Só‘Ô
	void DeadUpdate(std::shared_ptr<Player> player) override;

	std::vector<std::shared_ptr<EnemyMageBullet>> m_bullets;

	std::shared_ptr<Physics> m_physics;
};

