#include "Animation.h"
#include "DxLib.h"

void Animation::AttachAnim(Animation& data, int model, const char* animName, bool isLoop)
{
	// �A�^�b�`�������A�j���̔ԍ����擾
	int index = MV1GetAnimIndex(model, animName);
	// ���f���ɃA�j���[�V�������A�^�b�`
	data.m_attachNo = MV1AttachAnim(model, index, -1, false);
	// �A�j���J�E���^������
	data.m_frame = 0.0f;
	// �A�j���[�V�����̃��[�v�ݒ�
	data.m_isLoop = isLoop;
	// �񃋁[�v�A�j���̏I���t���O�𗎂Ƃ��Ă���
	data.m_isEnd = false;
}

void Animation::UpdateAnim(Animation& data, int model)
{
	// �A�j���[�V�������A�^�b�`����Ă��Ȃ��ꍇ�͉������Ȃ�
	if (data.m_attachNo == -1)
	{
		return;
	}

	// �A�j���[�V������i�߂�
	data.m_frame += 0.5f;

	// ���ݍĐ����̃A�j���[�V�����̑����Ԃ��擾����
	const float totalTime = MV1GetAttachAnimTotalTime(model, data.m_attachNo);

	// �A�j���[�V�����̐ݒ�ɂ���ă��[�v�����邩�Ō�̃t���[���Ŏ~�߂邩�𔻒�
	if (data.m_isLoop)
	{
		// �A�j���[�V���������[�v������
		while (data.m_frame > totalTime)
		{
			data.m_frame -= totalTime;
		}
	}
	else
	{
		// �Ō�̃t���[���Œ�~������
		if (data.m_frame > totalTime)
		{
			data.m_frame = totalTime;
			data.m_isEnd = true;
		}
	}

	// �i�s�������A�j���[�V���������f���ɓK�p����
	MV1SetAttachAnimTime(model, data.m_attachNo, data.m_frame);
}

void Animation::UpdateAnimBlend(Animation& prevAnim, Animation& nextAnim, int model)
{
	// �����ɃA�j�����ݒ肳��Ă��Ȃ��ꍇ�͕ω������Ȃ�
	if (nextAnim.m_attachNo == -1) return;
	if (prevAnim.m_attachNo == -1) return;

	// m_blendRate��0.0f -> 1.0f�ɕω�������
	m_blendRate += 1.0f / 8.0f;
	if (m_blendRate > 1.0f) m_blendRate = 1.0f;

	MV1SetAttachAnimBlendRate(model, prevAnim.m_attachNo, 1.0f - m_blendRate);
	MV1SetAttachAnimBlendRate(model, nextAnim.m_attachNo, m_blendRate);
}

void Animation::ChangeAnim(int model,const char* animName, bool isLoop)
{
	// �ЂƂO�̃f�[�^�͍���Ǘ��ł��Ȃ��Ȃ�̂ł��炩���߃f�^�b�`���Ă���
	MV1DetachAnim(model, prevAnim.attachNo);

	// ���ݍĐ����̃A�j���[�V�����͈�Â��f�[�^�����ɂȂ�
	m_prevAnim = m_nextAnim;

	// �V���ɃV���b�g�̃A�j���[�V�������A�^�b�`����
	AttachAnim(m_nextAnim, animName, isLoop);

	// �A�j���̃u�����h����0.0>1.0�ɕω�����̂�0.0�ŏ�����
	m_blendRate = 0.0f;

	// m_blendRate���A�j���[�V�����ɓK�p����
	MV1SetAttachAnimBlendRate(m_model, m_prevAnim.attachNo, 1.0f - m_blendRate);
	MV1SetAttachAnimBlendRate(m_model, m_nextAnim.attachNo, m_blendRate);
}
