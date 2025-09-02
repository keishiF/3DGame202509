#include "game.h"
#include "GameScene.h"
#include "Input.h"
#include "SceneController.h"
#include "TitleScene.h"
#include <cassert>
#include <DxLib.h>

namespace
{
	constexpr int kFadeInterval = 60;

	// スカイドームの拡大率
	constexpr float kSkyModelScale = 5.0f;
}

TitleScene::TitleScene(SceneController& controller) :
	SceneBase(controller),
	m_skyModel(-1),
	m_titleHandle(-1),
	m_bgmHandle(-1),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_update(&TitleScene::FadeInUpdate),
	m_draw(&TitleScene::FadeDraw)
{
	m_skyModel = MV1LoadModel("Data/Model/Sky/Sky_Daylight01.mv1");
	assert(m_skyModel >= 0);

	m_titleHandle = LoadGraph("Data/UI/Title2.png");
	assert(m_titleHandle >= 0);

	m_fontHandle = CreateFontToHandle("Algerian", 48, -1, DX_FONTTYPE_ANTIALIASING_8X8);
	assert(m_fontHandle != -1);

	m_bgmHandle = LoadSoundMem("Data/Sound/BGM/TitleBGM.mp3");
	assert(m_bgmHandle > 0);
	ChangeVolumeSoundMem(128, m_bgmHandle);
	PlaySoundMem(m_bgmHandle, DX_PLAYTYPE_LOOP);

	MV1SetScale(m_skyModel, VGet(kSkyModelScale, kSkyModelScale, kSkyModelScale));
}

TitleScene::~TitleScene()
{
	MV1DeleteModel(m_skyModel);
	StopSoundMem(m_bgmHandle);
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

	if (Input::GetInstance().IsPress("B"))
	{
		StopSoundMem(m_bgmHandle);
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
	DrawGraph(0, 0, m_titleHandle, true);

	// 点滅効果付きで「PRESS A BUTTON」「START」を中央表示
	if ((m_blinkFrame / 30) % 2 == 0)
	{
		const char* pressText = "PRESS A BUTTON";
		const char* startText = "START";
		int pressWidth = GetDrawStringWidthToHandle(pressText, strlen(pressText), m_fontHandle);
		int startWidth = GetDrawStringWidthToHandle(startText, strlen(startText), m_fontHandle);
		int centerX = Game::kScreenWidth / 2;
		int centerY = 500;

		// 「PRESS A BUTTON」を中央に描画
		DrawStringToHandle(centerX - pressWidth / 2, centerY, pressText, 0x7cfc00, m_fontHandle);

		// 「START」をその下に描画
		DrawStringToHandle(centerX - startWidth / 2, centerY + 50, startText, 0x7cfc00, m_fontHandle);
	}
}

void TitleScene::FadeDraw()
{
	MV1DrawModel(m_skyModel);
	DrawGraph(0, 0, m_titleHandle, true);

	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255.0f));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
