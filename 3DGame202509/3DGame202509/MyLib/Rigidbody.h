#pragma once
#include "Vector3.h"

class Rigidbody
{
public:
	Rigidbody();
	void Init();

	Vector3 GetPos()  const { return m_pos; }
	Vector3 GetDir()  const { return m_dir; }
	Vector3 GetVelo() const { return m_velo; }
	//次の座標
	Vector3 GetNextPos() { return m_pos + m_velo; };

	void SetPos(Vector3 pos) { m_pos = pos; }
	void SetVelo(Vector3 velo);

private:
	Vector3 m_pos;
	Vector3 m_dir;
	Vector3 m_velo;
};

