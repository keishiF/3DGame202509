#pragma once

#include "Vec3.h"
#include "Collidable.h"
#include <memory>

class Animation;
class Player;
class Enemy : public Collidable
{
public:
	// コンストラクタとデストラクタ
	Enemy();
	virtual ~Enemy();

	// 更新、描画
	void Update(std::shared_ptr<Player> player);
	void Draw();

	// プレイヤーの位置を取得
	Vec3 GetPos() const { return m_pos; }

	void OnCollide(const Collidable& collider) override;
	// ダメージを受けた時の処理をまとめる関数
	void OnDamage();

	bool IsDead() const { return m_isDead; }

	// 敵の状態
private:
	enum class EnemyState
	{
		Find,
		Chase,
		Attack,
		Hit,
		Dead
	};

	EnemyState m_state;
	void ChangeState(EnemyState newState);

	// 待機状態
	void FindUpdate(std::shared_ptr<Player> player);

	// 発見状態
	void ChaseUpdate(std::shared_ptr<Player> player);

	// 攻撃状態
	void AttackUpdate(std::shared_ptr<Player> player);

	// 被弾状態
	void HitUpdate(std::shared_ptr<Player> player);

	// 死亡状態
	void DeadUpdate(std::shared_ptr<Player> player);

private:
	int m_minionModel;
	int m_bladeModel;

	Vec3 m_pos;
	float m_radius;
	float m_findRadius;
	float m_attackRadius;

	int m_hp;

	bool m_isDead;
	bool m_isFind;
	bool m_isAttack;

	Animation *m_anim;
};

