#pragma once

#include "Vec3.h"
#include "Collidable.h"

class EnemyMageBullet : public Collidable
{
public:
	EnemyMageBullet(Vec3 pos, Vec3 vec);
	~EnemyMageBullet();

	void Update();
	void Draw();

	virtual void OnCollide(Collidable* collider) override;

	bool IsDead() const { return m_isDead; }

private:
	Vec3 m_pos;
	Vec3 m_vec;
	float m_frame;
	bool m_isDead;
};

