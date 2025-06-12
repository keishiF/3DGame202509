#pragma once

#include "Input.h"
#include "Vec3.h"
#include "Collidable.h"
#include <memory>

class Animation;
class Player : public Collidable
{
public:
	// �R���X�g���N�^�ƃf�X�g���N�^
	Player();
	virtual ~Player();

	// �X�V�A�`��
	void Update(Input& input);
	void Draw();

	// �v���C���[�̈ʒu���擾
	Vec3 GetPos() const { return m_pos; }
	float GetRadius() const { return m_radius; }

	void OnCollide(const Collidable& collider) abstract;
	// �_���[�W���󂯂����̏������܂Ƃ߂�֐�
	void OnDamage();

	bool IsDead() const { return m_isDead; }

	// �v���C���[�̏��
private:
	enum class PlayerState
	{
		Idle,     // �ҋ@
		Walk,     // ����
		Run,      // ����
		Chop,     // �U��1�i��
		Slice,    // �U��2�i��
		Stab,     // �U��3�i��
		Spin,     // ���U��
		Ultimate, // �K�E�Z
		Dodge,    // ���
		Hit,      // ��e
		Dead      // ���S
	};

	PlayerState m_state;
	void ChangeState(PlayerState newState);

	// �ҋ@���
	void IdleUpdate(Input& input);

	// �ړ�
	// ����
	void WalkUpdate(Input& input);
	// ����
	void RunUpdate(Input& input);

	// �e�U�����
	void ChopUpdate(Input& input);
	void SliceUpdate(Input& input);
	void StabUpdate(Input& input);
	void SpinUpdate(Input& input);
	void UltimateUpdate(Input& input);

	// ������
	void DodgeUpdate(Input& input);

	// ��e���
	void HitUpdate(Input& input);

	// ���S���
	void DeadUpdate(Input& input);

private:
	// �v���C���[�̃��f��
	int m_model;
	// �v���C���[�̈ʒu
	Vec3 m_pos;
	// �v���C���[�̓����蔻��
	float m_radius;
	// �v���C���[�̈ړ���
	Vec3 m_vec;
	// �v���C���[��HP
	int m_hp;
	// �v���C���[�̃t���O
	bool m_isCombo;
	bool m_isDead;

	// �o�߃t���[���𑪂�
	float m_frameCount;

	// �v���C���[�̃A�j���[�V����
	Animation *m_anim;
};

