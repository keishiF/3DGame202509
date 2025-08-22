#include "game.h"
#include "GameObjectManager.h"
#include "GameScene.h"
#include "Input.h"
#include "Physics.h"
#include "ResultScene.h"
#include "SceneController.h"
#include "Stage/StageObjectManager.h"
#include "SubGameObjectManager.h"
#include "TitleScene.h"
#include "UIManager.h"
#include <cassert>
#include <DxLib.h>

namespace
{
	constexpr int kFadeInterval = 60;
	// フィールドの一辺の長さ
	constexpr float kFieldSize = 500.0f;
	// スカイドームの拡大率
	constexpr float kSkyModelScale = 5.0f;
}

GameScene::GameScene(SceneController& controller) :
	SceneBase(controller),
	m_frame(0),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_gameObjectManager(GameObjectManager::Instance()),
	m_subGameObjectManager(SubGameObjectManager::Instance()),
	m_update(&GameScene::FadeInUpdate),
	m_draw(&GameScene::FadeDraw)
{
	//m_gameObjectManager.Init();
	m_subGameObjectManager.Init();

	m_stageObjectManager = std::make_shared<StageObjectManager>();
	m_stageObjectManager->Init();

	m_uiManager = std::make_shared<UIManager>();
}

GameScene::~GameScene()
{
	//m_gameObjectManager.Finalize();
	m_subGameObjectManager.Finalize();
	if (m_stageObjectManager)
	{
		m_stageObjectManager->Finalize(); 
	}
}

void GameScene::Update()
{
	(this->*m_update)();
}

void GameScene::Draw()
{
	(this->*m_draw)();
}

void GameScene::NormalUpdate()
{
	++m_frame;
	++m_blinkFrame;

	/*m_gameObjectManager.Update();
	if (m_gameObjectManager.IsGameOver())
	{
		m_update = &GameScene::FadeOutUpdate;
		m_draw = &GameScene::FadeDraw;
		m_fadeFrame = 0;
	}
	else if (m_gameObjectManager.IsClear())
	{
		m_update = &GameScene::FadeOutUpdate;
		m_draw = &GameScene::FadeDraw;
		m_fadeFrame = 0;
	}*/

	m_subGameObjectManager.Update();
	if (m_subGameObjectManager.IsGameOver())
	{
		m_update = &GameScene::FadeOutUpdate;
		m_draw = &GameScene::FadeDraw;
		m_fadeFrame = 0;
	}
	else if (m_subGameObjectManager.IsClear())
	{
		m_update = &GameScene::FadeOutUpdate;
		m_draw = &GameScene::FadeDraw;
		m_fadeFrame = 0;
	}
}

void GameScene::FadeInUpdate()
{
	if (--m_fadeFrame <= 0)
	{
		m_update = &GameScene::NormalUpdate;
		m_draw = &GameScene::NormalDraw;
	}
}

void GameScene::FadeOutUpdate()
{
	if (m_fadeFrame++ >= kFadeInterval)
	{
		m_controller.ChangeScene(std::make_shared<ResultScene>(m_controller));

		// 自分が死んでいるのでもし余計な処理が入っているとまずいのでreturn;
		return;
	}
}

void GameScene::NormalDraw()
{
#ifdef _DEBUG
	// 点滅効果のための条件
	if ((m_blinkFrame / 30) % 2 == 0)
	{
		DrawString(0, 0, "Game Scene", 0xffffff);
	}
#endif
	printf("frame %d\n", m_frame);

	m_stageObjectManager->Draw();
	m_uiManager->Draw();

	//m_gameObjectManager.Draw();
	m_subGameObjectManager.Draw();
}

void GameScene::FadeDraw()
{
	m_stageObjectManager->Draw();
	m_uiManager->Draw();
	//m_gameObjectManager.Draw();
	m_subGameObjectManager.Draw();

	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255.0f));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
