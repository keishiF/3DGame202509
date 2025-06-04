#pragma once

#include "Input.h"
#include "Vec3.h"
#include "Animation.h"
#include <memory>

class Player
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

	// �_���[�W���󂯂����̏������܂Ƃ߂�֐�
	void OnDamage();

	bool IsDead() const { return m_isDead; }

	// �v���C���[�̏��
private:
	// �ҋ@���
	void IdleInit(Input& input);
	void IdleUpdate(Input& input);

	// �ړ�
	// ����
	void WalkInit(Input& input);
	void WalkUpdate(Input& input);
	// ����
	void RunInit(Input& input);
	void RunUpdate(Input& input);

	// �e�U�����
	void ChopInit(Input& input);
	void ChopUpdate(Input& input);
	void SliceInit(Input& input);
	void SliceUpdate(Input& input);
	void StabInit(Input& input);
	void StabUpdate(Input& input);
	void SpinInit(Input& input);
	void SpinUpdate(Input& input);

	// ������
	void DodgeInit(Input& input);
	void DodgeUpdate(Input& input);

	// ��e���
	void HitInit(Input& input);
	void HitUpdate(Input& input);

	// ���S���
	void DeadInit(Input& input);
	void DeadUpdate(Input& input);

private:
	// �v���C���[�̃��f��
	int m_model;
	// �v���C���[�̈ʒu
	Vec3 m_pos;
	// �v���C���[�̈ړ���
	Vec3 m_vec;
	// �v���C���[��HP
	int m_hp;
	// �v���C���[�̏�ԃt���O
	bool m_isWalk;
	bool m_isRun;
	bool m_isChop;
	bool m_isSlice;
	bool m_isStab;
	bool m_isSpin;
	bool m_isDodge;
	bool m_isDead;

	// �o�߃t���[���𑪂�
	float m_frameCount;
	float m_walkFrameCount;

	// �v���C���[�̏�Ԃ��Ǘ����邽�߂̊֐��|�C���^
	using PlayerState = void(Player::*)(Input&);
	PlayerState m_state;

	// �v���C���[�̃A�j���[�V����
	Animation m_anim;
};

