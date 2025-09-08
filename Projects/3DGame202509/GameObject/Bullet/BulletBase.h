#pragma once
#include "Collidable.h"

class CharacterBase;
class BulletBase : public Collidable
{
public:
	BulletBase(ObjectTag tag, ObjectPriority priority, ColliderData::Kind colliderKind);
	virtual ~BulletBase();

	virtual void Init(Vector3 pos, Vector3 dir) abstract;
	virtual void Update() abstract;
	virtual void Draw() abstract;

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;
	bool IsDead() const { return m_isDead; }

protected:
	Vector3 m_dir;
	float m_lifeFrame;
	bool m_isDead;
};

