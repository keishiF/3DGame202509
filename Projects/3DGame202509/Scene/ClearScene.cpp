#include "ClearScene.h"
#include "game.h"
#include "GameScene.h"
#include "Input.h"
#include "SceneController.h"
#include "TitleScene.h"
#include <cassert>
#include <DxLib.h>

namespace
{
	constexpr float kFadeInterval = 60;

	// スカイドームの拡大率
	constexpr float kSkyModelScale = 5.0f;
}

ClearScene::ClearScene(SceneController& controller) :
	SceneBase(controller),
	m_skyModel(-1),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_update(&ClearScene::FadeInUpdate),
	m_draw(&ClearScene::FadeDraw)
{
	m_skyModel = MV1LoadModel("Data/Model/Sky/Sky_Daylight01.mv1");
	assert(m_skyModel >= 0);
	MV1SetScale(m_skyModel, VGet(kSkyModelScale, kSkyModelScale, kSkyModelScale));
}

ClearScene::~ClearScene()
{
	MV1DeleteModel(m_skyModel);
}

void ClearScene::Update()
{
	(this->*m_update)();
}

void ClearScene::Draw()
{
	(this->*m_draw)();
}

void ClearScene::NormalUpdate()
{
	++m_blinkFrame;

	if (Input::GetInstance().IsPress("OK"))
	{
		m_update = &ClearScene::FadeOutUpdate;
		m_draw = &ClearScene::FadeDraw;
		m_fadeFrame = 0;
	}
}

void ClearScene::FadeInUpdate()
{
	if (--m_fadeFrame <= 0)
	{
		m_update = &ClearScene::NormalUpdate;
		m_draw = &ClearScene::NormalDraw;
	}
}

void ClearScene::FadeOutUpdate()
{
	if (m_fadeFrame++ >= kFadeInterval)
	{
		m_controller.ChangeScene(std::make_shared<TitleScene>(m_controller));

		// 自分が死んでいるのでもし余計な処理が入っているとまずいのでreturn;
		return;
	}
}

void ClearScene::NormalDraw()
{
	MV1DrawModel(m_skyModel);

#ifdef _DEBUG
	// 点滅効果のための条件
	if ((m_blinkFrame / 30) % 2 == 0)
	{
		DrawString(0, 0, "Clear Scene", 0x000000);
	}
#endif
}

void ClearScene::FadeDraw()
{
	MV1DrawModel(m_skyModel);

	float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255.0f));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
