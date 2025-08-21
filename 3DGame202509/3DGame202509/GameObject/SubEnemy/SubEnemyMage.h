#pragma once
#include "SubEnemyBase.h"

class SubEnemyMage : public SubEnemyBase
{
public:
	SubEnemyMage();
	~SubEnemyMage();

	virtual void Init(Vector3& pos, Vector3& rot, Vector3& scale) override;
	virtual void Draw() override;

	virtual void Update(std::shared_ptr<SubPlayer> player) override;

	// “G‚Ìó‘Ô
private:
	const char* GetAnimName(SubEnemyState state) const override;
	virtual float GetAnimPlaySpeed(SubEnemyState state) const override;
	bool IsLoopAnim(SubEnemyState state) const override;

	// ‘Ò‹@ó‘Ô
	void FindUpdate(std::shared_ptr<SubPlayer> player) override;

	void WalkUpdate(std::shared_ptr<SubPlayer> player);

	// ”­Œ©ó‘Ô
	void ChaseUpdate(std::shared_ptr<SubPlayer> player) override;

	// UŒ‚ó‘Ô
	void AttackUpdate(std::shared_ptr<SubPlayer> player) override;

	// ”í’eó‘Ô
	void HitUpdate(std::shared_ptr<SubPlayer> player) override;

	// €–Só‘Ô
	void DeadUpdate(std::shared_ptr<SubPlayer> player) override;

	bool IsPlayerFind(const std::shared_ptr<SubPlayer>& player, const Vector3& centerDir, float viewAngleRad, float viewDistance);

	int m_weaponModel = -1;

	float m_angle = 0.0f;
	float m_rotSpeed = 0.0f;
	float m_angleMax = 0.0f;
};

