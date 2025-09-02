#pragma once
#include "CharacterBase.h"

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
	Dead,
	Tired
};

class Player;
class EnemyBase : public CharacterBase
{
public:
	EnemyBase(ObjectTag tag, ObjectPriority priority, ColliderData::Kind colliderKind);
	virtual ~EnemyBase();

	virtual void Init(Vector3& pos, Vector3& rot, Vector3& scale) override;
	virtual void Draw() override;
	virtual void OnDamage(float atk) override;

	virtual void Update(std::shared_ptr<Player> player) abstract;

	Vector3 GetPos() const { return m_rigidbody.GetPos(); }
	Vector3 GetScreenPos() const;

protected:
	EnemyState m_state;
	EnemyState m_prevState;
	virtual const char* GetAnimName(EnemyState state) const abstract;
	virtual float GetAnimPlaySpeed(EnemyState state) const abstract;
	virtual bool IsLoopAnim(EnemyState state) const abstract;
	void ChangeState(EnemyState newState);

	// Playerを探している状態
	virtual void FindUpdate(std::shared_ptr<Player> player) abstract;
	// 歩き状態
	virtual void WalkUpdate(std::shared_ptr<Player> player) abstract;
	// Playerを見つけて追いかけている状態
	virtual void ChaseUpdate(std::shared_ptr<Player> player) abstract;
	// 攻撃状態
	virtual void AttackUpdate(std::shared_ptr<Player> player) abstract;
	// 被弾状態
	virtual void HitUpdate(std::shared_ptr<Player> player) abstract;
	// 死亡状態
	virtual void DeadUpdate(std::shared_ptr<Player> player) abstract;

	// プレイヤーを探知できる範囲
	float m_playerFindRadius = 0.0f;
	// 攻撃に移行できる範囲
	float m_atkRadius = 0.0f;
};

