#include "game.h"
#include "GameObjectManager.h"
#include "GameScene.h"
#include "Input.h"
#include "Physics.h"
#include "ResultScene.h"
#include "SceneController.h"
#include "ScoreManager.h"
#include "Stage/StageObjectManager.h"
#include "TitleScene.h"
#include "UIManager.h"
#include <cassert>
#include <DxLib.h>
#include <EffekseerForDxLib.h>

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
	m_bgmHandle(-1),
	m_frame(0),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_gameObjectManager(GameObjectManager::Instance()),
	m_update(&GameScene::FadeInUpdate),
	m_draw(&GameScene::FadeDraw)
{
	Effekseer_Init(20000);
	SetChangeScreenModeGraphicsSystemResetFlag(false);
	Effekseer_InitDistortion();

	m_bgmHandle = LoadSoundMem("Data/Sound/BGM/GameBGM.mp3");
	assert(m_bgmHandle > 0);
	ChangeVolumeSoundMem(128, m_bgmHandle);
	PlaySoundMem(m_bgmHandle, DX_PLAYTYPE_LOOP);

	m_gameObjectManager.Init();

	m_stageObjectManager = std::make_shared<StageObjectManager>();
	m_stageObjectManager->Init();

	m_uiManager = std::make_shared<UIManager>();

	// ScoreManagerのタイマーを開始
	ScoreManager::Instance().Start();
}

GameScene::~GameScene()
{
	m_gameObjectManager.Finalize();
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

	UpdateEffekseer3D();

	m_gameObjectManager.Update();
	if (m_gameObjectManager.IsGameOver())
	{
		StopSoundMem(m_bgmHandle);
		m_update = &GameScene::FadeOutUpdate;
		m_draw = &GameScene::FadeDraw;
		m_fadeFrame = 0;
	}
	else if (m_gameObjectManager.IsClear())
	{
		StopSoundMem(m_bgmHandle);
		m_update = &GameScene::FadeOutUpdate;
		m_draw = &GameScene::FadeDraw;
		m_fadeFrame = 0;
	}

	if (m_gameObjectManager.IsGameOver() || m_gameObjectManager.IsClear())
	{
		// ゲームが終了したらタイマーを停止
		ScoreManager::Instance().Stop();
		StopSoundMem(m_bgmHandle);
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
		bool isClear = m_gameObjectManager.IsClear();
		m_controller.ChangeScene(std::make_shared<ResultScene>(m_controller, isClear));

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
	m_gameObjectManager.Draw();
	m_uiManager->Draw();

	Effekseer_Sync3DSetting();
	DrawEffekseer3D();
}

void GameScene::FadeDraw()
{
	m_stageObjectManager->Draw();
	m_gameObjectManager.Draw();
	m_uiManager->Draw();

	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255.0f));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
