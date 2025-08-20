#include "GameObjectManager.h"
#include "Player/Player.h"
#include "HPGauge.h"
#include <DxLib.h>

namespace
{
	// ゲージの横幅、縦幅
	constexpr float kHPGaugeWidth  = 200;
	constexpr float kHPGaugeHeight = 20;

	// プレイヤーのHPゲージの描画位置
	constexpr int kPlayerHPPosX = 50;
	constexpr int kPlayerHPPosY = 50;
}

HPGauge::HPGauge()
{
}

HPGauge::~HPGauge()
{
}

void HPGauge::Draw()
{
	DrawPlayerHP();
}

void HPGauge::DrawPlayerHP()
{
	auto& gameObjectManager = GameObjectManager::Instance();
	auto player = gameObjectManager.GetPlayer();
	if (!player) return;

	float playerHPRate = player->GetHPRate();

	DrawBox(kPlayerHPPosX, kPlayerHPPosY,
		kPlayerHPPosX + kHPGaugeWidth, kPlayerHPPosY + kHPGaugeHeight,
		0x808080, true);

	int hpBarWidth = static_cast<int>(kHPGaugeWidth * playerHPRate);
	DrawBox(kPlayerHPPosX, kPlayerHPPosY,
		kPlayerHPPosX + hpBarWidth, kPlayerHPPosY + kHPGaugeHeight,
		0x00ff00, true);

	DrawBox(kPlayerHPPosX, kPlayerHPPosY,
		kPlayerHPPosX + kHPGaugeWidth, kPlayerHPPosY + kHPGaugeHeight,
		0x000000, false);
}
