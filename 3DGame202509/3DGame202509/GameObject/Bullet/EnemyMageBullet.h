#pragma once
#include "BulletBase.h"

class EnemyMageBullet : public BulletBase
{
public:
	EnemyMageBullet();
	~EnemyMageBullet();

	virtual void Init(Vector3 pos, Vector3 dir) override;
	virtual void Update() override;
	virtual void Draw() override;

private:
	int m_bulletEffect = -1;
	int m_playingEffect = -1;
};

