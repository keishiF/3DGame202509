#include "EnemyMinionBlade.h"

#include "CapsuleColliderData.h"

#include <cassert>

namespace
{
	constexpr float kRadius = 7.5f;

	constexpr float kBladeModelScale = 0.01f;
}

EnemyMinionBlade::EnemyMinionBlade() :
	m_model(-1),
	Collidable(ObjectTag::EnemyWeapon, ObjectPriority::Low, ColliderData::Kind::Capsule)
{
}

EnemyMinionBlade::~EnemyMinionBlade()
{
}

void EnemyMinionBlade::Init(std::shared_ptr<Physics> physics)
{
	Collidable::Init(physics);
	m_rigidbody.Init();

	m_model = MV1LoadModel("Data/Enemy/Minion/BladeBlender.mv1");
	assert(m_model >= 0);
}

void EnemyMinionBlade::IdleUpdate(int model)
{
	SetActive(false);

	// �A�^�b�`���郂�f����MV1SetMatrix�̐ݒ�𖳌�������
	MV1SetMatrix(m_model, MGetIdent());
	// �A�^�b�`���郂�f���̃t���[���̍��W���擾����
	VECTOR position = MV1GetFramePosition(m_model, 0);
	// �A�^�b�`���郂�f����,�t���[���̍��W�����_�ɂ��邽�߂̕��s�ړ��s����쐬
	MATRIX transMat = MGetTranslate(VScale(position, -1.0f));
	// �A�^�b�`����郂�f���̃t���[���̍s����擾
	MATRIX frameMat = MV1GetFrameLocalWorldMatrix(model, 14);
	// �A�^�b�`���郂�f���̊g��s����擾
	MATRIX scaleMat = MGetScale(VGet(kBladeModelScale, kBladeModelScale, kBladeModelScale));
	// �A�^�b�`���郂�f���̉�]�s����擾
	MATRIX yMat = MGetRotY(DX_PI_F);
	// �e�s�������
	MATRIX mixMat = MGetIdent();
	mixMat = MMult(transMat, mixMat);
	mixMat = MMult(frameMat, mixMat);
	mixMat = MMult(scaleMat, mixMat);
	mixMat = MMult(yMat, mixMat);
	// ���������s������f���ɃZ�b�g
	MV1SetMatrix(m_model, mixMat);
}

void EnemyMinionBlade::AttackUpdate(int model)
{
	SetActive(true);

	// �A�^�b�`���郂�f����MV1SetMatrix�̐ݒ�𖳌�������
	MV1SetMatrix(m_model, MGetIdent());
	// �A�^�b�`���郂�f���̃t���[���̍��W���擾����
	VECTOR position = MV1GetFramePosition(m_model, 0);
	// �A�^�b�`���郂�f����,�t���[���̍��W�����_�ɂ��邽�߂̕��s�ړ��s����쐬
	MATRIX transMat = MGetTranslate(VScale(position, -1.0f));
	// �A�^�b�`����郂�f���̃t���[���̍s����擾
	MATRIX frameMat = MV1GetFrameLocalWorldMatrix(model, 14);
	// �A�^�b�`���郂�f���̊g��s����擾
	MATRIX scaleMat = MGetScale(VGet(kBladeModelScale, kBladeModelScale, kBladeModelScale));
	// �A�^�b�`���郂�f���̉�]�s����擾
	MATRIX yMat = MGetRotY(DX_PI_F);
	// �e�s�������
	MATRIX mixMat = MGetIdent();
	mixMat = MMult(transMat, mixMat);
	mixMat = MMult(frameMat, mixMat);
	mixMat = MMult(scaleMat, mixMat);
	mixMat = MMult(yMat, mixMat);
	// ���������s������f���ɃZ�b�g
	MV1SetMatrix(m_model, mixMat);

	//�����蔻��
	auto colData = std::dynamic_pointer_cast<CapsuleColliderData>(m_colliderData);
	colData->m_radius = kRadius;
	VECTOR start = MV1GetFramePosition(m_model, 2);
	VECTOR end = MV1GetFramePosition(m_model, 1);
	Vec3 startPos = { start.x, start.y, start.z };
	Vec3 endPos = { end.x, end.y, end.z };
	m_rigidbody.SetPos(endPos);
	colData->m_startPos = startPos;
}

void EnemyMinionBlade::Draw()
{
	MV1DrawModel(m_model);
}

void EnemyMinionBlade::OnCollide(std::shared_ptr<Collidable> collider)
{
}