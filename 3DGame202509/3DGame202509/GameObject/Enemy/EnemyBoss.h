#pragma once
#include "EnemyBase.h"

class EnemyBossRightAxe;
class EnemyBossLeftAxe;
class EnemyBoss : public EnemyBase
{
public:
	// コンストラクタとデストラクタ
	EnemyBoss();
	~EnemyBoss();

	// 初期化
	void Init(Vector3& pos, Vector3& rot, Vector3& scale) override;
	// 更新
	void Update(std::shared_ptr<Player> player) override;
	// 描画
	void Draw() override;

	// ダメージを受けた時の処理
	virtual void OnDamage() override;

	// HP割合の取得
	float GetHpRate() const { return m_hpRate; }

	// 死んでいるかどうか
	bool IsDead() { return m_isDead; }

	// 3D空間における位置座標をスクリーン座標に変換したものを取得
	Vector3 GetScreenPos() const;

	// 敵の状態
private:
	const char* GetAnimName(EnemyState state) const override;
	bool IsLoopAnim(EnemyState state) const override;

	// 歩き状態
	void WalkUpdate(std::shared_ptr<Player> player);
	// Playerを探している状態
	void FindUpdate(std::shared_ptr<Player> player) override;
	// 発見状態
	void ChaseUpdate(std::shared_ptr<Player> player) override;
	// 攻撃状態
	void AttackUpdate(std::shared_ptr<Player> player) override;
	void ChopUpdate(std::shared_ptr<Player> player);
	void SliceUpdate(std::shared_ptr<Player> player);
	void StabUpdate(std::shared_ptr<Player> player);
	void SpinUpdate(std::shared_ptr<Player> player);
	// 被弾状態
	void HitUpdate(std::shared_ptr<Player> player) override;
	// 死亡状態
	void DeadUpdate(std::shared_ptr<Player> player) override;

	std::shared_ptr<EnemyBossRightAxe> m_rightWeapon;
	std::shared_ptr<EnemyBossLeftAxe> m_leftWeapon;

	float m_walkFrame;
	bool m_isDead;
};

