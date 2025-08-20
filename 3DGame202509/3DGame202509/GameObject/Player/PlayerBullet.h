#pragma once
#include "Collidable.h"
#include "Vector3.h"

class PlayerBullet : public Collidable
{
public:
	PlayerBullet();
	~PlayerBullet();

	void Init(Vector3 myPos, Vector3 dir);
	void Update();
	void Draw();

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	bool IsDead() const { return m_isDead; }

private:
	Vector3 m_dir;
	float m_frame;
	bool m_isDead;
};

