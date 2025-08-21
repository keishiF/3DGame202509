#pragma once
#include "SubEnemyBase.h"

class SubEnemyBossLeftWeapon;
class SubEnemyBossRightWeapon;
class SubEnemyBoss : public SubEnemyBase
{
public:
	SubEnemyBoss();
	~SubEnemyBoss();

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
	// •à‚«ó‘Ô
	void WalkUpdate(std::shared_ptr<SubPlayer> player) override;
	// ”­Œ©ó‘Ô
	void ChaseUpdate(std::shared_ptr<SubPlayer> player) override;
	// UŒ‚ó‘Ô
	void AttackUpdate(std::shared_ptr<SubPlayer> player) override;
	void ChopUpdate(std::shared_ptr<SubPlayer> player);
	void SliceUpdate(std::shared_ptr<SubPlayer> player);
	void StabUpdate(std::shared_ptr<SubPlayer> player);
	void SpinUpdate(std::shared_ptr<SubPlayer> player);
	// ”í’eó‘Ô
	void HitUpdate(std::shared_ptr<SubPlayer> player) override;
	// €–Só‘Ô
	void DeadUpdate(std::shared_ptr<SubPlayer> player) override;

	float m_walkFrame = 0.0f;

	std::shared_ptr<SubEnemyBossLeftWeapon> m_leftWeapon;
	std::shared_ptr<SubEnemyBossRightWeapon> m_rightWeapon;
};

