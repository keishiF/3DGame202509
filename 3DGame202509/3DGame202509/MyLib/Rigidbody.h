#pragma once
#include "Vec3.h"

class Rigidbody
{
public:
	Rigidbody();
	void Init();

	Vec3 GetPos()  const { return m_pos; }
	Vec3 GetDir()  const { return m_dir; }
	Vec3 GetVelo() const { return m_velo; }

	void SetPos(Vec3 pos) { m_pos = pos; }
	void SetVelo(Vec3 velo);

private:
	Vec3 m_pos;
	Vec3 m_dir;
	Vec3 m_velo;
};

