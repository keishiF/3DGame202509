#pragma once

#include "Vec3.h"
#include "Collidable.h"

class EnemyMageBullet : public Collidable
{
public:
	EnemyMageBullet();
	~EnemyMageBullet();

	void Init(Vec3 myPos, Vec3 playerPos, std::shared_ptr<Physics> physics);
	void Update();
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	bool IsDead() const { return m_isDead; }

private:
	Vec3 m_dir;
	float m_frame;
	bool m_isDead;
};

