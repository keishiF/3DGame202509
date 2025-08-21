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

	Vector3 GetScreenPos() const;

protected:
	SubEnemyState m_state;
	SubEnemyState m_prevState;
	virtual const char* GetAnimName(SubEnemyState state) const abstract;
	virtual float GetAnimPlaySpeed(SubEnemyState state) const abstract;
	virtual bool IsLoopAnim(SubEnemyState state) const abstract;
	void ChangeState(SubEnemyState newState);

	// Player‚ğ’T‚µ‚Ä‚¢‚éó‘Ô
	virtual void FindUpdate(std::shared_ptr<SubPlayer> player) abstract;
	// Player‚ğŒ©‚Â‚¯‚Ä’Ç‚¢‚©‚¯‚Ä‚¢‚éó‘Ô
	virtual void ChaseUpdate(std::shared_ptr<SubPlayer> player) abstract;
	// UŒ‚ó‘Ô
	virtual void AttackUpdate(std::shared_ptr<SubPlayer> player) abstract;
	// ”í’eó‘Ô
	virtual void HitUpdate(std::shared_ptr<SubPlayer> player) abstract;
	// €–Só‘Ô
	virtual void DeadUpdate(std::shared_ptr<SubPlayer> player) abstract;

	// ƒvƒŒƒCƒ„[‚ğ’T’m‚Å‚«‚é”ÍˆÍ
	float m_playerFindRadius = 0.0f;
	// UŒ‚‚ÉˆÚs‚Å‚«‚é”ÍˆÍ
	float m_atkRadius = 0.0f;
};

