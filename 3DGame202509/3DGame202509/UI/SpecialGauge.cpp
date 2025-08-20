#include "game.h"
#include "GameObjectManager.h"
#include "Player/Player.h"
#include "SpecialGauge.h"
#include <DxLib.h>

namespace
{
	// ƒQ[ƒW‚Ì‰¡•Ac•
	constexpr float kSpecialGaugeWidth = 200;
	constexpr float kSpecialGaugeHeight = 20;

	// •KŽE‹ZƒQ[ƒW‚Ì•`‰æˆÊ’u‚ð•â³‚·‚é‚½‚ß‚Ì’l
	constexpr int kGaugeOffset = 100;
}

SpecialGauge::SpecialGauge()
{
}

SpecialGauge::~SpecialGauge()
{
}

void SpecialGauge::Draw()
{
	auto& gameObjectManager = GameObjectManager::Instance();
	auto player = gameObjectManager.GetPlayer();
	if (!player) return;

	// •KŽE‹ZƒQ[ƒW‚Ì•`‰æˆÊ’u
	int specialGaugePosX = (Game::kScreenWidth - kSpecialGaugeWidth) * 0.5f;
	int specialGaugePosY = Game::kScreenHeight - kSpecialGaugeHeight - kGaugeOffset;

	// •KŽE‹ZƒQ[ƒW—Ê
	float specialGaugeRate = player->GetSpecialGaugeRate();

	DrawBox(specialGaugePosX, 
		specialGaugePosY,
		specialGaugePosX + kSpecialGaugeWidth, 
		specialGaugePosY + kSpecialGaugeHeight,
		0x808080, true);

	int hpBarWidth = static_cast<int>(kSpecialGaugeWidth * specialGaugeRate);
	DrawBox(specialGaugePosX, 
		specialGaugePosY,
		specialGaugePosX + hpBarWidth, 
		specialGaugePosY + kSpecialGaugeHeight,
		0x66ffff, true);

	DrawBox(specialGaugePosX, 
		specialGaugePosY,
		specialGaugePosX + kSpecialGaugeWidth, 
		specialGaugePosY + kSpecialGaugeHeight,
		0x000000, false);
}
