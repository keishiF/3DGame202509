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
	DrawBossHP();
	DrawMageHP();
	DrawMinionHP();
	DrawPlayerHP();
}

void HPGauge::DrawPlayerHP()
{
	auto& gameObjectManager = GameObjectManager::Instance();
	auto player = gameObjectManager.GetPlayer();
	if (!player) return;

	// プレイヤーの現在のHPと最大HPを取得し
	// 現在のHPの割合を計算
	float playerCurrentHP = player->GetStatusComp().m_hp;
	float playerMaxHP = player->GetStatusComp().m_maxHP;
	float playerHPRate = playerCurrentHP / playerMaxHP;
	playerHPRate = std::clamp(playerHPRate, 0.0f, 1.0f);

	// HPが減った部分を灰色で描画
	DrawBox(kPlayerHPPosX, kPlayerHPPosY,
		kPlayerHPPosX + kPlayerHPGaugeWidth, kPlayerHPPosY + kPlayerHPGaugeHeight,
		0x808080, true);

	// 現在のHPの割合で描画するHPゲージの色を変える
	int hpColor = 0x00ff00;
	// 50%以上なら緑
	if (playerHPRate >= 0.5f)
	{
		hpColor = 0x00ff00;
	}
	// 25%以上なら黄色
	else if (playerHPRate > 0.25f)
	{
		hpColor = 0xffff00;
	}
	// それ以下なら赤
	else
	{
		hpColor = 0xff0000;
	}

	// HPゲージ部分の描画
	int hpBarWidth = static_cast<int>(kPlayerHPGaugeWidth * playerHPRate);
	DrawBox(kPlayerHPPosX, kPlayerHPPosY,
		kPlayerHPPosX + hpBarWidth, kPlayerHPPosY + kPlayerHPGaugeHeight,
		hpColor, true);

	// 境界線として塗りつぶしのない黒いボックスを描画HPゲージを少し強調
	DrawBox(kPlayerHPPosX, kPlayerHPPosY,
		kPlayerHPPosX + kPlayerHPGaugeWidth, kPlayerHPPosY + kPlayerHPGaugeHeight,
		0x000000, false);
}

void HPGauge::DrawBossHP()
{
	auto& gameObjectManager = GameObjectManager::Instance();
	auto boss = gameObjectManager.GetEnemyBoss();
	if (!boss) return;

	float bossCurrentHP = boss->GetStatusComp().m_hp;
	float bossMaxHP = boss->GetStatusComp().m_maxHP;
	float bossHPRate = bossCurrentHP / bossMaxHP;
	bossHPRate = std::clamp(bossHPRate, 0.0f, 1.0f);

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

		float mageCurrentHP = mage->GetStatusComp().m_hp;
		float mageMaxHP = mage->GetStatusComp().m_maxHP;
		float mageHPRate = mageCurrentHP / mageMaxHP;
		mageHPRate = std::clamp(mageHPRate, 0.0f, 1.0f);

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

		float minionCurrentHP = minion->GetStatusComp().m_hp;
		float minionMaxHP = minion->GetStatusComp().m_maxHP;
		float minionHPRate = minionCurrentHP / minionMaxHP;
		minionHPRate = std::clamp(minionHPRate, 0.0f, 1.0f);

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
