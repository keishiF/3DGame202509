#pragma once
#include "EnemyBase.h"
#include <memory>
#include <vector>

class EnemyMageBullet;
class EnemyMage : public EnemyBase
{
public:
	EnemyMage();
	virtual ~EnemyMage();

	void Init(Vector3& pos, Vector3& rot, Vector3& scale) override;
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

	bool IsPlayerFind(const std::shared_ptr<Player>& player, const Vector3& centerDir, float viewAngleRad, float viewDistance);

	float m_angle;
	float m_rotSpeed;
	float m_angleMax;

	Vector3 m_forward;

	std::vector<std::shared_ptr<EnemyMageBullet>> m_bullets;
};

