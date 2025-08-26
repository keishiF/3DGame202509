#pragma once
#include "Collidable.h"
#include "PlayerAtkColTiming.h"

class CharacterBase;
class PlayerWeapon : public Collidable
{
public:
	PlayerWeapon();
	~PlayerWeapon();

	// 初期化
	void Init();
	// 更新
	void Update(int model, float currentFrame, const AtkTiming& timing, bool specialFlag);
	// 待機状態
	void IdleUpdate(int model);
	// 攻撃状態
	void AttackUpdate(int model);
	// 必殺技状態
	void SpecialUpdate(int model);
	// 描画
	void Draw();
	// 持ち主をセット
	void SetOwner(std::shared_ptr<CharacterBase> owner);

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
	virtual float GetAtk() const override;

	int m_model;

private:
	std::weak_ptr<CharacterBase> m_owner;
};

