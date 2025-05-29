#pragma once

#include "Input.h"
#include "Vec3.h"
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

	// �v���C���[�̃X�e�[�g
private:
	// �ҋ@���
	void IdleInit(Input& input);
	void IdleUpdate(Input& input);

	// �ړ����
	void RunInit(Input& input);
	void RunUpdate(Input& input);

	// �e�U�����
	void ChopAttackInit(Input& input);
	void ChopAttackUpdate(Input& input);
	void SliceAttackInit(Input& input);
	void SliceAttackUpdate(Input& input);
	void StabAttackInit(Input& input);
	void StabAttackUpdate(Input& input);

	// �A�j���[�V�����֘A
private:
	struct AnimData
	{
		int attachNo = -1;
		float frame = 0.0f;
		bool isLoop = false;
		bool isEnd = false;
	};
	// �A�j���[�V�����̃A�^�b�`
	void AttachAnim(AnimData& data, const char* animName, bool isLoop);
	// �A�j���[�V�����̍X�V
	void UpdateAnim(AnimData& data);
	// �A�j���[�V�����u�����h���̍X�V
	void UpdateAnimBlend();
	// �A�j���[�V�����̕ύX
	void ChangeAnim(const char* animName, bool isLoop);

private:
	// �v���C���[�̃��f��
	int m_model;
	// �A�j���[�V����
	AnimData m_nextAnim;
	AnimData m_prevAnim;
	// �A�j���[�V���������炩�ɕύX���邽�߂̃u�����h��
	float m_blendRate;
	// �v���C���[�̈ʒu
	Vec3 m_pos;
	// �v���C���[�̈ړ���
	Vec3 m_vec;
	// �v���C���[��HP
	int m_hp;
	float m_jumpSpeed;
	float m_gravity;
	// �v���C���[�̏�ԃt���O
	bool m_isChop;
	bool m_isSlice;
	bool m_isStab;
	bool m_isRun;

	// �o�߃t���[���𑪂�
	float m_frameCount;

	// �v���C���[�̏�Ԃ��Ǘ����邽�߂̊֐��|�C���^
	using PlayerState = void(Player::*)(Input&);
	PlayerState m_state;
};

