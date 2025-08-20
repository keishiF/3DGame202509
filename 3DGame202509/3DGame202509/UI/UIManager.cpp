#include "Enemy/EnemyBoss.h"
#include "Enemy/EnemyMage.h"
#include "Enemy/EnemyMinion.h"
#include "HPGauge.h"
#include "Player/Player.h"
#include "SpecialGauge.h"
#include "StaminaGauge.h"
#include "UIManager.h"
#include <algorithm>
#include <DxLib.h>

namespace
{
	// プレイヤーのゲージを表示する位置
	// HPゲージ
	constexpr float kPlayerHPGaugePosX = 50.0f;
	constexpr float kPlayerHPGaugePosY = 50.0f;
	// スタミナゲージ
	constexpr float kPlayerStaminaGaugePosX = 50.0f;
	constexpr float kPlayerStaminaGaugePosY = 70.0f;
}

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
}

void UIManager::Draw(std::shared_ptr<Player> player,
	std::shared_ptr<EnemyBoss> boss,
	std::vector<std::shared_ptr<EnemyMinion>> minions,
	std::vector<std::shared_ptr<EnemyMage>> mages)
{
	// プレイヤーのゲージ描画
	m_hpGauge->Draw();
	m_staminaGauge->Draw(player->GetStaminaRate(), kPlayerStaminaGaugePosX, kPlayerStaminaGaugePosY);
	m_specialGauge->Draw(player->GetSpecialGaugeRate());
}
