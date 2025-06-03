#pragma once

#include <string>

class Animation
{
public:
	struct AnimData
	{
		int   attachNo =   -1;
		float frame    = 0.0f;
		bool  isLoop   = false;
		bool  isEnd    = false;
	};

	Animation();

	void Init(int model);
	void AttachAnim(AnimData& data, const char* animName, bool isLoop);
	void UpdateAnim(AnimData& data);
	void UpdateAnimBlend();
	void ChangeAnim(const char* animName, bool isLoop);

	AnimData& GetNextAnim() { return m_nextAnim; }
	AnimData& GetPrevAnim() { return m_prevAnim; }

private:
	AnimData m_nextAnim;
	AnimData m_prevAnim;
	float m_blendRate;

	int m_model;
};

