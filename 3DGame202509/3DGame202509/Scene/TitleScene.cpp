#include "TitleScene.h"
#include "GameScene.h"
#include "SceneController.h"
#include "game.h"
#include "Input.h"
#include "DxLib.h"
#include <cassert>

namespace
{
	constexpr int kFadeInterval = 60;

	// スカイドームの拡大率
	constexpr float kSkyModelScale = 5.0f;
}

TitleScene::TitleScene(SceneController& controller) :
	SceneBase(controller),
	m_skyModel(-1),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_update(&TitleScene::FadeInUpdate),
	m_draw(&TitleScene::FadeDraw)
{
	m_skyModel = MV1LoadModel("Data/Sky/Sky_Daylight01.mv1");
	assert(m_skyModel >= 0);
	MV1SetScale(m_skyModel, VGet(kSkyModelScale, kSkyModelScale, kSkyModelScale));
}

TitleScene::~TitleScene()
{
}

void TitleScene::Update()
{
	(this->*m_update)();
}

void TitleScene::Draw()
{
	(this->*m_draw)();
}

void TitleScene::NormalUpdate()
{
	++m_blinkFrame;

	if (Input::Instance().IsPress("LB"))
	{
		m_update = &TitleScene::FadeOutUpdate;
		m_draw = &TitleScene::FadeDraw;
		m_fadeFrame = 0;
	}
}

void TitleScene::FadeInUpdate()
{
	if (--m_fadeFrame <= 0)
	{
		m_update = &TitleScene::NormalUpdate;
		m_draw = &TitleScene::NormalDraw;
	}
}

void TitleScene::FadeOutUpdate()
{
	if (m_fadeFrame++ >= kFadeInterval)
	{
		m_controller.ChangeScene(std::make_shared<GameScene>(m_controller));

		// 自分が死んでいるのでもし余計な処理が入っているとまずいのでreturn;
		return;
	}
}

void TitleScene::NormalDraw()
{
	MV1DrawModel(m_skyModel);

	// 点滅効果のための条件
	if ((m_blinkFrame / 30) % 2 == 0)
	{
		DrawString(0, 0, "Title Scene", 0x000000);
	}
}

void TitleScene::FadeDraw()
{
	MV1DrawModel(m_skyModel);

	DrawString(0, 0, "Title Scene", 0x000000);

	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255.0f));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
