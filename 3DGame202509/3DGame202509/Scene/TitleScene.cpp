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
}

TitleScene::TitleScene(SceneController& controller) :
	SceneBase(controller),
	m_fadeFrame(0),
	m_blinkFrame(0),
	m_update(&TitleScene::FadeInUpdate),
	m_draw(&TitleScene::FadeDraw)
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Update(Input& input)
{
	(this->*m_update)(input);
}

void TitleScene::Draw()
{
	(this->*m_draw)();
}

void TitleScene::NormalUpdate(Input& input)
{
	++m_blinkFrame;

	if (input.IsPress(CheckHitKeyAll()))
	{
		m_update = &TitleScene::FadeOutUpdate;
		m_draw = &TitleScene::FadeDraw;
		m_fadeFrame = 0;
	}
}

void TitleScene::FadeInUpdate(Input&)
{
	if (m_fadeFrame-- <= 0)
	{
		m_update = &TitleScene::NormalUpdate;
		m_draw = &TitleScene::NormalDraw;
	}
}

void TitleScene::FadeOutUpdate(Input&)
{
	if (m_fadeFrame++ >= kFadeInterval)
	{
		m_controller.ChangeScene(std::make_shared<GameScene>(m_controller));

		// ©•ª‚ª€‚ñ‚Å‚¢‚é‚Ì‚Å‚à‚µ—]Œv‚Èˆ—‚ª“ü‚Á‚Ä‚¢‚é‚Æ‚Ü‚¸‚¢‚Ì‚Åreturn;
		return;
	}
}

void TitleScene::NormalDraw()
{
	// “_–ÅŒø‰Ê‚Ì‚½‚ß‚ÌğŒ
	if ((m_blinkFrame / 30) % 2 == 0)
	{
		DrawString(0, 0, "Title Scene", 0xffffff);
	}
}

void TitleScene::FadeDraw()
{
	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * rate));
	DrawBox(0, 0, 1280, 720, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
