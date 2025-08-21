#include "game.h"
#include "GameObjectManager.h"
#include "Player/Player.h"
#include "SpecialGauge.h"
#include <DxLib.h>

namespace
{
	// ゲージの横幅、縦幅
	constexpr float kSpecialGaugeWidth = 200;
	constexpr float kSpecialGaugeHeight = 20;

	// 必殺技ゲージの描画位置を補正するための値
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

	// 必殺技ゲージの描画位置
	int specialGaugePosX = (Game::kScreenWidth - kSpecialGaugeWidth) * 0.5f;
	int specialGaugePosY = Game::kScreenHeight - kSpecialGaugeHeight - kGaugeOffset;

	// 必殺技ゲージ量
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
