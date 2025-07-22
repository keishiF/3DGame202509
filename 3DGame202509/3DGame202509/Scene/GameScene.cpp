#include "Camera.h"
#include "Enemy/EnemyMage.h"
#include "Enemy/EnemyMinion.h"
#include "game.h"
#include "GameObjectManager.h"
#include "GameScene.h"
#include "Input.h"
#include "Physics.h"
#include "Player/Player.h"
#include "ResultScene.h"
#include "SceneController.h"
#include "Stage/StageObjectManager.h"
#include "TitleScene.h"
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
	m_update(&GameScene::FadeInUpdate),
	m_draw(&GameScene::FadeDraw)
{
	GameObjectManager::Instance().Init();

	m_stageObjectManager = std::make_shared<StageObjectManager>();
	m_stageObjectManager->Init();
}

GameScene::~GameScene()
{
	GameObjectManager::Instance().Finalize();
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

	GameObjectManager::Instance().Update();
	if (GameObjectManager::Instance().IsGameOver())
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
	// 点滅効果のための条件
	if ((m_blinkFrame / 30) % 2 == 0)
	{
		DrawString(0, 0, "Game Scene", 0xffffff);
	}
	printf("frame %d\n", m_frame);

	m_stageObjectManager->Draw();
	GameObjectManager::Instance().Draw();
}

void GameScene::FadeDraw()
{
	m_stageObjectManager->Draw();
	GameObjectManager::Instance().Draw();

	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255.0f));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameScene::DrawField()
{
	// グリッド
	Vec3 start;
	Vec3 end;
	start = { -kFieldSize, 0.0f,0.0f };
	end = { kFieldSize, 0.0f,0.0f };
	for (float z = -kFieldSize; z <= kFieldSize; z += 50.0f)
	{
		start.z = z;
		end.z = z;

		DrawLine3D(VGet(start.x, start.y, start.z), VGet(end.x, end.y, end.z), 0xff0000);
	}
	start = { 0.0f, 0.0f, -kFieldSize };
	end = { 0.0f, 0.0f, kFieldSize };
	for (float x = -kFieldSize; x <= kFieldSize; x += 50.0f)
	{
		start.x = x;
		end.x = x;

		DrawLine3D(VGet(start.x, start.y, start.z), VGet(end.x, end.y, end.z), 0x00ff00);
	}
}
