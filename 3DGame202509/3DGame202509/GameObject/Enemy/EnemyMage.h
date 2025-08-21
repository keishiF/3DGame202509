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

	// HP割合の取得
	float GetHpRate() const { return m_hpRate; }

	// 3D空間における位置座標をスクリーン座標に変換したものを取得
	Vector3 GetScreenPos() const;

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

	bool IsPlayerFind(const std::shared_ptr<Player>& player, const Vector3& centerDir, float viewAngleRad, float viewDistance);

	float m_angle;
	float m_rotSpeed;
	float m_angleMax;

	Vector3 m_forward;

	std::vector<std::shared_ptr<EnemyMageBullet>> m_bullets;
};

