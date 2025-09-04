#include "game.h"
#include "Input.h"
#include "ResultScene.h"
#include "SceneController.h"
#include "ScoreManager.h"
#include "TitleScene.h"
#include <cassert>
#include <cstring>
#include <DxLib.h>

namespace
{
	constexpr float kFadeInterval = 60;

	// スカイドームの拡大率
	constexpr float kSkyModelScale = 5.0f;

    constexpr float kScoreCountUpSpeed = 100.0f;
	constexpr float kDisplayDelayFrames = 1.0f;
    constexpr float kCountUpDurationFrame = 60.0f;
}

ResultScene::ResultScene(SceneController& controller, bool isClear) :
	SceneBase(controller),
	m_skyModel(-1),
	m_bgmHandle(-1),
	m_fadeFrame(kFadeInterval),
	m_blinkFrame(0),
	m_displayState(DisplayState::SHOW_TIME),
	m_animationCounter(0),
	m_displayTimeScore(0.0f),
	m_displayEnemyScore(0.0f),
	m_displayTotalScore(0.0f),
    m_timeScoreIncrement(0.0f),
    m_enemyScoreIncrement(0.0f),
    m_isClear(isClear),
	m_update(&ResultScene::FadeInUpdate),
	m_draw(&ResultScene::FadeDraw)
{
	m_skyModel = MV1LoadModel("Data/Model/Sky/Sky_Daylight01.mv1");
	assert(m_skyModel >= 0);
	MV1SetScale(m_skyModel, VGet(kSkyModelScale, kSkyModelScale, kSkyModelScale));

	m_bgmHandle = LoadSoundMem("Data/Sound/BGM/ResultBGM.mp3");
	assert(m_bgmHandle > 0);
	ChangeVolumeSoundMem(128, m_bgmHandle);
	PlaySoundMem(m_bgmHandle, DX_PLAYTYPE_LOOP);

    m_backGroundHandle = LoadGraph("Data/UI/Result.png");
    assert(m_backGroundHandle >= 0);

	m_fontHandle = CreateFontToHandle("Impact", 48, 3, DX_FONTTYPE_ANTIALIASING_8X8);
	assert(m_fontHandle >= 0);

	// ScoreManagerから最終スコアを取得
	auto& scoreManager = ScoreManager::Instance();
	m_finalTimeScore = scoreManager.GetTimeScore();
	m_finalEnemyScore = scoreManager.GetEnemyScore();
	m_finalTotalScore = scoreManager.GetTotalScore();
	m_rank = scoreManager.GetRank();

    if (kCountUpDurationFrame > 0)
    {
        m_timeScoreIncrement = static_cast<float>(m_finalTimeScore) / kCountUpDurationFrame;
        m_enemyScoreIncrement = static_cast<float>(m_finalEnemyScore) / kCountUpDurationFrame;
    }
}

ResultScene::~ResultScene()
{
	MV1DeleteModel(m_skyModel);

	if (m_fontHandle != -1)
	{
		DeleteFontToHandle(m_fontHandle);
	}
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
    m_animationCounter++;

    switch (m_displayState)
    {
    case DisplayState::SHOW_TIME:
        if (m_displayTimeScore < m_finalTimeScore)
        {
            m_displayTimeScore += m_timeScoreIncrement;
        }
        else
        {
            m_displayTimeScore = static_cast<float>(m_finalTimeScore); // 最終値に補正
            if (m_animationCounter > kDisplayDelayFrames)
            {
                m_displayState = DisplayState::SHOW_ENEMY_SCORE;
                m_animationCounter = 0;
            }
        }
        break;

    case DisplayState::SHOW_ENEMY_SCORE:
        if (m_displayEnemyScore < m_finalEnemyScore)
        {
            m_displayEnemyScore += m_enemyScoreIncrement;
        }
        else
        {
            m_displayEnemyScore = static_cast<float>(m_finalEnemyScore); // 最終値に補正
            if (m_animationCounter > kDisplayDelayFrames)
            {
                m_displayState = DisplayState::SHOW_TOTAL;
                m_animationCounter = 0;
            }
        }
        break;

    case DisplayState::SHOW_TOTAL:
        m_displayTotalScore = m_displayTimeScore + m_displayEnemyScore;
        if (m_displayTotalScore > m_finalTotalScore)
        {
            m_displayTotalScore = static_cast<float>(m_finalTotalScore); // 最終値に補正
        }

        // 合計スコアは即時反映なので、遅延のみ待つ
        if (m_animationCounter > kDisplayDelayFrames)
        {
            m_displayState = DisplayState::SHOW_RANK;
            m_animationCounter = 0;
        }
        break;

    case DisplayState::SHOW_RANK:
        if (m_animationCounter > kDisplayDelayFrames)
        {
            m_displayState = DisplayState::DONE;
            m_animationCounter = 0;
        }
        break;

    case DisplayState::DONE:
        m_blinkFrame++;
        if (Input::GetInstance().IsTrigger("B"))
        {
            StopSoundMem(m_bgmHandle);
            m_update = &ResultScene::FadeOutUpdate;
            m_draw = &ResultScene::FadeDraw;
            m_fadeFrame = 0;
        }
        break;
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
        return;
    }
}

void ResultScene::NormalDraw()
{
    DrawGraph(0, 0, m_backGroundHandle, true);

    const char* titleText = m_isClear ? "GAME CLEAR" : "GAME OVER";
    int titleWidth = GetDrawStringWidthToHandle(titleText, static_cast<int>(strlen(titleText)), m_fontHandle);
    DrawStringToHandle(Game::kScreenWidth / 2 - titleWidth / 2, 80, titleText, 0xffd700, m_fontHandle);

    int x = 350;
    int y = 200;
    unsigned int white = 0xffffff;

    if (m_displayState >= DisplayState::SHOW_TIME)
    {
        DrawFormatStringToHandle(x, y, white, m_fontHandle, "TIME SCORE");
        DrawFormatStringToHandle(x + 450, y, white, m_fontHandle, "%7d", static_cast<int>(m_displayTimeScore));
    }
    if (m_displayState >= DisplayState::SHOW_ENEMY_SCORE)
    {
        DrawFormatStringToHandle(x, y + 80, white, m_fontHandle, "ENEMY SCORE");
        DrawFormatStringToHandle(x + 450, y + 80, white, m_fontHandle, "%7d", static_cast<int>(m_displayEnemyScore));
    }
    if (m_displayState >= DisplayState::SHOW_TOTAL)
    {
        DrawLine(x, y + 150, x + 600, y + 150, white, 2);
        DrawFormatStringToHandle(x, y + 180, white, m_fontHandle, "TOTAL SCORE");
        DrawFormatStringToHandle(x + 450, y + 180, white, m_fontHandle, "%7d", static_cast<int>(m_displayTotalScore));
    }
    if (m_displayState >= DisplayState::SHOW_RANK)
    {
        DrawFormatStringToHandle(x, y + 280, white, m_fontHandle, "RANK");
        DrawFormatStringToHandle(x + 450, y + 280, 0xffd700, m_fontHandle, "%c", m_rank);
    }

    if (m_displayState == DisplayState::DONE)
    {
        if ((m_blinkFrame / 30) % 2 == 0)
        {
            const char* continueText = "PRESS A GO TITLE";
            int continueWidth = GetDrawStringWidthToHandle(continueText, static_cast<int>(strlen(continueText)), m_fontHandle);
            DrawStringToHandle(Game::kScreenWidth / 2 - continueWidth / 2,
                Game::kScreenHeight - 100, continueText, white, m_fontHandle);
        }
    }
}

void ResultScene::FadeDraw()
{
    NormalDraw();

    float rate = static_cast<float>(m_fadeFrame) / static_cast<float>(kFadeInterval);
    SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(rate * 255.0f));
    DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
