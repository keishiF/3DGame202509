#pragma once
#include "ColliderData.h"
#include "Rigidbody.h"
#include "ObjectTag.h"
#include "ObjectPriority.h"
#include <memory>

class Physics;
class Collidable abstract : public std::enable_shared_from_this<Collidable>
{
public:
	Collidable(ObjectTag tag, ObjectPriority priority, ColliderData::Kind colliderKind);
	virtual ~Collidable();
	virtual void Init(std::shared_ptr<Physics> physics);
	virtual void Final(std::shared_ptr<Physics> physics);

	ObjectTag GetTag() const { return m_tag; }
	ObjectPriority GetPriority() const { return m_priority; }

	virtual void OnCollide(std::shared_ptr<Collidable> collider) abstract;

protected:
	Rigidbody m_rigidbody;
	float m_colRadius;

	std::shared_ptr<ColliderData> m_colliderData;
private:
	std::shared_ptr<ColliderData> CreateColliderData(ColliderData::Kind kind);

	ObjectTag m_tag;
	ObjectPriority m_priority;

	Vec3 m_nextPos;

	friend Physics;
};

