#pragma once
#include "Collidable.h"
#include "DxLib.h"

class PlayerWeapon : public Collidable
{
public:
	PlayerWeapon();
	~PlayerWeapon();

	void Init(std::shared_ptr<Physics> physics);
	// ‘Ò‹@ó‘Ô
	void IdleUpdate(int model);
	// ŠeUŒ‚ó‘Ô
	void AttackUpdate(int model);
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	int m_model;
};

