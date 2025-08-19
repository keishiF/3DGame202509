#include "game.h"
#include "SpecialGauge.h"
#include <DxLib.h>

namespace
{
	// ƒQ[ƒW‚Ì‰¡•Ac•
	constexpr float kSpecialGaugeWidth = 200;
	constexpr float kSpecialGaugeHeight = 20;

	// •KE‹ZƒQ[ƒW‚Ì•`‰æˆÊ’u‚ğ•â³‚·‚é‚½‚ß‚Ì’l
	constexpr int kGaugeOffset = 100;
}

SpecialGauge::SpecialGauge()
{
}

SpecialGauge::~SpecialGauge()
{
}

void SpecialGauge::Draw(float specialGaugeRate)
{
	// •KE‹ZƒQ[ƒW‚Ì•`‰æˆÊ’u
	int specialGaugePosX = (Game::kScreenWidth - kSpecialGaugeWidth) * 0.5f;
	int specialGaugePosY = Game::kScreenHeight - kSpecialGaugeHeight - kGaugeOffset;

	DrawBox(specialGaugePosX, specialGaugePosY,
		specialGaugePosX + kSpecialGaugeWidth, specialGaugePosY + kSpecialGaugeHeight,
		0x808080, true);

	int hpBarWidth = static_cast<int>(kSpecialGaugeWidth * specialGaugeRate);
	DrawBox(specialGaugePosX, specialGaugePosY,
		specialGaugePosX + hpBarWidth, specialGaugePosY + kSpecialGaugeHeight,
		0x66ffff, true);

	DrawBox(specialGaugePosX, specialGaugePosY,
		specialGaugePosX + kSpecialGaugeWidth, specialGaugePosY + kSpecialGaugeHeight,
		0x000000, false);
}
