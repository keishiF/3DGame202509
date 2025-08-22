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
};

