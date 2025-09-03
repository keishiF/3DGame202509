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
	float specialGaugePosX = (Game::kScreenWidth - kSpecialGaugeWidth) * 0.5f;
	float specialGaugePosY = Game::kScreenHeight - kSpecialGaugeHeight - kGaugeOffset;

	// 必殺技ゲージ量
	float playerCurrentSpecialGauge = player->GetSpecialGauge();
	float playerMaxSpecialGauge = player->GetMaxSpecialGauge();
	float playerSpecialGaugeRate = playerCurrentSpecialGauge / playerMaxSpecialGauge;
	playerSpecialGaugeRate = std::clamp(playerSpecialGaugeRate, 0.0f, 1.0f);

	DrawBox(static_cast<int>(specialGaugePosX), 
		static_cast<int>(specialGaugePosY),
		static_cast<int>(specialGaugePosX + kSpecialGaugeWidth),
		static_cast<int>(specialGaugePosY + kSpecialGaugeHeight),
		0x808080, true);

	int hpBarWidth = static_cast<int>(kSpecialGaugeWidth * playerSpecialGaugeRate);
	DrawBox(static_cast<int>(specialGaugePosX),
		static_cast<int>(specialGaugePosY),
		static_cast<int>(specialGaugePosX + hpBarWidth),
		static_cast<int>(specialGaugePosY + kSpecialGaugeHeight),
		0x66ffff, true);

	DrawBox(static_cast<int>(specialGaugePosX),
		static_cast<int>(specialGaugePosY),
		static_cast<int>(specialGaugePosX + kSpecialGaugeWidth),
			static_cast<int>(specialGaugePosY + kSpecialGaugeHeight),
		0x000000, false);
}
