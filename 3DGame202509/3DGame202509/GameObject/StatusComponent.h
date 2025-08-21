#pragma once

class StatusComponent
{
public:
	float GetHP()    const { return m_hp; }
	float GetMaxHP() const { return m_maxHP; }
	float GetAtk()   const { return m_atk; }

	float m_hp = 0.0f;
	float m_maxHP = 0.0f;
	float m_atk = 0.0f;
};

