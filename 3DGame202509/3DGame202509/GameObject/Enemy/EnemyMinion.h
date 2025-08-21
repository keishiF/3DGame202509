#pragma once
#include "EnemyBase.h"

class EnemyMinionBlade;
class EnemyMinion : public EnemyBase
{
public:
	// コンストラクタとデストラクタ
	EnemyMinion();
	virtual ~EnemyMinion();

	// 初期化、更新、描画
	void Init(Vector3& pos, Vector3& rot, Vector3& scale) override;
	void Update(std::shared_ptr<Player> player) override;
	void Draw() override;

	virtual void OnDamage() override;

	// HP割合の取得
	float GetHpRate() const { return m_hpRate; }

	// 3D空間における位置座標をスクリーン座標に変換したものを取得
	Vector3 GetScreenPos() const;

	// 敵の状態
private:
	const char* GetAnimName(EnemyState state) const override;
	bool IsLoopAnim(EnemyState state) const override;

	// 待機状態
	void FindUpdate(std::shared_ptr<Player> player) override;

	void WalkUpdate(std::shared_ptr<Player> player);

	// 発見状態
	void ChaseUpdate(std::shared_ptr<Player> player) override;

	// 攻撃状態
	void AttackUpdate(std::shared_ptr<Player> player) override;

	// 被弾状態
	void HitUpdate(std::shared_ptr<Player> player) override;

	// 死亡状態
	void DeadUpdate(std::shared_ptr<Player> player) override;

	std::shared_ptr<EnemyMinionBlade> m_weapon;
};

