#pragma once
#include "EnemyBase.h"
#include <memory>
#include <vector>

class EnemyMageBullet;
class Physics;
class EnemyMage : public EnemyBase
{
public:
	EnemyMage();
	virtual ~EnemyMage();

	void Init(std::shared_ptr<Physics> physics, Vec3& pos, Vec3& rot, Vec3& scale) override;
	void Update(std::shared_ptr<Player> player) override;
	void Draw() override;

	virtual void OnDamage() override;

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

	bool IsPlayerFind(const std::shared_ptr<Player>& player, const Vec3& centerDir, float viewAngleRad, float viewDistance);

	float m_angle;
	float m_rotSpeed;
	float m_angleMax;

	Vec3 m_forward;

	std::vector<std::shared_ptr<EnemyMageBullet>> m_bullets;

	std::shared_ptr<Physics> m_physics;
};

