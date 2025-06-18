#include "ResultScene.h"
#include "SceneController.h"
#include "TitleScene.h"
#include "Input.h"
#include "game.h"
#include "DxLib.h"
#include <cstring>
#include <string>
#include <iostream>
#include <cassert>
#include <cmath>

namespace
{
	constexpr float kFadeInterval = 60;

	// スカイドームの拡大率
	constexpr float kSkyModelScale = 5.0f;
}

ResultScene::ResultScene(SceneController& controller):
	SceneBase(controller),
	m_skyModel(-1),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_update(&ResultScene::FadeInUpdate),
	m_draw(&ResultScene::FadeDraw)
{
	m_skyModel = MV1LoadModel("Data/Sky/Sky_Daylight01.mv1");
	assert(m_skyModel >= 0);
	MV1SetScale(m_skyModel, VGet(kSkyModelScale, kSkyModelScale, kSkyModelScale));
}

ResultScene::~ResultScene()
{
}

void ResultScene::Update()
{
	(this->*m_update)();
}

void ResultScene::Draw()
{
	(this->*m_draw)();
}

void ResultScene::NormalUpdate()
{
	++m_blinkFrame;

	if (Input::Instance().IsPress("LB"))
	{
		m_update = &ResultScene::FadeOutUpdate;
		m_draw = &ResultScene::FadeDraw;
		m_fadeFrame = 0;
	}
}

void ResultScene::FadeInUpdate()
{
	if (--m_fadeFrame <= 0)
	{
		m_update = &ResultScene::NormalUpdate;
		m_draw = &ResultScene::NormalDraw;
	}
}

void ResultScene::FadeOutUpdate()
{
	if (m_fadeFrame++ >= kFadeInterval)
	{
		m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller));

		// 自分が死んでいるのでもし余計な処理が入っているとまずいのでreturn;
		return;
	}
}

void ResultScene::NormalDraw()
{
	MV1DrawModel(m_skyModel);

	// 点滅効果のための条件
	if ((m_blinkFrame / 30) % 2 == 0)
	{
		DrawString(0, 0, "Result Scene", 0x000000);
	}
}

void ResultScene::FadeDraw()
{
	MV1DrawModel(m_skyModel);

	DrawString(0, 0, "Result Scene", 0x000000);

	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255.0f));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
