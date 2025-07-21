#include "Collidable.h"
#include "SphereColliderData.h"
#include "CapsuleColliderData.h"
#include "Physics.h"
#include "PolygonColliderData.h"
#include <cassert>

Collidable::Collidable(ObjectTag tag, ObjectPriority priority, ColliderData::Kind colliderKind):
	m_tag(tag),
	m_priority(priority),
	m_colliderData(nullptr)
{
	m_colliderData = CreateColliderData(colliderKind);
}

Collidable::~Collidable()
{
}

void Collidable::Init()
{
	Physics::Instance().Entry(shared_from_this());
}

void Collidable::Final()
{
	Physics::Instance().Exit(shared_from_this());
}

std::shared_ptr<ColliderData> Collidable::CreateColliderData(ColliderData::Kind kind)
{
	if (m_colliderData != nullptr)
	{
		assert(0 && "colliderDataはすでに作られています");
		return m_colliderData;
	}

	if (kind == ColliderData::Kind::Capsule)
	{
		// カプセルコライダーの情報を入れる
		return std::make_shared<CapsuleColliderData>();
	}
	else if (kind == ColliderData::Kind::Sphere)
	{
		// スフィアコライダーの情報を入れる
		return std::make_shared<SphereColliderData>();
	}
	else if (kind == ColliderData::Kind::Polygon)
	{
		// ポリゴンコライダーの情報を入れる
		return std::make_shared<PolygonColliderData>();
	}
	else
	{
		return nullptr;
	}
}
