#pragma once

class Animation
{
public:
	void Init();
	void AttachAnim(Animation& data, int model, const char* animName, bool isLoop);
	void UpdateAnim(Animation& data, int model);
	void UpdateAnimBlend(Animation& prevAnim, Animation& nextAnim, int model);
	void ChangeAnim(int model, const char* animName, bool isLoop);

private:
	int   m_attachNo;
	float m_frame;
	bool  m_isEnd;
	bool  m_isLoop;
};