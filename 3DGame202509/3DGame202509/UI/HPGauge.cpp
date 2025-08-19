#include "HPGauge.h"
#include "DxLib.h"

namespace
{
	// ÉQÅ[ÉWÇÃâ°ïùÅAècïù
	constexpr float kHPGaugeWidth  = 200;
	constexpr float kHPGaugeHeight = 20;
}

HPGauge::HPGauge()
{
}

HPGauge::~HPGauge()
{
}

void HPGauge::Draw(float hpRate, float posX, float posY)
{
	DrawBox(posX, posY,
		posX + kHPGaugeWidth, posY + kHPGaugeHeight,
		0x808080, true);

	int hpBarWidth = static_cast<int>(kHPGaugeWidth * hpRate);
	DrawBox(posX, posY,
		posX + hpBarWidth, posY + kHPGaugeHeight,
		0x00ff00, true);

	DrawBox(posX, posY,
		posX + kHPGaugeWidth, posY + kHPGaugeHeight,
		0x000000, false);
}
