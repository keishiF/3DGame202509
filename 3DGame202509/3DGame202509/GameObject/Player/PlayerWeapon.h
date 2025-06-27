#pragma once
#include "Collidable.h"
#include "DxLib.h"

struct AttackTiming
{
	float start;
	float end;
};

class PlayerWeapon : public Collidable
{
public:
	PlayerWeapon();
	~PlayerWeapon();

	void Init(std::shared_ptr<Physics> physics);
	void Update(int model, float currentFrame, const AttackTiming& timing);
	// �ҋ@���
	void IdleUpdate(int model);
	// �e�U�����
	void AttackUpdate(int model);
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	int m_model;
};

