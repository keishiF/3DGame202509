#pragma once
#include "Collidable.h"
#include <DxLib.h>

struct RightAttackTiming
{
	float start;
	float end;
};

class PlayerRightWeapon : public Collidable
{
public:
	PlayerRightWeapon();
	~PlayerRightWeapon();

	// ‰Šú‰»
	void Init();
	// XV
	void Update(int model, float currentFrame, const RightAttackTiming& timing, bool specialFlag);
	// ‘Ò‹@ó‘Ô
	void IdleUpdate(int model);
	// UŒ‚ó‘Ô
	void AttackUpdate(int model);
	// •KE‹Zó‘Ô
	void SpecialUpdate(int model);
	// •`‰æ
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	int m_model;
};

