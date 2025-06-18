#include "Physics.h"
#include "Vec3.h"
#include "Collidable.h"
#include "CapsuleColliderData.h"
#include "SphereColliderData.h"
#include "Enemy/EnemyMage.h"
#include "Enemy/EnemyMinion.h"
#include "DxLib.h"
#include <cassert>

void Physics::Entry(std::shared_ptr<Collidable> col)
{
	// �o�^
	bool found = (std::find(m_collidables.begin(), m_collidables.end(), col) != m_collidables.end());
	if (!found)
	{
		m_collidables.emplace_back(col);
	}
	// ���ɓo�^����Ă���G���[
	else
	{
		assert(0 && "�w���collidable�͓o�^�ςł��B");
	}
}

void Physics::Exit(std::shared_ptr<Collidable> col)
{
	// �o�^����
	bool found = (std::find(m_collidables.begin(), m_collidables.end(), col) != m_collidables.end());
	if (found)
	{
		m_collidables.remove(col);
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
		item->m_nextPos = (item->m_rigidbody.GetPos() + item->m_rigidbody.GetVelo());
	}
	//�������Ă�����̂�����z��
	std::vector<OnCollideInfo> pushData;
	for (auto& first : m_collidables)
	{
		for (auto& second : m_collidables)
		{
			//�����蔻��`�F�b�N
			if (CheckCollide(first, second))
			{
				//��x���ꂽ���̂��x����Ȃ��悤�Ƀ`�F�b�N
				bool hasFirstColData = false;
				bool hasSecondColData = false;
				for (auto& item : pushData)
				{
					//���łɓ���Ă�����e��
					if (item.owner == first)
					{
						hasFirstColData = true;
					}
					if (item.owner == second)
					{
						hasSecondColData = true;
					}
				}
				//�e����Ȃ������ꍇ�����������̃��X�g�ɓ����
				if (!hasFirstColData)
				{
					pushData.push_back({ first,second });
					//if (first->GetTag() == ObjectTag::kPlayer)
					//{
					//	printfDx("�v���C���[�ƂԂ�����");
					//}
					//if (first->GetTag() == ObjectTag::kEnemy)
					//{
					//	printfDx("�G�l�~�[�ƂԂ�����");
					//}
				}
				if (!hasSecondColData)
				{
					pushData.push_back({ second,first });
					//if (first->GetTag() == ObjectTag::kPlayer)
					//{
					//	printfDx("�v���C���[�ƂԂ�����");
					//}
					//if (first->GetTag() == ObjectTag::kEnemy)
					//{
					//	printfDx("�G�l�~�[�ƂԂ�����");
					//}
				}
			}
		}
	}
	//�������������蔻��̓����������̏������Ă�
	for (auto& hitCol : pushData)
	{
		hitCol.OnCollide();
	}
	//�ʒu�C��
	FixPosition();
}

void Physics::DebugDraw()
{
	//�����蔻��̕`����s��
	for (auto& item : m_collidables)
	{
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

		Vec3 centerPos(0, 0, 0);

		////TODO:�X�e�[�W�̓����蔻����쐬����
		////�ړ�������t����(������)
		//const float stageScale = 500.0f;
		//const float groundHeight = -50.0f;

		//if ((nextPos - centerPos).Length() > stageScale)
		//{
		//	//�Ԃ������ꏊ��ۑ�����
		//	OnCollideInfo hitCollides;
		//	//�X�e�[�W�ƂԂ���������
		//	std::shared_ptr<EnemyMinion> enemyMinion = std::make_shared<EnemyMinion>(GameObjectTag::Bullet);
		//	//�Ԃ������ꏊ��␳�O�̍��W�ɐݒ�
		//	enemyMinion->m_rigidbody.SetPos(nextPos);
		//	hitCollides.owner = enemyMinion;
		//	hitCollides.colider = item;
		//	//�Ԃ��������̏������Ă�
		//	hitCollides.OnCollide();
		//	//���W��␳
		//	nextPos = (nextPos - centerPos).GetNormalize() * stageScale;
		//}
		//if (nextPos.y < groundHeight)
		//{
		//	nextPos.y = groundHeight;
		//}

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

bool Physics::CheckCollide(std::shared_ptr<Collidable> first, std::shared_ptr<Collidable> second)
{
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
