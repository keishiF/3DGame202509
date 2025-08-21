#pragma once
#include "Animator.h"
#include "Collidable.h"
#include "Physics.h"
#include "Vector3.h"

enum class EnemyState
{
	Find,
	Walk,
	Chase,
	Attack,
	Chop,
	Slice,
	Stab,
	Spin,
	Hit,
	Dead
};

class Player;
class EnemyBase : public Collidable
{
public:
	EnemyBase();
	virtual ~EnemyBase();

	// 初期化
	virtual void Init(Vector3& pos, Vector3& rot, Vector3& scale) abstract;
	// 更新
	virtual void Update(std::shared_ptr<Player> player) abstract;
	// 描画
	virtual void Draw() abstract;

	// 位置を取得
	Vector3 GetPos() const;
	// ダメージを受けた時の処理
	virtual void OnDamage() abstract;
	// 死んでいるかどうか
	bool IsDead() const { return m_isDead; }

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

protected:
	EnemyState m_state;
	EnemyState m_prevState;
	virtual const char* GetAnimName(EnemyState state) const abstract;
	virtual bool IsLoopAnim(EnemyState state) const abstract;
	void ChangeState(EnemyState newState, float playSpeed);

	// Playerを探している状態
	virtual void FindUpdate(std::shared_ptr<Player> player) abstract;
	// Playerを見つけて追いかけている状態
	virtual void ChaseUpdate(std::shared_ptr<Player> player) abstract;
	// 攻撃状態
	virtual void AttackUpdate(std::shared_ptr<Player> player) abstract;
	// 被弾状態
	virtual void HitUpdate(std::shared_ptr<Player> player) abstract;
	// 死亡状態
	virtual void DeadUpdate(std::shared_ptr<Player> player) abstract;

protected:
	// キャラと武器のモデル
	int m_charModel;
	int m_weaponModel;

	// プレイヤーを探知する範囲
	float m_findRadius;
	// 攻撃状態に移行できる範囲
	float m_attackRadius;
	// HP
	float m_hp;
	float m_hpRate;
	// 死んでいるかどうか
	bool m_isDead;
	// 攻撃フレーム
	float m_attackFrame;
	// アニメーション管理
	Animator m_anim;
};

