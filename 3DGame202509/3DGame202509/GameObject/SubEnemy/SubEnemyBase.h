#pragma once
#include "CharacterBase.h"

enum class SubEnemyState
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

class SubPlayer;
class SubEnemyBase : public CharacterBase
{
public:
	SubEnemyBase(ObjectTag tag, ObjectPriority priority, ColliderData::Kind colliderKind);
	virtual ~SubEnemyBase();

	virtual void Init(Vector3& pos, Vector3& rot, Vector3& scale) override;
	virtual void Draw() override;
	virtual void OnDamage(float atk) override;

	virtual void Update(std::shared_ptr<SubPlayer> player) abstract;

	Vector3 GetPos() const { return m_rigidbody.GetPos(); }
	Vector3 GetScreenPos() const;

protected:
	SubEnemyState m_state;
	SubEnemyState m_prevState;
	virtual const char* GetAnimName(SubEnemyState state) const abstract;
	virtual float GetAnimPlaySpeed(SubEnemyState state) const abstract;
	virtual bool IsLoopAnim(SubEnemyState state) const abstract;
	void ChangeState(SubEnemyState newState);

	// Playerを探している状態
	virtual void FindUpdate(std::shared_ptr<SubPlayer> player) abstract;
	// 歩き状態
	virtual void WalkUpdate(std::shared_ptr<SubPlayer> player) abstract;
	// Playerを見つけて追いかけている状態
	virtual void ChaseUpdate(std::shared_ptr<SubPlayer> player) abstract;
	// 攻撃状態
	virtual void AttackUpdate(std::shared_ptr<SubPlayer> player) abstract;
	// 被弾状態
	virtual void HitUpdate(std::shared_ptr<SubPlayer> player) abstract;
	// 死亡状態
	virtual void DeadUpdate(std::shared_ptr<SubPlayer> player) abstract;

	// プレイヤーを探知できる範囲
	float m_playerFindRadius = 0.0f;
	// 攻撃に移行できる範囲
	float m_atkRadius = 0.0f;
};

