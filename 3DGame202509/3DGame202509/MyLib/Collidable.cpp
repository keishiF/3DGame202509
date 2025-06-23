#include "Collidable.h"
#include "SphereColliderData.h"
#include "CapsuleColliderData.h"
#include "Physics.h"
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

void Collidable::Init(std::shared_ptr<Physics> physics)
{
	physics->Entry(shared_from_this());
}

void Collidable::Final(std::shared_ptr<Physics> physics)
{
	physics->Exit(shared_from_this());
}

std::shared_ptr<ColliderData> Collidable::CreateColliderData(ColliderData::Kind kind)
{
	if (m_colliderData != nullptr)
	{
		assert(0 && "colliderData�͂��łɍ���Ă��܂�");
		return m_colliderData;
	}

	if (kind == ColliderData::Kind::Capsule)
	{
		//�J�v�Z���R���C�_�[�̏�������
		return std::make_shared<CapsuleColliderData>();
	}
	else if (kind == ColliderData::Kind::Sphere)
	{
		//�X�t�B�A�R���C�_�[�̏�������
		return std::make_shared<SphereColliderData>();
	}
	else
	{
		return nullptr;
	}
}
