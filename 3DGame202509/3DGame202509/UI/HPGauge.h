#pragma once
#include "Vector3.h"

class HPGauge
{
public:
	HPGauge();
	~HPGauge();

	void Draw(float hpRate, float posX, float posY);
};

