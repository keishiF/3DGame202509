#pragma once

#include "EnemyBase.h"
#include "Vec3.h"
#include "Animation.h"
#include <memory>

class Player;
class EnemyMinion : public EnemyBase
{
public:
	// コンストラクタとデストラクタ
	EnemyMinion();
	virtual ~EnemyMinion();

	// 更新、描画
	void Update(std::shared_ptr<Player> player) override;
	void Draw() override;

	//// プレイヤーの位置を取得
	//Vec3 GetPos() const { return m_pos; }

	//// ダメージを受けた時の処理をまとめる関数
	//void OnDamage();

	//bool IsDead() const { return m_isDead; }

	// 敵の状態
private:
	enum class MinionState
	{
		Find,
		Chase,
		Attack,
		Hit,
		Dead
	};

	MinionState m_state;
	void ChangeState(MinionState newState);

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

	Animation m_anim;
};

