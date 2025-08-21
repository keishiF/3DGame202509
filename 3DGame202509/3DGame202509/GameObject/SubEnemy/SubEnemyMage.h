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

	// 敵の状態
private:
	const char* GetAnimName(SubEnemyState state) const override;
	virtual float GetAnimPlaySpeed(SubEnemyState state) const override;
	bool IsLoopAnim(SubEnemyState state) const override;

	// 待機状態
	void FindUpdate(std::shared_ptr<SubPlayer> player) override;
	// 歩き状態
	void WalkUpdate(std::shared_ptr<SubPlayer> player) override;
	// 発見状態
	void ChaseUpdate(std::shared_ptr<SubPlayer> player) override;
	// 攻撃状態
	void AttackUpdate(std::shared_ptr<SubPlayer> player) override;
	// 被弾状態
	void HitUpdate(std::shared_ptr<SubPlayer> player) override;
	// 死亡状態
	void DeadUpdate(std::shared_ptr<SubPlayer> player) override;

	bool IsPlayerFind(const std::shared_ptr<SubPlayer>& player, const Vector3& centerDir, float viewAngleRad, float viewDistance);

	int m_weaponModel = -1;

	float m_angle = 0.0f;
	float m_rotSpeed = 0.0f;
	float m_angleMax = 0.0f;
};

