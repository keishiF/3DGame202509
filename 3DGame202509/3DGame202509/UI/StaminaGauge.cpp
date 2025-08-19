#include "StaminaGauge.h"
#include <DxLib.h>

namespace
{
	// ÉQÅ[ÉWÇÃâ°ïùÅAècïù
	constexpr float kStaminaGaugeWidth = 200;
	constexpr float kStaminaGaugeHeight = 20;
}

StaminaGauge::StaminaGauge()
{
}

StaminaGauge::~StaminaGauge()
{
}

void StaminaGauge::Draw(float staminaRate, float posX, float posY)
{
	DrawBox(posX, posY,
		posX + kStaminaGaugeWidth, posY + kStaminaGaugeHeight,
		0x808080, true);

	int hpBarWidth = static_cast<int>(kStaminaGaugeWidth * staminaRate);
	DrawBox(posX, posY,
		posX + hpBarWidth, posY + kStaminaGaugeHeight,
		0xffff00, true);

	DrawBox(posX, posY,
		posX + kStaminaGaugeWidth, posY + kStaminaGaugeHeight,
		0x000000, false);
}
