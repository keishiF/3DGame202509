#pragma once

#include "Vec3.h"
#include "Animation.h"
#include <memory>

class Player;
class Enemy
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

	// ダメージを受けた時の処理をまとめる関数
	void OnDamage();

	bool IsDead() const { return m_isDead; }

	// 状態
private:
	// 待機状態
	void IdleInit(std::shared_ptr<Player> player);
	void IdleUpdate(std::shared_ptr<Player> player);

	// 発見状態
	void FoundInit();
	void FoundUpdate();

	// 攻撃状態
	void AttackInit();
	void AttackUpdate();

	// 被弾状態
	void HitInit();
	void HitUpdate();

	// 死亡状態
	void DeadInit();
	void DeadUpdate();

private:
	int m_model;

	Vec3 m_pos;
	float m_findRadius;

	int m_hp;

	bool m_isDead;

	using EnemyState = void(Enemy::*)(std::shared_ptr<Player> player);
	EnemyState m_state;

	Animation m_anim;
};

