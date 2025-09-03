#pragma once
#include "EnemyBase.h"

class EnemyBossLeftWeapon;
class EnemyBossRightWeapon;
class EnemyBoss : public EnemyBase
{
public:
	EnemyBoss();
	~EnemyBoss();

	virtual void Init(Vector3& pos, Vector3& rot, Vector3& scale) override;
	virtual void Draw() override;

	virtual void Update(std::shared_ptr<Player> player) override;

	// “G‚Ìó‘Ô
private:
	const char* GetAnimName(EnemyState state) const override;
	virtual float GetAnimPlaySpeed(EnemyState state) const override;
	bool IsLoopAnim(EnemyState state) const override;

	// ‘Ò‹@ó‘Ô
	void FindUpdate(std::shared_ptr<Player> player) override;
	// •à‚«ó‘Ô
	void WalkUpdate(std::shared_ptr<Player> player) override;
	// ”­Œ©ó‘Ô
	void ChaseUpdate(std::shared_ptr<Player> player) override;
	// UŒ‚ó‘Ô
	void AttackUpdate(std::shared_ptr<Player> player) override;
	void ChopUpdate(std::shared_ptr<Player> player);
	void SliceUpdate(std::shared_ptr<Player> player);
	void StabUpdate(std::shared_ptr<Player> player);
	void SpinUpdate(std::shared_ptr<Player> player);
	// ”í’eó‘Ô
	void HitUpdate(std::shared_ptr<Player> player) override;
	// €–Só‘Ô
	void DeadUpdate(std::shared_ptr<Player> player) override;
	// À‚èó‘Ô
	void TiredUpdate(std::shared_ptr<Player> player);

	int m_deadEffect = -1;
	int m_playingEffect = -1;

	int m_chopSE = -1;

	float m_walkFrame = 0.0f;
	float m_tiredFrame = 0.0f;
	int m_atkCount = 0;

	std::shared_ptr<EnemyBossLeftWeapon> m_leftWeapon;
	std::shared_ptr<EnemyBossRightWeapon> m_rightWeapon;

	bool m_isScoreAdded = false;
};

