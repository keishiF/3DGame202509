#pragma once
#include "Animator.h"
#include "Collidable.h"
#include "Input.h"
#include "Vec3.h"
#include <memory>
#include <queue>
#include <string>

enum class PlayerState
{
	Idle,       // �ҋ@
	Walk,       // ����
	Run,        // ����
	Chop,       // �U��1�i��
	Slice,      // �U��2�i��
	Stab,       // �U��3�i��
	Spin,       // ���U��
	Special,    // �K�E�Z
	DualChop,   // �K�E�Z
	DualSlice,  // �K�E�Z
	DualStab,   // �K�E�Z
	Dodge,      // ���
	Hit,        // ��e
	Dead        // ���S
};

class PlayerRightWeapon;
class PlayerLeftWeapon;
class Player : public Collidable
{
public:
	// �R���X�g���N�^�ƃf�X�g���N�^
	Player();
	virtual ~Player();

	// �������A�X�V�A�`��
	void Init(std::shared_ptr<Physics> physics);
	void Init(std::shared_ptr<Physics> physics, Vec3& pos, const Vec3& rot, const Vec3& scale);
	void Update();
	void Draw();

	// �v���C���[�̈ʒu���擾
	Vec3 GetPos() const { return m_rigidbody.GetPos(); }
	float GetRadius() const { return m_radius; }
	int GetModel() const { return m_charModel; }

	// �_���[�W���󂯂����̏������܂Ƃ߂�֐�
	void OnDamage();

	bool IsDead() const { return m_isDead; }

	virtual void OnCollide(std::shared_ptr<Collidable> collider) override;

	// �v���C���[�̏��
private:
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
	void SpecialUpdate();

	// ������
	void DodgeUpdate();

	// ��e���
	void HitUpdate();

	// ���S���
	void DeadUpdate();

	std::queue<const char*> m_ultimateAnimQueue;
	std::string m_currentSpecialAnim;

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
	float m_attackFrame;

	// �v���C���[�̃A�j���[�V����
	Animator m_anim;

	std::shared_ptr<PlayerRightWeapon> m_rightWeapon;
	std::shared_ptr<PlayerLeftWeapon> m_leftWeapon;
};

