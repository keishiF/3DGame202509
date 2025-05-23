#include "GameScene.h"
#include "TitleScene.h"
#include "ResultScene.h"
#include "SceneController.h"
#include "Player.h"
#include "Camera.h"
#include "game.h"
#include "Input.h"
#include <cassert>

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
	m_fadeFrame(0),
	m_blinkFrame(0),
	m_skyModel(0),
	m_update(&GameScene::FadeInUpdate),
	m_draw(&GameScene::FadeDraw)
{
	m_skyModel = MV1LoadModel("Data/Model/Sky/Sky_Daylight01.mv1");
	assert(m_skyModel >= 0);
	MV1SetScale(m_skyModel, VGet(kSkyModelScale, kSkyModelScale, kSkyModelScale));

	m_player = std::make_shared<Player>();
	m_camera = std::make_shared<Camera>();
	m_camera->SetCamera(m_player);
}

GameScene::~GameScene()
{
}

void GameScene::Update(Input& input)
{
	(this->*m_update)(input);
}

void GameScene::Draw()
{
	(this->*m_draw)();
}

void GameScene::NormalUpdate(Input& input)
{
	++m_blinkFrame;

	m_player->Update(input);
	m_camera->Update(m_player);

	if (input.IsPress(PAD_INPUT_5))
	{
		m_update = &GameScene::FadeOutUpdate;
		m_draw = &GameScene::FadeDraw;
		m_fadeFrame = 0;
	}
}

void GameScene::FadeInUpdate(Input&)
{
	if (m_fadeFrame-- <= 0)
	{
		m_update = &GameScene::NormalUpdate;
		m_draw = &GameScene::NormalDraw;
	}
}

void GameScene::FadeOutUpdate(Input&)
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
	printf("frame %d\r", m_frame);

	MV1DrawModel(m_skyModel);

	DrawField();
	m_player->Draw();
}

void GameScene::FadeDraw()
{
	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * rate));
	DrawBox(0, 0, 1280, 720, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameScene::DrawField()
{
	// グリッド
	Vec3 start;
	Vec3 end;
	start = { -kFieldSize, 0.0f,0.0f };
	end = { kFieldSize, 0.0f,0.0f };
	for (int z = -kFieldSize; z <= kFieldSize; z += 50)
	{
		start.z = z;
		end.z = z;

		DrawLine3D(VGet(start.x, start.y, start.z), VGet(end.x, end.y, end.z), 0xff0000);
	}
	start = { 0.0f, 0.0f, -kFieldSize };
	end = { 0.0f, 0.0f, kFieldSize };
	for (int x = -kFieldSize; x <= kFieldSize; x += 50)
	{
		start.x = x;
		end.x = x;

		DrawLine3D(VGet(start.x, start.y, start.z), VGet(end.x, end.y, end.z), 0x0000ff);
	}
}
