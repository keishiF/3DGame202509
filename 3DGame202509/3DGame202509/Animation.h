#pragma once
#include "AnimationData.h"

class Animation
{
public:
	void Init(AnimationData prev, AnimationData next, int model, float blendRate);

	void AttachAnim(AnimationData next, const char* animName, bool isLoop);
	void PrevUpdateAnim();
	void NextUpdateAnim();
	void UpdateAnimBlend();
	void ChangeAnim(const char* animName, bool isLoop);

private:
	AnimationData m_prev;
	AnimationData m_next;
	int m_model;
	float m_blendRate;
};

