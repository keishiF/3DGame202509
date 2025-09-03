#pragma once
#include "BulletBase.h"

class PlayerBullet : public BulletBase
{
public:
	PlayerBullet();
	~PlayerBullet();

	virtual void Init(Vector3 pos, Vector3 dir) override;
	virtual void Update() override;
	virtual void Draw() override;

	virtual float GetAttackPower() const override;

private:
	int m_bulletEffect = -1;
	int m_playingEffect = -1;
};

