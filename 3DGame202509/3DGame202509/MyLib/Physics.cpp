#include "Physics.h"
#include "Vec3.h"
#include "Collidable.h"
#include "CapsuleColliderData.h"
#include "SphereColliderData.h"
#include "Enemy/EnemyMage.h"
#include "Enemy/EnemyMinion.h"
#include "DxLib.h"
#include <cassert>
#include <algorithm>

void Physics::Entry(std::shared_ptr<Collidable> collider)
{
	// �o�^
	bool found = (std::find(m_collidables.begin(), m_collidables.end(), collider) != m_collidables.end());
	if (!found)
	{
		m_collidables.emplace_back(collider);
	}
	// ���ɓo�^����Ă���G���[
	else
	{
		assert(0 && "�w���collidable�͓o�^�ςł��B");
	}
}

void Physics::Exit(std::shared_ptr<Collidable> collider)
{
	// �o�^����
	bool found = (std::find(m_collidables.begin(), m_collidables.end(), collider) != m_collidables.end());
	if (found)
	{
		m_collidables.remove(collider);
	}
	// �o�^����ĂȂ�������G���[
	else
	{
		assert(0 && "�w���collidable���o�^����Ă��܂���B");
	}
}

void Physics::Update()
{
	//������
	for (auto& item : m_collidables)
	{
		auto pos = item->m_rigidbody.GetPos();
		auto nextPos = pos + item->m_rigidbody.GetVelo();
		item->m_nextPos = (item->m_rigidbody.GetPos() + item->m_rigidbody.GetVelo());

		item->m_nextPos = nextPos;
	}
	std::vector<OnCollideInfo> onCollideInfo = CheckCollide();
	FixPosition();
	for (auto& item : onCollideInfo)
	{
		item.owner->OnCollide(item.collider);
	}

	//�ʒu�C��
	FixPosition();
}

void Physics::DebugDraw()
{
	//�����蔻��̕`����s��
	for (auto& item : m_collidables)
	{
		// �����蔻�肪�I�t�ɂȂ��Ă�����͖̂�������
		if (!item->IsActive()) continue;

		if (item->m_colliderData->GetKind() == ColliderData::Kind::Capsule)
		{
			auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(item->m_colliderData);

			DrawCapsule3D(capsuleData->m_startPos.ToDxVECTOR(), item->m_rigidbody.GetPos().ToDxVECTOR(),
				capsuleData->m_radius, 16, 0xff00ff, 0xff00ff, false);
		}
		else if (item->m_colliderData->GetKind() == ColliderData::Kind::Sphere)
		{
			auto sphereData = std::dynamic_pointer_cast<SphereColliderData>(item->m_colliderData);

			DrawSphere3D(item->m_rigidbody.GetPos().ToDxVECTOR(), 
				sphereData->m_radius, 16, 0xff00ff, 0xff00ff, false);
		}
	}
}

void Physics::FixPosition()
{
	for (auto& item : m_collidables)
	{
		// Pos���X�V����̂ŁAvelocity�������Ɉړ�����velocity�ɏC��
		Vec3 toFixedPos = item->m_nextPos - item->m_rigidbody.GetPos();
		Vec3 nextPos = item->m_rigidbody.GetPos() + toFixedPos;

		item->m_rigidbody.SetVelo(toFixedPos);
		// �ʒu�m��
		item->m_rigidbody.SetPos(nextPos);

		//�����蔻�肪�J�v�Z����������
		if (item->m_colliderData->GetKind() == ColliderData::Kind::Capsule)
		{
			auto capsuleCol = std::dynamic_pointer_cast<CapsuleColliderData>(item->m_colliderData);
			//�L�т�J�v�Z�����ǂ����擾����
			if (!capsuleCol->m_isStartPos)
			{
				//�L�тȂ��J�v�Z���������珉���ʒu���ꏏ�ɓ�����
				capsuleCol->m_startPos = item->m_nextPos;
			}
		}
	}
}

void Physics::FixNextPosition(std::shared_ptr<Collidable> primary, std::shared_ptr<Collidable> secondary) const
{
	// �����蔻�肪�I�t�ɂȂ��Ă�����͖̂�������
	if (!primary->IsActive() || !secondary->IsActive()) return;

	auto primaryKind   = primary->m_colliderData->GetKind();
	auto secondaryKind = secondary->m_colliderData->GetKind();

	// �����m�̈ʒu�␳
	if (primaryKind == ColliderData::Kind::Sphere && secondaryKind == ColliderData::Kind::Sphere)
	{
		Vec3 primaryToSecondary = secondary->m_nextPos - primary->m_nextPos;
		primaryToSecondary.Normalize();

		auto primaryColliderData = dynamic_pointer_cast<SphereColliderData>(primary->m_colliderData);
		auto secondaryColliderData = dynamic_pointer_cast<SphereColliderData>(secondary->m_colliderData);
		float awayDist = primaryColliderData->m_radius + secondaryColliderData->m_radius;
		Vec3 primaryToNewSecondaryPos = primaryToSecondary * awayDist;
		Vec3 fixedPos = primary->m_nextPos + primaryToNewSecondaryPos;
		secondary->m_nextPos = fixedPos;
	}
	// ���ƃJ�v�Z���̈ʒu�␳
	else if (primaryKind == ColliderData::Kind::Sphere  && secondaryKind == ColliderData::Kind::Capsule ||
		     primaryKind == ColliderData::Kind::Capsule && secondaryKind == ColliderData::Kind::Sphere)
	{
	}
	// �J�v�Z���ƃJ�v�Z���̈ʒu�␳
	else if (primaryKind == ColliderData::Kind::Capsule && secondaryKind == ColliderData::Kind::Capsule)
	{
		auto primaryColliderData = dynamic_pointer_cast<CapsuleColliderData>(primary->m_colliderData);
		auto secondaryColliderData = dynamic_pointer_cast<CapsuleColliderData>(secondary->m_colliderData);

		// �J�v�Z���̐����̎n�_�ƏI�_
		Vec3 primaryStart   = primary->m_nextPos;
		Vec3 primaryEnd     = primaryColliderData->m_startPos;

		Vec3 secondaryStart = secondary->m_nextPos;
		Vec3 secondaryEnd   = secondaryColliderData->m_startPos;

		// �������m�̍ŋߐړ_�����߂�
		Vec3 closestPointA, closestPointB;
		SegmentClosestPoint(primaryStart, primaryEnd, secondaryStart, secondaryEnd, &closestPointA, &closestPointB);

		// �ŋߐړ_�Ԃ̋����ƕ���
		Vec3 closestDir = closestPointB - closestPointA;
		float dist = closestDir.Length();

		float awayDist = primaryColliderData->m_radius + secondaryColliderData->m_radius;

		if (dist == 0.0f)
		{
			closestDir = {1.0f, 0.0f, 0.0f};
			dist = 0.000001f;
		}
		else
		{
			closestDir.Normalize();
		}

		if (dist < awayDist)
		{
			Vec3 fixedPos = closestDir * (awayDist - dist);
			fixedPos.y = 0.0f;
			secondary->m_nextPos += fixedPos;
		}
	}
}

std::vector<Physics::OnCollideInfo> Physics::CheckCollide() const
{
	//�������Ă�����̂�����z��
	std::vector<OnCollideInfo> onCollideInfo;
	for (auto& first : m_collidables)
	{
		for (auto& second : m_collidables)
		{
			//�����蔻��`�F�b�N
			if (IsCollide(first, second))
			{
				auto priority1 = first->GetPriority();
				auto priority2 = second->GetPriority();
				std::shared_ptr<Collidable> primary   = first;
				std::shared_ptr<Collidable> secondary = second;
				if (priority1 < priority2)
				{
					primary = second;
					secondary = first;
				}

				if (!ShouldSkipFixPos(primary, secondary))
				{
					FixNextPosition(primary, secondary);
				}

				//��x���ꂽ���̂��x����Ȃ��悤�Ƀ`�F�b�N
				bool hasFirstColData = false;
				bool hasSecondColData = false;
				for (auto& item : onCollideInfo)
				{
					//���łɓ���Ă�����e��
					if (item.owner == primary)
					{
						hasFirstColData = true;
					}
					if (item.owner == secondary)
					{
						hasSecondColData = true;
					}
				}
				//�e����Ȃ������ꍇ�����������̃��X�g�ɓ����
				if (!hasFirstColData)
				{
					onCollideInfo.push_back({ first,second });
					/*if (first->GetTag() == ObjectTag::Player)
					{
						printfDx("�v���C���[�ƂԂ�����\n");
					}
					if (first->GetTag() == ObjectTag::Enemy)
					{
						printfDx("�G�l�~�[�ƂԂ�����\n");
					}*/
				}
				if (!hasSecondColData)
				{
					onCollideInfo.push_back({ second,first });
					/*if (first->GetTag() == ObjectTag::Player)
					{
						printfDx("�v���C���[�ƂԂ�����\n");
					}
					if (first->GetTag() == ObjectTag::Enemy)
					{
						printfDx("�G�l�~�[�ƂԂ�����\n");
					}*/
				}
			}
		}
	}
	//return std::vector<OnCollideInfo>();
	return onCollideInfo;
}

bool Physics::IsCollide(std::shared_ptr<Collidable> first, std::shared_ptr<Collidable> second) const
{
	// �����蔻�肪�I�t�ɂȂ��Ă�����͖̂�������
	if (!first->IsActive() || !second->IsActive()) return false;

	// ����̃^�O���m�ł͓����蔻�����炸�ɃX�L�b�v
	if (ShouldSkipCheckCollide(first, second))
	{
		return false;
	}

	//���̓����蔻��Ƒ��̓����蔻�肪���Ȃ����̂łȂ����
	if (first != second)
	{
		//�����蔻��̎�ނ��擾
		ColliderData::Kind firstKind = first->m_colliderData->GetKind();
		ColliderData::Kind secondKind = second->m_colliderData->GetKind();

		//���ǂ����̓����蔻��
		if (firstKind == ColliderData::Kind::Sphere && secondKind == ColliderData::Kind::Sphere)
		{
			//�����蔻������Ƀ_�E���L���X�g����
			auto firstCol = std::dynamic_pointer_cast<SphereColliderData>(first->m_colliderData);
			auto secondCol = std::dynamic_pointer_cast<SphereColliderData>(second->m_colliderData);

			//�����蔻��̋��������߂�
			float distance = (first->m_nextPos - second->m_nextPos).Length();

			//���̑傫�������킹�����̂����������Z����΂Ԃ����Ă���
			if (distance < firstCol->m_radius + secondCol->m_radius)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		//�J�v�Z�����m�̓����蔻��
		else if (firstKind == ColliderData::Kind::Capsule && secondKind == ColliderData::Kind::Capsule)
		{
			//�����蔻����J�v�Z���Ƀ_�E���L���X�g����
			auto firstCol = std::dynamic_pointer_cast<CapsuleColliderData>(first->m_colliderData);
			auto secondCol = std::dynamic_pointer_cast<CapsuleColliderData>(second->m_colliderData);

			//�J�v�Z�����m�̍ŒZ����
			float distance = Segment_Segment_MinLength(first->m_nextPos.ToDxVECTOR(), firstCol->m_startPos.ToDxVECTOR(),
				second->m_nextPos.ToDxVECTOR(), secondCol->m_startPos.ToDxVECTOR());

			if (distance < firstCol->m_radius + secondCol->m_radius)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		//���ƃJ�v�Z���̓����蔻��
		else if (firstKind == ColliderData::Kind::Sphere && secondKind == ColliderData::Kind::Capsule ||
			firstKind == ColliderData::Kind::Capsule && secondKind == ColliderData::Kind::Sphere)
		{
			//���ƃJ�v�Z���̃R���C�_�[�f�[�^�쐬
			std::shared_ptr<ColliderData> sphereDataBase;
			std::shared_ptr<ColliderData> capsuleDataBase;
			float distance;
			//�ǂ��炪�J�v�Z���Ȃ̂����Ȃ̂����ʂ��Ă���f�[�^������
			if (firstKind == ColliderData::Kind::Sphere)
			{
				sphereDataBase = first->m_colliderData;
				capsuleDataBase = second->m_colliderData;

				//�J�v�Z���̐����̃f�[�^���g�����߂Ƀ_�E���L���X�g
				auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleDataBase);
				//�����Ɠ_�̍ŋߋ��������߂�
				distance = Segment_Point_MinLength(second->m_nextPos.ToDxVECTOR(),
					capsuleData->m_startPos.ToDxVECTOR(), first->m_nextPos.ToDxVECTOR());
			}
			else
			{
				capsuleDataBase = first->m_colliderData;
				sphereDataBase = second->m_colliderData;
				//�J�v�Z���̐����̃f�[�^���g�����߂Ƀ_�E���L���X�g
				auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleDataBase);
				//�����Ɠ_�̍ŋߋ��������߂�
				distance = Segment_Point_MinLength(first->m_nextPos.ToDxVECTOR(),
					capsuleData->m_startPos.ToDxVECTOR(), second->m_nextPos.ToDxVECTOR());
			}
			//�_�E���L���X�g
			auto sphereData = std::dynamic_pointer_cast<SphereColliderData>(sphereDataBase);
			auto capsuleData = std::dynamic_pointer_cast<CapsuleColliderData>(capsuleDataBase);

			//�~�ƃJ�v�Z���̔��a�����~�ƃJ�v�Z���̋������߂���Γ������Ă���
			if (distance < sphereData->m_radius + capsuleData->m_radius)
			{
				return true;
			}
			else
			{
				return false;
			}


		}
		//�ǂ�ɂ����Ă͂܂�Ȃ�������
		else
		{
			assert(false);
			return false;
		}
	}
	return false;
}

bool Physics::ShouldSkipCheckCollide(std::shared_ptr<Collidable> primary, std::shared_ptr<Collidable> secondary) const
{
	if ((primary->GetTag() == ObjectTag::Player       && secondary->GetTag() == ObjectTag::PlayerWeapon) ||
		(primary->GetTag() == ObjectTag::PlayerWeapon && secondary->GetTag() == ObjectTag::Player)       ||
		(primary->GetTag() == ObjectTag::Enemy        && secondary->GetTag() == ObjectTag::EnemyWeapon)  ||
		(primary->GetTag() == ObjectTag::EnemyWeapon  && secondary->GetTag() == ObjectTag::Enemy)        ||
		(primary->GetTag() == ObjectTag::Enemy        && secondary->GetTag() == ObjectTag::Bullet)       ||
		(primary->GetTag() == ObjectTag::Bullet       && secondary->GetTag() == ObjectTag::Enemy)        ||
		(primary->GetTag() == ObjectTag::Enemy        && secondary->GetTag() == ObjectTag::Enemy))
	{
		return true;
	}
	return false;
}

bool Physics::ShouldSkipFixPos(std::shared_ptr<Collidable> primary, std::shared_ptr<Collidable> secondary) const
{
	if (primary->GetTag() == ObjectTag::Player       && secondary->GetTag() == ObjectTag::Bullet       ||
		primary->GetTag() == ObjectTag::Enemy        && secondary->GetTag() == ObjectTag::Bullet       ||
		primary->GetTag() == ObjectTag::Boss         && secondary->GetTag() == ObjectTag::Bullet       ||
		primary->GetTag() == ObjectTag::Enemy        && secondary->GetTag() == ObjectTag::PlayerWeapon ||
		primary->GetTag() == ObjectTag::Boss         && secondary->GetTag() == ObjectTag::PlayerWeapon ||
		primary->GetTag() == ObjectTag::PlayerWeapon && secondary->GetTag() == ObjectTag::Enemy        ||
		primary->GetTag() == ObjectTag::PlayerWeapon && secondary->GetTag() == ObjectTag::Boss         ||
		primary->GetTag() == ObjectTag::Player       && secondary->GetTag() == ObjectTag::EnemyWeapon  ||
		primary->GetTag() == ObjectTag::EnemyWeapon  && secondary->GetTag() == ObjectTag::Player)
	{
		return true;
	}
	return false;
}

void Physics::SegmentClosestPoint(Vec3& segAStart, Vec3& segAEnd,
	Vec3& segBStart, Vec3& segBEnd,
	Vec3* closestPtA, Vec3* closestPtB) const
{
	// ����A�̕����x�N�g��
	Vec3 segADir = segAEnd - segAStart;
	// ����B�̕����x�N�g��
	Vec3 segBDir = segBEnd - segBStart;
	// ����A�̎n�_�������B�̎n�_
	Vec3 startDiff = segAStart -segBStart;

	// ����A�̒���2��
	float segASqLen = segADir.Dot(segADir);
	// ����B�̒���2��
	float segBSqLen = segBDir.Dot(segBDir);
	// B�n�_��A�n�_�x�N�g�����A����B�ɂǂꂾ�������Ă��邩
	float segBDotDiff = segBDir.Dot(startDiff);

	float paramA, paramB;

	// �����̐������_�̏ꍇ
	if (segASqLen <= 0.000001f && segBSqLen <= 0.000001f)
	{
		// �o���̎n�_���ŋߐړ_�ɂ���
		*closestPtA = segAStart;
		*closestPtB = segBStart;
		return;
	}

	// ����A���_�̏ꍇ
	if (segASqLen <= 0.000001f)
	{
		paramA = 0.0f;
		// ����B��̍ŋߐړ_�̌v�Z�����A0~1�ɐ���
		paramB = segBDotDiff / segBSqLen;
		paramB = std::clamp(paramB, 0.0f, 1.0f);
	}
	else
	{
		// A�Ɍ������x�N�g����A�n�_�AB�n�_�̓���
		float segADotDiff = segADir.Dot(startDiff);

		// ����B���_�̏ꍇ
		if (segBSqLen <= 0.000001f)
		{
			paramB = 0.0f;
			// ����A��̍ŋߐړ_�̌v�Z�����A0~1�ɐ���
			paramA = -segADotDiff / segASqLen;
			paramA = std::clamp(paramA, 0.0f, 1.0f);
		}
		else
		{
			// A�Ɍ������x�N�g����B�Ɍ������x�N�g���̓���
			float segABDot = segADir.Dot(segBDir);
			// ��̐������ǂꂾ�����s����Ȃ���(0�ɋ߂��قǕ��s)
			float segmentParallel = segASqLen * segBSqLen - segABDot * segABDot;

			// ���s����Ȃ��ꍇ
			if (segmentParallel != 0.0f)
			{
				paramA = (segABDot * segBDotDiff - segADotDiff * segBSqLen) / segmentParallel;
				paramA = std::clamp(paramA, 0.0f, 1.0f);
			}
			else
			{
				// ���s�̏ꍇ��A�n�_�����̍ŋߐړ_�Ƃ���
				paramA = 0.0f;
			}

			// ����B��̍ŋߐړ_�̌v�Z
			paramB = (segABDot * paramA + segBDotDiff) / segBSqLen;

			// ����B�̊O�ɂ���ꍇ�A�␳����A���𒲐�
			if (paramB < 0.0f)
			{
				paramB = 0.0f;
				paramA = std::clamp(-segADotDiff / segASqLen, 0.0f, 1.0f);
			}
			else if (paramB > 1.0f)
			{
				paramB = 1.0f;
				paramA = std::clamp((segABDot - segADotDiff) / segASqLen, 0.0f, 1.0f);
			}
		}
	}

	// ������̍ŋߐړ_���W����
	*closestPtA = segAStart + (segADir * paramA);
	*closestPtB = segBStart + (segBDir * paramB);
}
