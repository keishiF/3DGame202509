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

	void Init(std::shared_ptr<Physics> physics, Vec3& pos, const Vec3& rot, const Vec3& scale) override;
	void Update(std::shared_ptr<Player> player) override;
	void Draw() override;

	virtual void OnDamage() override;

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

	bool IsPlayerFind(const std::shared_ptr<Player>& player, const Vec3& centerDir, float viewAngleRad, float viewDistance);

	float m_angle;        // 経過時間（スイング用）
	float m_rotSpeed = 1.5f;        // 回転速度（ラジアン/秒）
	float m_angleMax   = DX_PI_F / 4; // 左右最大回転角（±45度）

	Vec3 m_forward;

	std::vector<std::shared_ptr<EnemyMageBullet>> m_bullets;

	std::shared_ptr<Physics> m_physics;
};

