#pragma once

#include "Input.h"
#include "Vec3.h"
#include "Collidable.h"
#include "Animation.h"
#include <memory>

class PlayerWeapon;
class Player : public Collidable
{
public:
	// �R���X�g���N�^�ƃf�X�g���N�^
	Player();
	virtual ~Player();

	// �������A�X�V�A�`��
	void Init(std::shared_ptr<Physics> physics);
	void Update();
	void Draw();

	// �v���C���[�̈ʒu���擾
	Vec3 GetPos() const { return m_rigidbody.GetPos(); }
	float GetRadius() const { return m_radius; }

	// �_���[�W���󂯂����̏������܂Ƃ߂�֐�
	void OnDamage();

	bool IsDead() const { return m_isDead; }

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

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
	void IdleUpdate();

	// �ړ�
	// ����
	void WalkUpdate();
	// ����
	void RunUpdate();

	// �e�U�����
	void ChopUpdate();
	void SliceUpdate();
	void StabUpdate();
	void SpinUpdate();
	void UltimateUpdate();

	// ������
	void DodgeUpdate();

	// ��e���
	void HitUpdate();

	// ���S���
	void DeadUpdate();

private:
	// �v���C���[�̃��f��
	int m_charModel;
	// �v���C���[�̓����蔻��
	float m_radius;
	// �v���C���[��HP
	int m_hp;
	// �v���C���[�̃t���O
	bool m_isCombo;
	bool m_isDead;

	// �o�߃t���[���𑪂�
	float m_frameCount;

	// �v���C���[�̃A�j���[�V����
	Animation m_anim;

	std::shared_ptr<PlayerWeapon> m_weapon;
};

