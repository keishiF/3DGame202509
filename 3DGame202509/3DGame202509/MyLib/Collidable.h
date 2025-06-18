#pragma once
#include "ColliderData.h"
#include "Rigidbody.h"
#include "GameObjectTag.h"
#include <memory>

class Physics;
class Collidable abstract : public std::enable_shared_from_this<Collidable>
{
public:
	Collidable(GameObjectTag tag, ColliderData::Kind colliderKind);
	virtual ~Collidable();
	virtual void Init(std::shared_ptr<Physics> physics);
	virtual void Final(std::shared_ptr<Physics> physics);

	GameObjectTag GetTag() const { return m_tag; }

	virtual void OnCollide(std::shared_ptr<Collidable> collider) abstract;

protected:
	Rigidbody m_rigidbody;
	float m_colRadius;

	std::shared_ptr<ColliderData> m_colliderData;
private:
	std::shared_ptr<ColliderData> CreateColliderData(ColliderData::Kind kind);

	GameObjectTag m_tag;

	Vec3 m_nextPos;

	friend Physics;
};

