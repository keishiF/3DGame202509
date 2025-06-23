#pragma once

#include "Vec3.h"
#include "Collidable.h"

class EnemyMageBullet : public Collidable
{
public:
	EnemyMageBullet(Vec3 pos, Vec3 vec, std::shared_ptr<Physics> physics);
	~EnemyMageBullet();

	void Update();
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	bool IsDead() const { return m_isDead; }

private:
	float m_frame;
	bool m_isDead;
};

