#pragma once
#include "Collidable.h"
#include "Vector3.h"

class EnemyMageBullet : public Collidable
{
public:
	EnemyMageBullet();
	~EnemyMageBullet();

	void Init(Vector3 myPos, Vector3 playerPos);
	void Update();
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	bool IsDead() const { return m_isDead; }

private:
	Vector3 m_dir;
	float m_frame;
	bool m_isDead;
};

