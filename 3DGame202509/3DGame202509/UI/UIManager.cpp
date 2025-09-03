#include "Enemy/EnemyBoss.h"
#include "Enemy/EnemyMage.h"
#include "Enemy/EnemyMinion.h"
#include "GameTimer.h"
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
	m_gameTimer = std::make_shared<GameTimer>();
	m_hpGauge = std::make_shared<HPGauge>();
	m_staminaGauge = std::make_shared<StaminaGauge>();
	m_specialGauge = std::make_shared<SpecialGauge>();
}

UIManager::~UIManager()
{
}

void UIManager::Draw()
{
	//m_gameTimer->Draw();
	// プレイヤーのゲージ描画
	m_hpGauge->Draw();
	m_staminaGauge->Draw();
	m_specialGauge->Draw();
}
