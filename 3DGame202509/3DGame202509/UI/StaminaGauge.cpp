#include "GameObjectManager.h"
#include "Player/Player.h"
#include "StaminaGauge.h"
#include <DxLib.h>

namespace
{
	// ゲージの横幅、縦幅
	constexpr float kStaminaGaugeWidth = 200;
	constexpr float kStaminaGaugeHeight = 20;

	// プレイヤーのスタミナゲージの描画位置
	constexpr int kPlayerStaminaPosX = 50;
	constexpr int kPlayerStaminaPosY = 70;
}

StaminaGauge::StaminaGauge()
{
}

StaminaGauge::~StaminaGauge()
{
}

void StaminaGauge::Draw()
{
	auto& gameObjectManager = GameObjectManager::Instance();
	auto player = gameObjectManager.GetPlayer();
	if (!player) return;

	float playerStaminaRate = player->GetStaminaRate();

	DrawBox(kPlayerStaminaPosX, 
		kPlayerStaminaPosY,
		kPlayerStaminaPosX + kStaminaGaugeWidth, 
		kPlayerStaminaPosY + kStaminaGaugeHeight,
		0x808080, true);

	int staminaBarWidth = static_cast<int>(kStaminaGaugeWidth * playerStaminaRate);
	DrawBox(kPlayerStaminaPosX, 
		kPlayerStaminaPosY,
		kPlayerStaminaPosX + staminaBarWidth, 
		kPlayerStaminaPosY + kStaminaGaugeHeight,
		0xffff00, true);

	DrawBox(kPlayerStaminaPosX, 
		kPlayerStaminaPosY,
		kPlayerStaminaPosX + kStaminaGaugeWidth, 
		kPlayerStaminaPosY + kStaminaGaugeHeight,
		0x000000, false);
}
