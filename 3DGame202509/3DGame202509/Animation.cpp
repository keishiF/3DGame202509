#include "Animation.h"
#include "DxLib.h"

void Animation::Init(AnimationData prev, AnimationData next, int model, float blendRate)
{
	m_prev = prev;
	m_next = next;
	m_model = model;
	m_blendRate = blendRate;
}

void Animation::AttachAnim(AnimationData next, const char* animName, bool isLoop)
{
	// �A�^�b�`�������A�j���̔ԍ����擾
	int index = MV1GetAnimIndex(m_model, animName);
	// ���f���ɃA�j���[�V�������A�^�b�`
	next.attachNo = MV1AttachAnim(m_model, index, -1, false);
	// �A�j���J�E���^������
	next.frame = 0.0f;
	// �A�j���[�V�����̃��[�v�ݒ�
	next.isLoop = isLoop;
	// �񃋁[�v�A�j���̏I���t���O�𗎂Ƃ��Ă���
	next.isEnd = false;
}

void Animation::PrevUpdateAnim()
{
	// �A�j���[�V�������A�^�b�`����Ă��Ȃ��ꍇ�͉������Ȃ�
	if (m_prev.attachNo == -1)
	{
		return;
	}

	// �A�j���[�V������i�߂�
	m_prev.frame += 0.5f;

	// ���ݍĐ����̃A�j���[�V�����̑����Ԃ��擾����
	const float totalTime = MV1GetAttachAnimTotalTime(m_model, m_prev.attachNo);

	// �A�j���[�V�����̐ݒ�ɂ���ă��[�v�����邩�Ō�̃t���[���Ŏ~�߂邩�𔻒�
	if (m_prev.isLoop)
	{
		// �A�j���[�V���������[�v������
		while (m_prev.frame > totalTime)
		{
			m_prev.frame -= totalTime;
		}
	}
	else
	{
		// �Ō�̃t���[���Œ�~������
		if (m_prev.frame > totalTime)
		{
			m_prev.frame = totalTime;
			m_prev.isEnd = true;
		}
	}

	// �i�s�������A�j���[�V���������f���ɓK�p����
	MV1SetAttachAnimTime(m_model, m_prev.attachNo, m_prev.frame);
}

void Animation::NextUpdateAnim()
{
	// �A�j���[�V�������A�^�b�`����Ă��Ȃ��ꍇ�͉������Ȃ�
	if (m_next.attachNo == -1)
	{
		return;
	}

	// �A�j���[�V������i�߂�
	m_next.frame += 0.5f;

	// ���ݍĐ����̃A�j���[�V�����̑����Ԃ��擾����
	const float totalTime = MV1GetAttachAnimTotalTime(m_model, m_next.attachNo);

	// �A�j���[�V�����̐ݒ�ɂ���ă��[�v�����邩�Ō�̃t���[���Ŏ~�߂邩�𔻒�
	if (m_next.isLoop)
	{
		// �A�j���[�V���������[�v������
		while (m_next.frame > totalTime)
		{
			m_next.frame -= totalTime;
		}
	}
	else
	{
		// �Ō�̃t���[���Œ�~������
		if (m_next.frame > totalTime)
		{
			m_next.frame = totalTime;
			m_next.isEnd = true;
		}
	}

	// �i�s�������A�j���[�V���������f���ɓK�p����
	MV1SetAttachAnimTime(m_model, m_next.attachNo, m_next.frame);
}

void Animation::UpdateAnimBlend()
{
	// �����ɃA�j�����ݒ肳��Ă��Ȃ��ꍇ�͕ω������Ȃ�
	if (m_next.attachNo == -1) return;
	if (m_prev.attachNo == -1) return;

	// m_blendRate��0.0f -> 1.0f�ɕω�������
	m_blendRate += 1.0f / 8.0f;
	if (m_blendRate > 1.0f) m_blendRate = 1.0f;

	MV1SetAttachAnimBlendRate(m_model, m_prev.attachNo, 1.0f - m_blendRate);
	MV1SetAttachAnimBlendRate(m_model, m_next.attachNo, m_blendRate);
}

void Animation::ChangeAnim(const char* animName, bool isLoop)
{
	// �ЂƂO�̃f�[�^�͍���Ǘ��ł��Ȃ��Ȃ�̂ł��炩���߃f�^�b�`���Ă���
	MV1DetachAnim(m_model, m_prev.attachNo);

	// ���ݍĐ����̃A�j���[�V�����͈�Â��f�[�^�����ɂȂ�
	m_prev = m_next;

	// �V���ɃV���b�g�̃A�j���[�V�������A�^�b�`����
	AttachAnim(m_next, animName, isLoop);

	// �A�j���̃u�����h����0.0>1.0�ɕω�����̂�0.0�ŏ�����
	m_blendRate = 0.0f;

	// m_blendRate���A�j���[�V�����ɓK�p����
	MV1SetAttachAnimBlendRate(m_model, m_prev.attachNo, 1.0f - m_blendRate);
	MV1SetAttachAnimBlendRate(m_model, m_next.attachNo, m_blendRate);
}
