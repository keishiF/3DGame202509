#include "Enemy/EnemyBoss.h"
#include "Enemy/EnemyMage.h"
#include "Enemy/EnemyMinion.h"
#include "GameObjectManager.h"
#include "HPGauge.h"
#include "Player/Player.h"
#include <DxLib.h>

namespace
{
	// ゲージの横幅、縦幅
	constexpr float kPlayerHPGaugeWidth  = 200;
	constexpr float kPlayerHPGaugeHeight = 20;

	constexpr float kEnemyHPGaugeWidth = 100;
	constexpr float kEnemyHPGaugeHeight= 10;

	constexpr float kBossHPGaugeWidth  = 200;
	constexpr float kBossHPGaugeHeight = 20;

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
	DrawBossHP();
	DrawMageHP();
	DrawMinionHP();
}

void HPGauge::DrawPlayerHP()
{
	auto& gameObjectManager = GameObjectManager::Instance();
	auto player = gameObjectManager.GetPlayer();
	if (!player) return;

	float playerHPRate = player->GetHPRate();

	DrawBox(kPlayerHPPosX, kPlayerHPPosY,
		kPlayerHPPosX + kPlayerHPGaugeWidth, kPlayerHPPosY + kPlayerHPGaugeHeight,
		0x808080, true);

	int hpBarWidth = static_cast<int>(kPlayerHPGaugeWidth * playerHPRate);
	DrawBox(kPlayerHPPosX, kPlayerHPPosY,
		kPlayerHPPosX + hpBarWidth, kPlayerHPPosY + kPlayerHPGaugeHeight,
		0x00ff00, true);

	DrawBox(kPlayerHPPosX, kPlayerHPPosY,
		kPlayerHPPosX + kPlayerHPGaugeWidth, kPlayerHPPosY + kPlayerHPGaugeHeight,
		0x000000, false);
}

void HPGauge::DrawBossHP()
{
	auto& gameObjectManager = GameObjectManager::Instance();
	auto boss = gameObjectManager.GetEnemyBoss();
	if (!boss) return;

	float bossHPRate = boss->GetHpRate();

	float bossPosX = boss->GetScreenPos().x;
	float bossPosY = boss->GetScreenPos().y;

	DrawBox(bossPosX, bossPosY,
		bossPosX + kBossHPGaugeWidth, bossPosY + kBossHPGaugeHeight,
		0x808080, true);

	int hpBarWidth = static_cast<int>(kBossHPGaugeWidth * bossHPRate);
	DrawBox(bossPosX, bossPosY,
		bossPosX + hpBarWidth, bossPosY + kBossHPGaugeHeight,
		0xff0000, true);

	DrawBox(bossPosX, bossPosY,
		bossPosX + kBossHPGaugeWidth, bossPosY + kBossHPGaugeHeight,
		0x000000, false);
}

void HPGauge::DrawMageHP()
{
	auto& gameObjectManager = GameObjectManager::Instance();
	auto mages = gameObjectManager.GetEnemyMages();

	for (auto& mage : mages)
	{
		// nullの時はcontinue
		if (!mage) continue;

		float mageHPRate = mage->GetHpRate();

		// nullじゃなくてHPがないときもcontinue
		if (mageHPRate <= 0.0f) continue;

		float magePosX = mage->GetScreenPos().x;
		float magePosY = mage->GetScreenPos().y;

		DrawBox(magePosX, magePosY,
			magePosX + kEnemyHPGaugeWidth, magePosY + kEnemyHPGaugeHeight,
			0x808080, true);

		int hpBarWidth = static_cast<int>(kEnemyHPGaugeWidth * mageHPRate);
		DrawBox(magePosX, magePosY,
			magePosX + hpBarWidth, magePosY + kEnemyHPGaugeHeight,
			0xff0000, true);

		DrawBox(magePosX, magePosY,
			magePosX + kEnemyHPGaugeWidth, magePosY + kEnemyHPGaugeHeight,
			0x000000, false);
	}
}

void HPGauge::DrawMinionHP()
{
	auto& gameObjectManager = GameObjectManager::Instance();
	auto minions = gameObjectManager.GetEnemyMinions();

	for (auto& minion : minions)
	{
		// nullの時はcontinue
		if (!minion) continue;

		float minionHPRate = minion->GetHpRate();

		// HPがない場合はcontinue
		if (minionHPRate <= 0.0f) continue;

		float minionPosX = minion->GetScreenPos().x;
		float minionPosY = minion->GetScreenPos().y;

		DrawBox(minionPosX, minionPosY,
			minionPosX + kEnemyHPGaugeWidth, minionPosY + kEnemyHPGaugeHeight,
			0x808080, true);

		int hpBarWidth = static_cast<int>(kEnemyHPGaugeWidth * minionHPRate);
		DrawBox(minionPosX, minionPosY,
			minionPosX + hpBarWidth, minionPosY + kEnemyHPGaugeHeight,
			0xff0000, true);

		DrawBox(minionPosX, minionPosY,
			minionPosX + kEnemyHPGaugeWidth, minionPosY + kEnemyHPGaugeHeight,
			0x000000, false);
	}
}
